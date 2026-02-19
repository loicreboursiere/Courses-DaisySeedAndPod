# Explications de MusicBox

Ce code génère des notes aléatoires dans une gamme définie à chaque fois qu'on appuie sur un bouton. Le signal passe par une enveloppe, un filtre et une réverbération. 

L'exemple original se trouve ici : https://github.com/electro-smith/DaisyExamples/tree/master/pod/MusicBox

Fichier écrit par Olivier CLAVIER (2026).


## Membres de la classe

### Objets

- DaisyPod (`hw`) : Permet de gérer l'interface matérielle spécifique au Pod, ici des boutons, un encodeur, des LEDs et les potentiomètres.
- Oscillator (`osc`) : Le générateur de signal de base
- Svf (`filt`) : Un filtre d'état variable qui permet de sculpter le timbre
- ReverbSc (`venb`) : Un module de réverbération pour ajouter de l'espace au son
- AdEnv (`env`) : Une enveloppe simple Attack/Decay pour contrôler le volume
- Parameter (`p_xf`, `p_vamt`, `p_dec`, `p_vtime`) : Des utilitaires pour lisser et mapper les valeurs des potentiomètres vers des plages de valeurs musicales

### Autres variables

- `scale[7]` : Un tableau de constantes définissant les intervalles d'une gamme majeure
- `sig`, `rawsig`, `filtsig`, `sendsig`: Des variables de stockage temporaire du signal audio à différentes étapes du traitement
- `wetvl`, `wetvr` : Des variables qui récupèrent les sorties gauche et droite de la réverbération
- `freq` : Variable qui sert à stocker la valeur de la fréquence actuelle de l'oscillateur

### Divers
- `get_new_note()`: Une fonction qui choisit aléatoirement une note dans le tableau `scale` sur deux octaves.

## Fonctions de la classe

### Main 

```
int main(void)
{
    float samplerate;
    hw.Init();
    hw.SetAudioBlockSize(4);
    samplerate = hw.AudioSampleRate();
    InitSynth(samplerate);
    hw.StartAdc();
    hw.StartAudio(audio);
    while(1) {}
}
```

Dans la fonction `main`, les opérations suivantes sont effectuées : 

- Initialisation du matériel avec `hw.Init()`, ce qui permet de configurer les GPIO, la mémoire, les horloges internes.
- `hw.SetAudioBlockSize(4)` permet de configurer le traitement du son par blocs de 4 échantillons, plus il y à moins d'échantillons plus il y aura moins de latence
- `hw.AudioSampleRate()` permet d'intérroger la carte pour savoir le taux d'échantillonage, par défaut on est a 48Khz.
- `InitSynth(samplerate)` permet d'envoyer la valeur du taux d'échantillonage aux objets (`osc`,`env`,`filt`,`verb`) qui se trouve dans la fonction `InitSynth()`.
- `hw.StartAdc()` permet de mettre en marche le convertisseur analogique vers digital pour récupérer les valeurs des potentiomètres.
- `hw.StartAudio(audio)` permet d'indiquer à la carte de commencer à produire du son en utilisant la fonction `audio`.


### AudioCallback

```
static void  audio(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size){}
```

Dans la fonction AudioCallback, les opérations suivantes sont effectuées :

- Récupère les valeurs des boutons
- Vérifie si un front montant est détécté sur le Bouton 1 
- Calcule le signal de l'oscillateur multiplié par l'enveloppe, applique le filtre, traite la réverbération et envoie le résultat final aux sorties gauche et droite.

### Autre fonction

```
void InitSynth(float samplerate)
```

Dans la fonction InitSynth, les opérations suivantes sont effectuées :

- Relie les potentiomètres (`knob1`, `knob2`) aux paramètres de fréquence, de déclin et de réverbération en définissant leurs plages de valeurs.
- Définit la forme d'onde de l'oscillateur sur "Polyblep Saw" et règle les temps d'attaque et de déclin par défaut sur l'enveloppe.

## Exemples pour les différents objets

### Oscillateur 

L'oscillateur est la source du son. On peut changer sa forme d'onde et sa fréquence.

```
osc.SetWaveform(Oscillator::WAVE_SQUARE);
osc.SetFreq(440.0f); // Note LA (A4)
float signal = osc.Process();
```

### AdEnv (Enveloppe)

L'enveloppe contrôle le volume du son pour qu'il ne soit pas continu.

```
env.SetTime(ADENV_SEG_ATTACK, 0.01f); // Attaque rapide
env.SetTime(ADENV_SEG_DECAY, 0.5f);   // Déclin court
env.Trigger();                        // Déclenche l'enveloppe
float volume = env.Process();
```

### Parameter

L'objet Parameter est crucial pour transformer la valeur d'un potentiomètre (0 à 1) en valeur musicale.
```
// Transformation du Knob 1 en fréquence de 100Hz à 5000Hz (Logarithmique)
p_xf.Init(hw.knob1, 100.0f, 5000.0f, Parameter::LOGARITHMIC);
float cutoff = p_xf.Process();
```