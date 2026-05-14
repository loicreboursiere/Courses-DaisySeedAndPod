//Chanson de la choral des fantômes en ajoutant les accords (ghost choir)

#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisyPod   hw; //objet matériel avec : boutons, pota, audio, LEDs
Oscillator osc[4]; //tableau de 4 oscilloscopes
Parameter  p_freq, p_inversion; //objets qui lisent les potentiomètres
int        notes[4]; //tableau de 4 notes (format MIDI)
int        chord[10][3]; //tableau de 10 accords à 3 intervalles
Color      colors[10]; //tableau 10 couleurs : 1 pour chaque accords
int        chordNum = 0; //numéro de l'accord actuel, change quand l'encodeur est tourné

//notes
// Octave -1
#define Do_1 0
#define DoDies_1 1
#define Reb_1 1
#define Re_1 2
#define ReDies_1 3
#define Mib_1 3
#define Mi_1 4
#define Fa_1 5
#define FaDies_1 6
#define Solb_1 6
#define Sol_1 7
#define SolDies_1 8
#define Lab_1 8
#define La_1 9
#define LaDies_1 10
#define Sib_1 10
#define Si_1 11

// Octave 0
#define Do0 12
#define DoDies0 13
#define Re0 14
#define ReDies0 15
#define Mi0 16
#define Fa0 17
#define FaDies0 18
#define Sol0 19
#define SolDies0 20
#define La0 21
#define LaDies0 22
#define Si0 23

// Octave 1
#define Do1 24
#define DoDies1 25
#define Re1 26
#define ReDies1 27
#define Mi1 28
#define Fa1 29
#define FaDies1 30
#define Sol1 31
#define SolDies1 32
#define La1 33
#define LaDies1 34
#define Si1 35

// Octave 2
#define Do2 36
#define DoDies2 37
#define Re2 38
#define ReDies2 39
#define Mi2 40
#define Fa2 41
#define FaDies2 42
#define Sol2 43
#define SolDies2 44
#define La2 45
#define LaDies2 46
#define Si2 47

// Octave 3
#define Do3 48
#define DoDies3 49
#define Re3 50
#define ReDies3 51
#define Mi3 52
#define Fa3 53
#define FaDies3 54
#define Sol3 55
#define SolDies3 56
#define La3 57
#define LaDies3 58
#define Si3 59

// Octave 4 (Middle C)
#define Do4 60
#define DoDies4 61
#define Re4 62
#define ReDies4 63
#define Mi4 64
#define Fa4 65
#define FaDies4 66
#define Sol4 67
#define SolDies4 68
#define La4 69  // La440
#define LaDies4 70
#define Si4 71

// Octave 5
#define Do5 72
#define DoDies5 73
#define Re5 74
#define ReDies5 75
#define Mi5 76
#define Fa5 77
#define FaDies5 78
#define Sol5 79
#define SolDies5 80
#define La5 81
#define LaDies5 82
#define Si5 83

// Octave 6
#define Do6 84
#define DoDies6 85
#define Re6 86
#define ReDies6 87
#define Mi6 88
#define Fa6 89
#define FaDies6 90
#define Sol6 91
#define SolDies6 92
#define La6 93
#define LaDies6 94
#define Si6 95

// Octave 7
#define Do7 96
#define DoDies7 97
#define Re7 98
#define ReDies7 99
#define Mi7 100
#define Fa7 101
#define FaDies7 102
#define Sol7 103
#define SolDies7 104
#define La7 105
#define LaDies7 106
#define Si7 107

// Octave 8
#define Do8 108
#define DoDies8 109
#define Re8 110
#define ReDies8 111
#define Mi8 112
#define Fa8 113
#define FaDies8 114
#define Sol8 115
#define SolDies8 116
#define La8 117
#define LaDies8 118
#define Si8 119

int        melodie1_t1[4][72] = {

    {Sol4, Sol4, Sol4, Sol4, Sol4, Sol4, Sol4, Sol4, Si4, Si4, Si4, Si4, LaDies4, LaDies4, LaDies4, LaDies4, 
        La4, La4, La4, La4, La4, La4, La4, La4, DoDies5, DoDies5, DoDies5, DoDies5, DoDies5, DoDies5, DoDies5, DoDies5, 
        Si4, Si4, Si4, Si4, Si4, Si4, Si4, Si4, ReDies5, ReDies5, ReDies5, ReDies5, Re5, Re5, Re5, Re5, 
        DoDies5, DoDies5, DoDies5, DoDies5, Mi5, Mi5, DoDies5, DoDies5, La4, La4, Sol4, Sol4, Sol4, Sol4, Fa4, Fa4, Fa4, Fa4}, 
        
        //Sol4, Sol4, Sol4, Sol4, Sol4, Sol4, Sol4, Sol4, Si4, Si4, Si4, Si4, LaDies4, LaDies4, LaDies4, LaDies4, 
        //La4, La4, La4, La4, La4, La4, La4, La4, DoDies5, DoDies5, DoDies5, DoDies5, DoDies5, DoDies5, DoDies5, DoDies5},
    
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, La4, La4, La4, La4, Sol4, Sol4, Sol4, Sol4, 
        SolDies4, SolDies4, SolDies4, SolDies4, SolDies4, SolDies4, SolDies4, SolDies4, Si4, Si4, Si4, Si4, LaDies4, LaDies4, LaDies4, LaDies4, 
        La4, La4, La4, La4, DoDies5, DoDies5, La4, La4, Fa4, Fa4, Re4, Re4, Re4, Re4, Re4, Re4, Re4, Re4}, 
        
        //Mi4, Mi4, Mi4, Mi4, Mi4, Mi4, Mi4, Mi4, SolDies4, SolDies4, SolDies4, SolDies4, Sol4, Sol4, Sol4, Sol4, 
        //Fa4, Fa4, Fa4, Fa4, Fa4, Fa4, Fa4, Fa4, La4, La4, La4, La4, Sol4, Sol4, Sol4, Sol4},
    
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, SolDies4, SolDies4, SolDies4, SolDies4, Sol4, Sol4, Sol4, Sol4, 
        Fa4, Fa4, Fa4, Fa4, Fa4, Fa4, Fa4, Fa4, Fa4, Fa4, Si3, Si3, Si3, Si3, Fa3, Fa3, Fa3, Fa3}, 
        
        //Si3, Si3, Si3, Si3, Si3, Si3, Si3, Si3, ReDies4, ReDies4, ReDies4, ReDies4, Re4, Re4, Re4, Re4, 
        //Do4, Do4, Do4, Do4, Do4, Do4, Do4, Do4, Fa4, Fa4, Fa4, Fa4, Re4, Re4, Re4, Re4},

    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},

};


int freq = 69;
int step = 0;
int counter = 0;
int length = 71;

void UpdateControls(); //lit les potentiomètres, boutons, encodeur

//génération du son, choix de la fréquence
static void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t                                size)
{

    /*float samplerate;

    samplerate = hw.AudioSampleRate();

    counter++;

    if(counter >= samplerate/20) //pour avoir un changement de note toutes les 56 ms
    {
        counter=0;
        
        for(int i=0; i<4; i++) {
            notes[i] = melodie1_t1[i][step];
            }

        step++;
        if(step>=length)
        {
            step = 0;
        }
    }*/

    UpdateControls(); // lit la valeur des pota

    for(int i=0; i<4; i++) {
            osc[i].SetFreq(mtof(notes[i])); //envoie une freq def à chaque oscillo
        }

    // Audio Loop
    for(size_t i = 0; i < size; i += 2)
    {
        float sig = 0; 
        for(int i = 0; i < 4; i++)
        {
            sig += osc[i].Process(); //génère le signal sur chaque oscillo
        }

        //buffer audio de sortie, sig = son calculé, out = buffer audio de sortie
        out[i]     = sig; //gauche
        out[i + 1] = sig; //droit
        // même son des deux côtés
    }
}

// Initialise le synthétiseur de l'onde
void InitSynth(float samplerate)
{
    // Init freq Parameter to knob1 using MIDI note numbers
    //Initialise les fréquences associées au pota de 0 à 127 en notes MIDI
    // min 10, max 127, curve linear
    p_freq.Init(hw.knob1, 0, 127, Parameter::LINEAR); //hauteur de base de la note en MIDI : de 0 à 127
    p_inversion.Init(hw.knob2, 0, 5, Parameter::LINEAR); //Inversion d'accord : de 0 à 5

    for(int i = 0; i < 4; i++) //Initialise les paramètres de l'onde : amplitude, forme, frequence  echantillon
    {
        osc[i].Init(samplerate); // fréquence d'échantillonnage
        osc[i].SetAmp(0.1f); //l'amplitude choisie est basse pour éviter l'écrêtage et la saturation
        osc[i].SetWaveform(Oscillator::WAVE_SIN); //met la forme d'onde à une sinusoïde
        notes[i] = 30; //init toutes les valeurs des notes à 30 (en MIDI)
    }
}

void InitChords() //initialise le tableau des accords avec 10 accords de 4 notes
{
    // Les intervalles de notes utilisées : 
    // Maj, min, Aug, Dim
    // Maj7, min7, dom7, min/Maj7
    // dim7, half dim7

    //set thirds
    for(int i = 0; i < 8; i++)
    {
        //every other chord, maj third, min third
        chord[i][0] = 3 + ((i + 1) % 2);
    }
    //min 3rds
    chord[8][0] = chord[9][0] = 3;

    //set fifths
    // perfect 5th
    chord[0][1] = chord[1][1] = chord[4][1] = chord[5][1] = chord[6][1]
        = chord[7][1]                                     = 7;
    // diminished 5th
    chord[3][1] = chord[8][1] = chord[9][1] = 6;
    // augmented 5th
    chord[2][1] = 8;

    //set sevenths
    // triads (octave since triad has no 7th)
    chord[0][2] = chord[1][2] = chord[2][2] = chord[3][2] = 12;
    // major 7th
    chord[4][2] = chord[7][2] = 11;
    // minor 7th
    chord[5][2] = chord[6][2] = chord[9][2] = 10;
    // diminished 7th
    chord[8][2] = 9;
}

void InitColors() //initialise les couleurs pour chaque accords
{
    for(int i = 0; i < 7; i++)
    {
        colors[i].Init((Color::PresetColor)i);
    }
    colors[7].Init(1, 1, 0);
    colors[8].Init(1, 0, 1);
    colors[9].Init(0, .7, .4);
}

int main(void)
{

    float samplerate;

    // Initialise le tout
    hw.Init();
    hw.SetAudioBlockSize(4); //taille bloc audio
    samplerate = hw.AudioSampleRate();

    InitSynth(samplerate);
    InitChords();
    InitColors();

    // Fonctions appelées par le système audio lui même
    hw.StartAdc();
    hw.StartAudio(AudioCallback);

    while(1) {}
}

void UpdateEncoder() //change accords
{
    if(hw.encoder.RisingEdge()) //si appuie, revient au premier accord
    {
        chordNum = 0;
    }

    chordNum += hw.encoder.Increment(); //tourner = + ou - 1    
    chordNum = (chordNum % 10 + 10) % 10; //utilisation du modulo pour garantir des valeurs entre 0 et 9
}

void UpdateKnobs() //maj des potas : déplace + ou - les notes d'une octave (+12 demi tons)
{
    float samplerate;

    samplerate = hw.AudioSampleRate();

    counter++;

    if(counter >= samplerate/20) //pour avoir un changement de note toutes les 56 ms
    {
        counter=0;
        
        freq = melodie1_t1[0][step];

        step++;
        if(step>=length)
        {
            step = 0;
        }
    }

    int inversion = (int)p_inversion.Process(); //recup val pota 2 - inversion de l'ordre des notes

    //faire un if plus tard pour faire jouer que la mélodie principal et ajouter des accords en changer l'encodeur
    
    notes[0] = freq + (12 * (inversion >= 1)); //note fonda inversée ou non
    notes[1] = freq + chord[chordNum][0] + (12 * (inversion >= 2)); //freq note fonda + freq pour atteindre pro notes de l'accord + 12 demi tons si inversion >=2 
    notes[2] = freq + chord[chordNum][1] + (12 * (inversion >= 3));
    notes[3] = freq + chord[chordNum][2] + (12 * (inversion >= 4)); //si inversion 4 est atteinte inverse tout l'odre des accords (augmente de 12 tout)
}

void UpdateLeds() //une couleur associée à un accord, feedback visuel
{
    hw.led1.SetColor(colors[chordNum]);
    hw.led2.SetColor(colors[chordNum]);
    hw.UpdateLeds();
}

void UpdateControls()
{
    hw.ProcessAnalogControls();
    hw.ProcessDigitalControls();

    UpdateEncoder();
    UpdateKnobs();
    UpdateLeds();
}
