# NomExemple-explanations

*(This file is meant to be modified)*

Quick sum of what the code do, and the [https://Link/to/the/original/example](https://Link/to/the/original/example). 

File written by *Emma LEON* (2026). 


## Members of the class

### Objects

#### DaisyPod `hw`

Objet matériel principal

Il regroupe tous les éléments physiques de la Daisy Pod :

- Boutons
- Encodeur
- Potentiomètres
- LEDs
- entrée/sortie audio
- ADC (convertisseur analogique à numérique)

Il sert d'interface entre le code et le hardware.

#### Oscillator `osc[4]`

Tableau de 4 oscillateurs sinusoïdaux DaisySP. Chaque oscillateur génère une onde audio indépendante. Ils sont utilisés pour produire les 4 notes de l'accord.

#### Parameter `p_freq`, `p-inversion`

Objets qui lisent les potentiomètres : 

- `p-freq` : note principale (0 - 127 valeurs possibles correspondant au valeur des notes MIDI)
- `p-inversion` : inversion d'accord (0 - 5 valeurs possibles)

Ils convertissent la position du potentiomètre en une valeur numérique exploitable.

#### Color `colors[10]`

Tableau de 10 couleurs, une par accord. Utilisé pour afficher visuellement l'accord sélectionné via les LEDs.

### Other variables

`notes[4]`

Tableau contenant les 4 notes MIDI de l'accord actuel.


`chord[10][3]`

Tableau contenant les 10 types d'accord possibles, chacun défini par 3 intervalles (4 notes) : 

- Tierce
- Quinte
- Septième

`chordNum`

Numéro de l'accord sélectionné (0-9). Modifié par l'encodeur.

### Misc

#### `UpdateControls()`

```
void UpdateControls()
{
    hw.ProcessAnalogControls();
    hw.ProcessDigitalControls();

    UpdateEncoder();
    UpdateKnobs();
    UpdateLeds();
}
```

Les opérations suicantes sont effectuées : 
- Lecture des contrôles analogiques - potentiomètres (`hw.ProcessAnalogControls()`)
- Lecture des contrôles digitaux - boutons, encodeur (`hw.ProcessDigitalControls()`)
- Mise à jour de l'encodeur (`UpdateEncoder()`)
- Mise à jour des notes via la valeur des potentiomètres (`UpdateKnobs()`)
- Mise à jour des LEDs (`UpdateLeds()`)

Fonction qui met à jour tous les contrôles : 

- Potentiomètres
- Encodeur
- LEDs

Elle est appelée à chaque bloc audio.


#### `UpdateEncoder()`

```
void UpdateEncoder()
{
    if(hw.encoder.RisingEdge())
    {
        chordNum = 0;
    }

    chordNum += hw.encoder.Increment();
    chordNum = (chordNum % 10 + 10) % 10;
}
```
Les opérations suivantes sont effecturées : 
- Si l'encodeur est pressé : retour à l'accord 0
- Si l'encodeur tourne : changement d'accord (+1 ou -1)
- Application d'un modulo pour rester entre 0 et 9

#### `UpdateKnobs()`

```
void UpdateKnobs()
{
    int freq      = (int)p_freq.Process();
    int inversion = (int)p_inversion.Process();

    notes[0] = freq + (12 * (inversion >= 1));
    notes[1] = freq + chord[chordNum][0] + (12 * (inversion >= 2));
    notes[2] = freq + chord[chordNum][1] + (12 * (inversion >= 3));
    notes[3] = freq + chord[chordNum][2] + (12 * (inversion >= 4));
}
```

Les opérations suivantes sont effectuées : 
- Lecture du potard de fréquence (`p_freq.Process()`)
- Lecture du potard d'inversion (`p_inversion.Process()`)
- Calcul des 4 notes MIDI :
	- note fondamentale (`freq`)
 	- ajout des intervalles de l'accord (`chord[chordNum][?]`)
  	- ajout d'octaves selon l'inversion (`(12 * (inversion >= ?))`)

#### `UpdateLeds()`

```
void UpdateLeds()
{
    hw.led1.SetColor(colors[chordNum]);
    hw.led2.SetColor(colors[chordNum]);
    hw.UpdateLeds();
}
```
Les opérations suivantes sont effectuées : 
- Sélection de la couleur correspondant à l'accord actuel (`hw.led?.SetColor(colors[chordNum])`)
- Mise à jour des deux LEDs RGB (`hw.UpdateLeds()`)

## Functions of the class

### Main 

```
int main(void)
{
    float samplerate;

    hw.Init();
    hw.SetAudioBlockSize(4);
    samplerate = hw.AudioSampleRate();

    InitSynth(samplerate);
    InitChords();
    InitColors();

    hw.StartAdc();
    hw.StartAudio(AudioCallback);

    while(1) {}
}
```

Dans la fonction main, les oprations suivantes sont effectuées :
- Initialisation du hardware (`hw.Init()`)
- Définition de la taille des bloc audio (`hw.SetAudioBlockSize(4)`)
- Récupération de la fréquence d'échantillonnage (`samplerate = hw.AudioSampleRate()`)
- Initialisation du synthès, des accords et des couleurs (tous les `Init...()`)
- Démarrage de l'ADC et de l'audio (`hw.StartAdc()` et `hw.StartAudio(AudioCallback)`)
- Boucle infinie (programme embarqué `while(1)`)


### AudioCallback

```
static void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t                                size)
{
    UpdateControls();

    for(int i = 0; i < 4; i++)
    {
        osc[i].SetFreq(mtof(notes[i]));
    }

    for(size_t i = 0; i < size; i += 2)
    {
        float sig = 0;
        for(int i = 0; i < 4; i++)
        {
            sig += osc[i].Process();
        }

        out[i]     = sig;
        out[i + 1] = sig;
    }
}
```

Dans la fonction AudioCallback, les opérations suivantes sont effectuées : 

- Mise à jour des contrôles - potentiomètres, encodeur, LEDs (`UpdateControls()`)
- Mise à jour des fréquences des 4 oscilalteurs selon les notes MIDI (`osc[i].SetFreq(mtof(notes[i]))`)
- Boucle audio :
	- Génération du signal de chaque oscillateur (`osc[i].Process()`)
	- Addition des 4 signaux
  	- Ecriture du signal dans les sorties gauche et droite (`out[i]`, `out[i+1]`)

### InitSynth

```
void InitSynth(float samplerate)
{
    p_freq.Init(hw.knob1, 0, 127, Parameter::LINEAR);
    p_inversion.Init(hw.knob2, 0, 5, Parameter::LINEAR);

    for(int i = 0; i < 4; i++)
    {
        osc[i].Init(samplerate);
        osc[i].SetAmp(0.1f);
        osc[i].SetWaveform(Oscillator::WAVE_SIN);
        notes[i] = 30;
    }
}
```

Opérations effectuées : 
- Initialisation des paeamètres des potentiomètres :
	- `p_freq` : note principale (0 - 127 MIDI)
 	- `p_inversion` : inversion d'accord (0 - 5)
- Initialisation des 4 oscillateurs :
	- définition de la fréquence d'échantillonnage (`Init(samplerate)`)
 	- réglage de l'amplitude (`SetAmp(0,1f)`)
	- choix de la forme d'onde (`WAVE_SIN`)
- Initialisation des notes MIDI à 30, valeur par défaut (`notes[i] = 30`)

### InitChords()

```
void InitChords()
{
    for(int i = 0; i < 8; i++)
    {
        chord[i][0] = 3 + ((i + 1) % 2);
    }
    chord[8][0] = chord[9][0] = 3;

    chord[0][1] = chord[1][1] = chord[4][1] = chord[5][1] = chord[6][1]
        = chord[7][1]                                     = 7;
    chord[3][1] = chord[8][1] = chord[9][1] = 6;
    chord[2][1] = 8;

    chord[0][2] = chord[1][2] = chord[2][2] = chord[3][2] = 12;
    chord[4][2] = chord[7][2] = 11;
    chord[5][2] = chord[6][2] = chord[9][2] = 10;
    chord[8][2] = 9;
}
```

Opérations effectuées : 
- Construction de 10 accords différents sous formes d'intervalles MIDI
- Définition des tierces - majeures ou mineures (boucle `for` et `chord[8][0] = ... = 3`)
- Définition des quintes - justes, diminuées ou augmentées (`chord[0][1] = ... = 7`, `chord[3][1] = ... = 6` et `chord[2][1]=8`)
- Définition des septièmes - majeures, mineures, dominuées ou octave (`chord[0][2] = ... = 9`)

### InitColors

```
void InitColors()
{
    for(int i = 0; i < 7; i++)
    {
        colors[i].Init((Color::PresetColor)i);
    }
    colors[7].Init(1, 1, 0);
    colors[8].Init(1, 0, 1);
    colors[9].Init(0, .7, .4);
}
```

Opérations effectuées : 
- Association d'une couleur par accord
- Initialisation des 7 premières couleurs via les presets Daisy (boucle `for`)
- Définition manuelle des 3 dernières couleurs - RGB (`colors[?].Init(?, ?, ?)`)

## Examples for the different object

### Parameter

Un `parameter` lit un potentiomètre et convertit sa position en une valeur numérique.

#### Exemple d'utilisation
```
p_freq.Init(hw.knob1, 0, 127, Parameter::LINEAR); // potard → note MIDI
int note = (int)p_freq.Process();                 // lit la valeur du potentiomètre

p_inversion.Init(hw.knob2, 0, 5, Parameter::LINEAR); // potard → inversion
int inv = (int)p_inversion.Process();                // lit la valeur
```

### Color

Chaque `color`représente une couleur RGB utilisée pour les LEDs.

#### Exemple d'utilisation
```
colors[0].Init(Color::BLUE); // couleur prédéfinie
colors[1].Init(1.0f, 0.5f, 0.0f); // couleur personnalisée (RGB)

hw.led1.SetColor(colors[1]); // applique la couleur à la LED
hw.UpdateLeds();
```



