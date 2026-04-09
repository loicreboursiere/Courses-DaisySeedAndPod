#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisyPod pod;

Oscillator osc;
AdEnv      env;
Parameter  decayTimeParam, pitchParam, tickFreqParam, filterFreqParam;
Metro      tick;
MoogLadder flt;

bool    edit;
bool    editCycle;
uint8_t step;
uint8_t wave;
float   dec[8];
float   pitch[8];
bool    active[8];
float   env_out;

// --- EXTENSION : Changement de gamme ---
// Remplacement du tableau pent[] unique par une banque de 3 gammes.
// pitchIndex[8] stocke l'index de note (0-9) de chaque pas,
// ce qui permet de recalculer le pitch dans n'importe quelle gamme.

const int NUM_SCALES  = 3;
uint8_t   currentScale = 0;

// 3 gammes pentatoniques, chacune sur la note fondamentale La (110 Hz)
// Chaque ligne contient les 5 intervalles de base (1 octave)
// L'octave supérieure est générée en multipliant par 2 dans le calcul
float scales[NUM_SCALES][5] = {
    {110.f, 123.47f, 138.59f, 164.81f, 185.f},  // Pentatonique majeure  (La)
    {110.f, 130.81f, 146.83f, 164.81f, 196.f},  // Pentatonique mineure  (La)
    {110.f, 123.47f, 146.83f, 164.81f, 185.f},  // In Sen (japonaise)    (La)
};

// Index de note (0-9) pour chaque pas — remplace le stockage direct en Hz
int pitchIndex[8];

// Couleurs associées aux 3 gammes pour feedback visuel sur LED 1 en mode play
// Blanc = majeure, Bleu = mineure, Cyan = japonaise
Color scaleColors[NUM_SCALES];

// Calcule la fréquence Hz depuis un index (0-9) et la gamme active
float IndexToFreq(int index, uint8_t scale)
{
    return scales[scale][index % 5] * (index / 5 + 1);
}

// Recalcule les pitchs de tous les pas avec la gamme courante
void ApplyScaleToPitches()
{
    for(int i = 0; i < 8; i++)
    {
        pitch[i] = IndexToFreq(pitchIndex[i], currentScale);
    }
}
// --- FIN EXTENSION ---

Color colors[8];

float oldk1, oldk2;
float tickFrequency, filterFrequency;
float k1, k2;

void Controls();
void NextSamples(float &sig);

static void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t                                size)
{
    float sig;
    Controls();
    sig = 0;
    for(size_t i = 0; i < size; i += 2)
    {
        NextSamples(sig);
        out[i]     = sig;
        out[i + 1] = sig;
    }
}

int main(void)
{
    float sample_rate;
    pod.Init();
    pod.SetAudioBlockSize(4);
    sample_rate     = pod.AudioSampleRate();
    tickFrequency   = 3.f;
    filterFrequency = 20000.f;
    k1 = k2 = 0.f;

    osc.Init(sample_rate);
    env.Init(sample_rate);
    tick.Init(3, sample_rate);
    flt.Init(sample_rate);

    decayTimeParam.Init(pod.knob1, .03, 1, decayTimeParam.LINEAR);
    pitchParam.Init(pod.knob2, 0, 10, pitchParam.LINEAR);
    tickFreqParam.Init(pod.knob1, 4, 13, tickFreqParam.LINEAR);
    filterFreqParam.Init(pod.knob2, 100, 10000, filterFreqParam.LOGARITHMIC);

    osc.SetWaveform(osc.WAVE_TRI);

    env.SetTime(ADENV_SEG_ATTACK, 0.01);
    env.SetMin(0.0);
    env.SetMax(1);

    flt.SetFreq(20000);
    flt.SetRes(0.7);

    oldk1 = oldk2 = 0;
    edit           = true;
    editCycle      = false;
    step           = 0;
    env_out        = 0;
    wave           = 2;
    currentScale   = 0;

    for(int i = 0; i < 8; i++)
    {
        dec[i]        = .5;
        active[i]     = false;
        pitchIndex[i] = 0;               // Index de départ : note 0 de la gamme
        pitch[i]      = scales[0][0];    // Fréquence correspondante
        colors[i].Init((Color::PresetColor)i);
    }
    colors[7].Init(1, 1, 0);

    // Couleurs de feedback pour chaque gamme
    scaleColors[0].Init(1.f, 1.f, 1.f);   // Blanc  = pentatonique majeure
    scaleColors[1].Init(0.f, 0.f, 1.f);   // Bleu   = pentatonique mineure
    scaleColors[2].Init(0.f, 1.f, 1.f);   // Cyan   = japonaise (In Sen)

    pod.StartAdc();
    pod.StartAudio(AudioCallback);

    while(1) {}
}

void ConditionalParameter(float o, float n, float &param, float update)
{
    if(abs(o - n) > 0.00005)
    {
        param = update;
    }
}

void UpdateEncoderPressed()
{
    if(pod.encoder.RisingEdge())
    {
        edit = !edit;
        step = 0;
    }
    if(edit == false)
    {
        editCycle = false;
    }
}

void UpdateEncoderIncrement()
{
    if(edit)
    {
        step += pod.encoder.Increment();
        step = (step % 8 + 8) % 8;
    }
    else
    {
        wave += pod.encoder.Increment();
        wave = ((wave % 2 + 2) % 2);
        osc.SetWaveform(wave + 3);
    }
}

void UpdateButtons()
{
    if(edit)
    {
        if(pod.button2.RisingEdge())
        {
            active[step] = !active[step];
        }
        if(pod.button1.RisingEdge())
        {
            editCycle = !editCycle;
        }
    }

    // --- EXTENSION : Changement de gamme ---
    // En mode play, Button 1 cycle entre les gammes disponibles.
    // Tous les pitchs sont recalculés immédiatement avec la nouvelle gamme.
    else
    {
        if(pod.button1.RisingEdge())
        {
            currentScale = (currentScale + 1) % NUM_SCALES;
            ApplyScaleToPitches();
        }
    }
    // --- FIN EXTENSION ---
}

void UpdateKnobs()
{
    k1 = pod.knob1.Process();
    k2 = pod.knob2.Process();

    if(edit)
    {
        ConditionalParameter(oldk1, k1, dec[step], decayTimeParam.Process());

        // --- EXTENSION : stockage de l'index au lieu de la fréquence brute ---
        // On récupère l'index (0-9) depuis le potentiomètre,
        // on le stocke dans pitchIndex, puis on calcule la fréquence
        // via la gamme active avec IndexToFreq().
        int temp = (int)pitchParam.Process();
        if(abs(oldk2 - k2) > 0.00005)
        {
            pitchIndex[step] = temp;
            pitch[step]      = IndexToFreq(temp, currentScale);
        }
        // --- FIN EXTENSION ---

        osc.SetFreq(pitch[step]);
    }
    else
    {
        ConditionalParameter(oldk1, k1, tickFrequency, tickFreqParam.Process());
        ConditionalParameter(
            oldk2, k2, filterFrequency, filterFreqParam.Process());

        tick.SetFreq(tickFrequency);
        flt.SetFreq(filterFrequency);
    }

    oldk1 = k1;
    oldk2 = k2;
}

void UpdateLedEdit()
{
    Color cur_color = colors[step];
    pod.led1.SetColor(cur_color);

    if(active[step])
    {
        pod.led2.SetColor(cur_color);
    }
    else
    {
        pod.led2.Set(0, 0, 0);
    }

    pod.UpdateLeds();
}

void UpdateLeds()
{
    if(edit)
    {
        UpdateLedEdit();
    }
    else
    {
        // --- EXTENSION : LED 1 indique la gamme active par sa couleur ---
        // Blanc = majeure, Bleu = mineure, Cyan = japonaise
        pod.led1.SetColor(scaleColors[currentScale]);
        // --- FIN EXTENSION ---

        pod.led2.Set(0, env_out, 0);
    }
    pod.UpdateLeds();
}

void Controls()
{
    pod.ProcessDigitalControls();
    pod.ProcessAnalogControls();

    UpdateEncoderPressed();
    UpdateEncoderIncrement();
    UpdateButtons();
    UpdateKnobs();
    UpdateLeds();
}

void NextSamples(float &sig)
{
    env_out = env.Process();
    osc.SetAmp(env_out);
    sig = osc.Process();
    sig = flt.Process(sig);

    if(tick.Process() && !edit)
    {
        step++;
        step %= 8;
        if(active[step])
        {
            env.Trigger();
        }
    }

    if(active[step])
    {
        env.SetTime(ADENV_SEG_DECAY, dec[step]);
        osc.SetFreq(pitch[step]);
    }
    if(!env.IsRunning() && editCycle)
    {
        env.Trigger();
    }
}
