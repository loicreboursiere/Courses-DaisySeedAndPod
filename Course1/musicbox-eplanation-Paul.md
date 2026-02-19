# NomExemple-explanations

*(This file is meant to be duplicated, not modified)*

Quick sum of what the code do, and the [https://Link/to/the/original/example](https://Link/to/the/original/example). 

File written by Paul, Liaras* (2026). 


## Members of the class

What type of `object`are used in this example ? 
What do they do ? 
What are the other `variables`?

### Objects
-Daisy Pod hw: Cela représente les potentiomètres physique du daysiPod. On contrôle les potentiomètres, les leds

-Oscillator osc: C'est l'oscillator qui va permetre d'avoir un son. On défini plus tard quelle type de waveform on veut.

-Svf filt: C'est un filtre.

-ReverbSc verb: Applique de la reverb au signal.

-AdEnv  env: Créer l'enveloppe du signal, on définie en suite l'attack, le decay.  C'est le volume de la note pendant qu'elle est jouée. Ca rajoute un début et une fin.

-Parameter: Ce sont des parametres pour les boutons afin de faire varier les effets du son. p_xf pour le filtre, p_vamt pour la reverb et p_dec pour le decay.

### Other variables
On retrouve aussi:

-Scale: un tableau qui contient une gamme de note que l'on viendra tirer au hasard. Par défaut une gamme majeure afin que toute les notes sonnent bien ensemble.

-freq: une variable flottante qui prend la fréquence jouée.

-sig, raw, filtsig et sendsig: Ce sont des variables qui contiennent le signal entre l'oscillo, le filtre, la reverb.


### Misc
get_new_note():
C'est une fonction qui choisi une note au hasard dans le tableau scale.

## Functions of the class

Explain the content of

### Main 

```
int main(void)
{
    float samplerate;
    // Init
    hw.Init();
    hw.SetAudioBlockSize(4);
    samplerate = hw.AudioSampleRate();
    InitSynth(samplerate); 
    // Start Callbacks
    hw.StartAdc();
    hw.StartAudio(audio);
    while(1) {}
}
```

In the main function, the following operations are performed : 

- hw.Init(): On initialise les boutons, leds du DaisyPod.
- InitSynth(samplerate); On configure les paramètres initiaux de l'oscillo mais aussi des filtres et de la reverb
- hw.StartAudio(audio); On utilise audio pour "démarrer" le son.


### AudioCallback

```
static void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t                                size){}
```

In the AudioCallback function, the following operations are performed : 

- On regarde si les boutons sont utilisé, si il y a un front montant avec hw.button1.RisingEdge(). Si c'est le cas on paramètre une nouvel fréquence et on Trigger l'enveloppe. 

- On vérifie aussi les parametres assigné aux boutons. Ces parametres peuvent être la fréquence de coupure du filtre xf et la reverb vamt.

- On génère aussi le son avec osc.Process, on applique l'enveloppe, les filtres, la reverb et on l'envoie à la sortie du DaisyPod.

### Another Function

```
void InitSynth(float samplerate){}
```

In the Another function, the following operations are performed : 

- On initialise les boutons avec les parametres p_xf,p_vimt,p_dec, p_time etc. Cela permet d'assigner tel boutons à un effet ou parametres mais aussi de définir sa plage de valeur.

- On initialise au le DSP, avec le samplerate, mais aussi la waveform et la fréquence du signal. Il y a aussi la déinition de l'enveloppe, le filtre et la reverb.

## Examples for the different object

### Object 1

The code below shows ...
Oscillator osc;
...
osc.SetFreq(freq);  (dans la boucle pour détecter un front montant)
...
rawsig = osc.Process(); ( dans la audio loop)
...
 osc.Init(samplerate);
osc.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
osc.SetFreq(100.0f);
osc.SetAmp(.5f);
...

```
On initialsie l'oscillateur
On choisi sa forme
On lui donne une note a jouer
on lui demande jouer la note
```

### Object 2

The code below shows ...
AdEnv env;
...
env.SetTime(ADENV_SEG_DECAY, dec);
env.Trigger();     
(Dans la boucle pour détecter un front montant)
...
sig = rawsig * env.Process();
...
env.Init(samplerate);
env.SetCurve(-15.0f);
env.SetTime(ADENV_SEG_ATTACK, 0.002f);
env.SetTime(ADENV_SEG_DECAY, 2.6f);
env.SetMax(1.f);
env.SetMin(0.f);

```
On donne la courbe de l'envelloppe, exponentielle, linéaire...
On définie les temps d'attack et de decay
et les volumes lorsque le signal est au minimum et au max. 
```

### Object 3

The code below shows ...
Svf  filt;
...
filt.Process(sig);
(Dans la boucle de détection de front montant)
...
filt.Init(samplerate);
filt.SetRes(0.5f);
filt.SetDrive(0.8f);
filt.SetFreq(2400.0f);
...

```
On initialise les parametres du filtres tel qsue la resonance, la distortion et la fréquence de coupure.
On passe le signal audio dans le filtre avec .process(sig)
```

### Object 5

The code below shows ...
Paramter p_xf, p_vamt, p_dec, p_vtime;;
...
p_xf.Init(hw.knob1, 10.0f, 12000.0f, Parameter::LOGARITHMIC);
p_dec.Init(hw.knob1, 0.2f, 5.0f, Parameter::EXPONENTIAL);       
p_vamt.Init(hw.knob2, 0.0f, 1.0f, Parameter::LINEAR);
p_vtime.Init(hw.knob2, 0.4f, 0.95f, Parameter::LINEAR);
...

```
On associe un hardware a un parametre avec ses propres settings.
```