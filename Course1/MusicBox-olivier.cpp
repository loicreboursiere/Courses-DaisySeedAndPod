#include "daisy_pod.h"
#include "daisysp.h"
#include "fatfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace daisy;
using namespace daisysp;

const int MAX_VOICES = 8;
const int MAX_NOTES  = 20000;

DaisyPod   hw;
Oscillator osc[MAX_VOICES]; 
AdEnv      env[MAX_VOICES]; 
Svf        filt;
ReverbSc   verb;
Parameter  p_xf, p_speed; 

SdmmcHandler   sdcard;
FatFSInterface fsi;
FIL            fichier; 

struct NoteEvent { float time_sec; int midiNote; };
NoteEvent DSY_SDRAM_BSS mon_morceau[MAX_NOTES];

int   total_notes = 0; 
float loop_length = 0.0f; 
int   current_step = 0;
float current_time_sec = 0.0f;
int   current_voice = 0; 

char playlist[30][64]; 
int  total_tracks = 0;
int  current_track_index = 0;

bool playing = false;
volatile bool change_track = false;

void BuildPlaylist() {
    DIR dir; FILINFO fno; total_tracks = 0;
    if (f_opendir(&dir, "music") == FR_OK) {
        while (f_readdir(&dir, &fno) == FR_OK && fno.fname[0] != 0 && total_tracks < 30) {
            if (fno.fname[0] == '.') continue;
            if (strstr(fno.fname, ".csv") || strstr(fno.fname, ".CSV")) {
                strcpy(playlist[total_tracks++], fno.fname);
            }
        }
        f_closedir(&dir);
    }
}

void LoadTrack(int index) {
    char filepath[128];
    sprintf(filepath, "music/%s", playlist[index]);
    if(f_open(&fichier, filepath, FA_READ) == FR_OK) {
        total_notes = 0; char ligne[64];
        while(f_gets(ligne, sizeof(ligne), &fichier) && total_notes < MAX_NOTES) {
            char* v = strchr(ligne, ',');
            if (v) {
                *v = '\0';
                mon_morceau[total_notes].time_sec = atof(ligne);
                mon_morceau[total_notes].midiNote = atoi(v + 1);
                total_notes++;
            }
        }
        f_close(&fichier);
        loop_length = (total_notes > 0) ? mon_morceau[total_notes-1].time_sec + 2.0f : 0;
        current_step = 0; current_time_sec = 0;
    }
}

static void audio(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size)
{
    hw.ProcessDigitalControls();
    if(hw.button1.RisingEdge()) playing = !playing;
    if(hw.button2.RisingEdge()) change_track = true;

    // Knob 1 : Vitesse (0.5x au centre, 2x à droite)
    float speed_mult = 0.2f + (hw.knob1.Process() * 1.8f);
    // Knob 2 : Filtre
    filt.SetFreq(p_xf.Process());

    float sample_dur = 1.0f / hw.AudioSampleRate();

    for(size_t i = 0; i < size; i += 2) {
        if (playing && !change_track) {
            current_time_sec += sample_dur * speed_mult;
            while (current_step < total_notes && current_time_sec >= mon_morceau[current_step].time_sec) {
                osc[current_voice].SetFreq(mtof(mon_morceau[current_step].midiNote));
                env[current_voice].Trigger();
                current_voice = (current_voice + 1) % MAX_VOICES;
                current_step++;
            }
            if (current_time_sec >= loop_length) { current_time_sec = 0; current_step = 0; }
        }

        float rawsig = 0;
        for(int v=0; v<MAX_VOICES; v++) 
        {
            rawsig += osc[v].Process() * env[v].Process();
        }
        
        filt.Process(rawsig);
        float sig = filt.Low();
        float verbl, verbr;
        verb.Process(sig * 0.2f, sig * 0.2f, &verbl, &verbr);

        out[i] = (sig + verbl) * 0.5f;
        out[i+1] = (sig + verbr) * 0.5f;
    }
}

int main(void) {
    hw.Init();
    SdmmcHandler::Config sd_cfg; sd_cfg.Defaults(); sdcard.Init(sd_cfg);
    FatFSInterface::Config fsi_cfg; fsi_cfg.media = FatFSInterface::Config::MEDIA_SD; fsi.Init(fsi_cfg);
    f_mount(&fsi.GetSDFileSystem(), "/", 1);

    BuildPlaylist();
    LoadTrack(0);

    p_xf.Init(hw.knob2, 100.0f, 20000.0f, Parameter::LOGARITHMIC);
    for(int i=0; i<MAX_VOICES; i++) {
        osc[i].Init(hw.AudioSampleRate());
        osc[i].SetWaveform(Oscillator::WAVE_POLYBLEP_TRI);
        osc[i].SetAmp(0.08f); // Volume réduit pour 8 voix
        env[i].Init(hw.AudioSampleRate());
        env[i].SetTime(ADENV_SEG_ATTACK, 0.01f);
        env[i].SetTime(ADENV_SEG_DECAY, 0.5f);
    }
    filt.Init(hw.AudioSampleRate());
    verb.Init(hw.AudioSampleRate());

    hw.StartAdc();
    hw.StartAudio(audio);
    while(1) {
        if(change_track) {
            current_track_index = (current_track_index + 1) % total_tracks;
            LoadTrack(current_track_index);
            change_track = false;
        }
        System::Delay(10);
    }
}