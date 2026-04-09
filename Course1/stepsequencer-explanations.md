# NomExemple-explanations

*(This file is meant to be duplicated, not modified)*


Séquenceur de 8 pas avec des contrôles individuels pour chaque step sur le decay, le pitch et l'activation du pas.
Un mode édition et un mode Play existe pour pouvoir éditer notre séquenceur depuis la carte directement

[https://github.com/electro-smith/DaisyExamples/tree/master/pod/StepSequencer](https://github.com/electro-smith/DaisyExamples/tree/master/pod/StepSequencer). 

File written by *Yanis OUCHENE* (2026). 


## Members of the class

What type of `object`are used in this example ? 
What do they do ? 
What are the other `variables`?

### Objects

| Objet | Type | Rôle |
|---|---|---|
| `pod` | `DaisyPod` | Interface principale avec le matériel (audio, boutons, encodeur, LEDs, potentiomètres) |
| `osc` | `Oscillator` | Génère le signal audio (onde triangulaire, carrée, rampe) |
| `env` | `AdEnv` | Enveloppe Attack-Decay : module l'amplitude du son dans le temps |
| `tick` | `Metro` | Métronome interne : déclenche l'avancement d'un pas à intervalles réguliers |
| `flt` | `MoogLadder` | Filtre passe-bas de type Moog |
| `decayTimeParam` | `Parameter` | Mappe la valeur brute du Knob 1 vers une durée de decay (0.03s → 1s) en mode édition |
| `pitchParam` | `Parameter` | Mappe la valeur du Knob 2 vers un index de gamme pentatonique majeure (0 → 10) en mode édition |
| `tickFreqParam` | `Parameter` | Mappe le Knob 1 vers une fréquence de tick (4 → 13 BPS) en mode play |
| `filterFreqParam` | `Parameter` | Mappe le Knob 2 vers une fréquence de coupure (100 → 10000 Hz, échelle log) en mode play |
| `colors[8]` | `Color[8]` | Tableau des 8 couleurs associées à chaque pas du séquenceur |

### Other variables

| Variable | Type | Rôle |
|---|---|---|
| `edit` | `bool` | `true` = mode édition, `false` = mode play |
| `editCycle` | `bool` | Si `true` en mode édition, l'enveloppe se redéclenche en boucle pour écouter le son du pas courant |
| `step` | `uint8_t` | Index du pas courant (0 à 7) |
| `wave` | `uint8_t` | Index de la forme d'onde sélectionnée (0 = Ramp, 1 = Square) |
| `dec[8]` | `float[8]` | Durée de decay (en secondes) pour chacun des 8 pas |
| `pitch[8]` | `float[8]` | Fréquence (en Hz) pour chacun des 8 pas |
| `active[8]` | `bool[8]` | Indique si chaque pas est activé (jouera lors de la lecture) ou non |
| `env_out` | `float` | Valeur instantanée de l'enveloppe, utilisée aussi pour piloter les LEDs en mode play |
| `pent[5]` | `float[5]` | Fréquences de base de la gamme pentatonique majeure : {110, 128.33, 146.66, 174.166, 192.5} Hz |
| `oldk1`, `oldk2` | `float` | Valeurs précédentes des potentiomètres (pour détecter un changement réel) |
| `k1`, `k2` | `float` | Valeurs actuelles lues sur les potentiomètres (0.0 → 1.0) |
| `tickFrequency` | `float` | Fréquence courante du métronome (en pas par seconde) |
| `filterFrequency` | `float` | Fréquence de coupure courante du filtre (en Hz) |


### Misc

La fonction `ConditionalParameter` est un utilitaire qui évite de mettre à jour un paramètre si le potentiomètre n'a pas bougé (seuil de `0.00005`). Cela évite les sauts de valeurs indésirables au changement de mode.

La gamme pentatonique est construite dynamiquement grâce à `pitchParam` qui retourne une valeur entre 0 et 10, qui est convertie en fréquence réelle avec `pent[index % 5] * (index / 5 + 1)`. Les 5 premières notes sont à l'octave de base (×1), les 5 suivantes à l'octave supérieure (×2).

## Functions of the class

### Main 

```cpp
int main(void)
{
    // initialisation + boucle infinie
}
```

Dans la fonction `main`, les opérations suivantes sont effectuées :

- **Initialisation du hardware** : `pod.Init()` démarre tous les périphériques (audio, ADC, boutons, LEDs). La taille du bloc audio est fixée à 4 samples avec `pod.SetAudioBlockSize(4)`.

- **Initialisation des objets DSP** : `osc`, `env`, `tick` et `flt` sont tous initialisés avec le `sample_rate` récupéré via `pod.AudioSampleRate()`. Leurs paramètres par défaut sont configurés : forme d'onde triangulaire pour l'oscillateur, fréquence de tick à 3 Hz, filtre ouvert à 20 kHz.

- **Configuration des `Parameter`** : les quatre objets `Parameter` sont liés à leur potentiomètre (`pod.knob1` ou `pod.knob2`) et à leur plage de valeurs, avec une échelle linéaire ou logarithmique.

- **Initialisation des données du séquenceur** : les tableaux `dec`, `pitch` et `active` sont remplis avec des valeurs par défaut (decay à 0.5s, pitch à 110 Hz, tous les pas inactifs). Les 8 couleurs sont initialisées avec les presets de la classe `Color` (rouge, vert, bleu, blanc, violet, cyan, orange, jaune).

- **Démarrage de l'audio** : `pod.StartAdc()` lance la lecture des potentiomètres, `pod.StartAudio(AudioCallback)` démarre le callback audio. La `while(1){}` finale maintient le programme en vie — tout le traitement se passe dans le callback.



### AudioCallback

```cpp
static void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t                                size){}
```

Le callback audio est appelé automatiquement par le système à intervalles réguliers pour remplir le buffer de sortie. Les opérations effectuées sont :

- **Lecture des contrôles** : `Controls()` est appelé une fois par bloc pour traiter l'encodeur, les boutons et les potentiomètres, et mettre à jour les LEDs. Ce n'est pas fait sample par sample pour des raisons de performances.

- **Boucle de génération audio** : la boucle `for` parcourt le buffer (`i += 2`). À chaque itération, `NextSamples(sig)` calcule un sample audio, qui est ensuite copié sur les deux canaux (`out[i]` et `out[i+1]`) pour obtenir un signal stéréo identique gauche/droite.

### Another Function

#### Controls

```cpp
void Controls()
```

Fonction appelée une fois par bloc audio. Elle orchestre la lecture de tous les contrôles dans l'ordre :

- `pod.ProcessDigitalControls()` : lit l'état des boutons et de l'encodeur (détecte les fronts montants/descendants).
- `pod.ProcessAnalogControls()` : lit les valeurs ADC des potentiomètres.
- `UpdateEncoderPressed()` : bascule entre mode édition et mode play si l'encodeur est pressé, remet `step` à 0.
- `UpdateEncoderIncrement()` : en mode édition, navigue entre les pas ; en mode play, change la forme d'onde de l'oscillateur.
- `UpdateButtons()` : en mode édition, Button 2 active/désactive le pas courant, Button 1 bascule le mode `editCycle`.
- `UpdateKnobs()` : lit les potentiomètres et met à jour soit les paramètres du pas en cours (decay, pitch) en mode édition, soit le tempo et la fréquence de filtre en mode play.
- `UpdateLeds()` : met à jour les deux LEDs selon le mode actif.

#### NextSamples

```cpp
void NextSamples(float &sig)
```

Appelée pour chaque sample dans la boucle audio. C'est le cœur du traitement DSP :

- `env.Process()` avance l'enveloppe d'un sample et retourne sa valeur courante (stockée dans `env_out`).
- `osc.SetAmp(env_out)` module l'amplitude de l'oscillateur par l'enveloppe (le son monte et descend selon la courbe AD).
- `sig = osc.Process()` génère un sample de l'oscillateur.
- `sig = flt.Process(sig)` filtre le sample avec le `MoogLadder`.
- `tick.Process()` retourne `true` une fois par période (selon la fréquence du métronome). Si on est en mode play, le pas avance et l'enveloppe est déclenchée (`env.Trigger()`) si le pas est actif.
- La fréquence et le decay de l'oscillateur sont mis à jour à chaque sample si le pas est actif (pour que les changements en édition soient instantanément audibles).
- Si l'enveloppe est terminée et que `editCycle` est actif, elle est redéclenchée immédiatement pour créer un bouclage continu.


#### UpdateLeds/Edit
```cpp
void UpdateLeds()
void UpdateLedEdit()
```

- **Mode édition** (`UpdateLedEdit`) : LED 1 affiche la couleur du pas sélectionné. LED 2 affiche la même couleur si le pas est actif, ou s'éteint s'il est inactif.
- **Mode play** : les deux LEDs passent au vert avec une intensité proportionnelle à `env_out`, créant un effet de "pulse" au rythme des notes jouées.


#### ConditionalParameter


```cpp
void ConditionalParameter(float o, float n, float &param, float update)
```

Utilitaire anti-saut : compare l'ancienne valeur `o` et la nouvelle valeur `n` d'un potentiomètre. Si la différence dépasse `0.00005`, le paramètre cible est mis à jour avec `update`. Cela évite qu'un changement de mode ne provoque une mise à jour immédiate si le potentiomètre n'a pas bougé.


## Examples for the different object

### Oscillator

L'`Oscillator` génère un signal audio périodique. Il supporte plusieurs formes d'onde et permet de contrôler fréquence, amplitude et forme en temps réel.

```cpp
#include "daisysp.h"
using namespace daisysp;

Oscillator osc;

void InitOscillator(float sample_rate)
{
    osc.Init(sample_rate); // Initialisation obligatoire avec le sample rate

    // Choisir une forme d'onde parmi :
    // WAVE_SIN, WAVE_TRI, WAVE_SAW, WAVE_RAMP, WAVE_SQUARE, WAVE_POLYBLEP_SAW...
    osc.SetWaveform(Oscillator::WAVE_TRI);

    osc.SetFreq(440.f);   // Fréquence en Hz (ici La4)
    osc.SetAmp(0.5f);     // Amplitude entre 0.0 et 1.0
}

void GenerateAudio(float &sig)
{
    // Appeler Process() une fois par sample pour obtenir la valeur du signal
    sig = osc.Process();

    // Changer la fréquence à la volée

    osc.SetFreq(220.f); // Passe à La3

    // Changer la forme d'onde en cours de route
    osc.SetWaveform(Oscillator::WAVE_SQUARE);
}
```

### AdEnv (Enveloppe Attack-Decay)

L'`AdEnv` génère une enveloppe à deux segments (Attack puis Decay). Elle est déclenchée manuellement et peut être utilisée pour moduler l'amplitude, un filtre, ou n'importe quel paramètre.

```cpp
#include "daisysp.h"
using namespace daisysp;

AdEnv env;

void InitEnvelope(float sample_rate)
{
    env.Init(sample_rate);

    env.SetTime(ADENV_SEG_ATTACK, 0.01f);  // Durée de l'attaque : 10 ms
    env.SetTime(ADENV_SEG_DECAY, 0.5f);   // Durée du decay : 500 ms

    env.SetMin(0.0f);   // Valeur minimale de la sortie (fin du decay)
    env.SetMax(1.0f);   // Valeur maximale de la sortie (pic de l'attaque)

    // Optionnel : courbe de l'enveloppe (linéaire par défaut)
    env.SetCurve(0.f); // 0 = linéaire, valeurs négatives = convexe, positives = concave
}

void TriggerAndProcess(float &amp_out)
{
    // Déclencher l'enveloppe (à appeler lors d'un événement)
    env.Trigger();

    // Dans la boucle audio, appeler Process() à chaque sample
    amp_out = env.Process(); // Retourne la valeur courante entre Min et Max

    // Vérifier si l'enveloppe est encore active
    if(!env.IsRunning())
    {
        // L'enveloppe a terminé son decay
    }
}
```


### Metro (Métronome)

Le `Metro` génère une impulsion régulière à une fréquence donnée. Il retourne `true` exactement une fois par période — idéal pour cadencer un séquenceur.

```cpp
#include "daisysp.h"
using namespace daisysp;

Metro tick;

void InitMetro(float sample_rate)
{
    tick.Init(2.f, sample_rate); // Initialisation à 2 Hz = 2 ticks par seconde = 120 BPM
}

void ProcessMetro()
{
    // Changer le tempo à la volée
    tick.SetFreq(4.f); // 4 Hz = 4 ticks/s = 240 BPM

    // Process() doit être appelé à chaque sample
    // Il retourne true uniquement lors du tick
    if(tick.Process())
    {
        // Déclencher l'événement rythmique ici (avancer le pas, déclencher une note...)
    }
}
```

> **N. B. :** la relation entre Hz et BPM dépend de la résolution souhaitée. Pour un séquenceur 8 pas à 120 BPM (quart de note par pas), la fréquence de tick est `120 / 60 = 2 Hz`.


### MoogLadder (Filtre)

Le `MoogLadder` est un filtre passe-bas inspiré du filtre analogique Moog. Il offre une résonance caractéristique et s'utilise en insertion dans la chaîne audio.


```cpp
#include "daisysp.h"
using namespace daisysp;

MoogLadder flt;

void InitFilter(float sample_rate)
{
    flt.Init(sample_rate);

    flt.SetFreq(8000.f);  // Fréquence de coupure en Hz (laisse passer les sons < 8kHz)
    flt.SetRes(0.7f);     // Résonance entre 0.0 (aucune) et 1.0 (auto-oscillation)
}

void ProcessFilter(float &sig)
{
    // Modifier la fréquence de coupure en temps réel (ex: LFO ou enveloppe)
    flt.SetFreq(500.f);  // Son mat / fermé
    flt.SetFreq(10000.f); // Son brillant / ouvert

    // Appliquer le filtre sur le signal : à appeler chaque sample
    sig = flt.Process(sig);
}
```

### Parameter

`Parameter` est un utilitaire de mapping : il convertit la valeur brute d'un potentiomètre (0.0 → 1.0) vers une plage utile, avec une courbe linéaire ou logarithmique.

```cpp
#include "daisy_pod.h"
#include "daisysp.h"
using namespace daisy;
using namespace daisysp;

DaisyPod pod;
Parameter myParam;

void InitParameter()
{
    // Lier le paramètre au Knob 1, avec une plage de 200 Hz à 4000 Hz, en log
    myParam.Init(pod.knob1, 200.f, 4000.f, Parameter::LOGARITHMIC);

    // Ou en linéaire, ex : durée de 0.01s à 2.0s
    // myParam.Init(pod.knob1, 0.01f, 2.0f, Parameter::LINEAR);
}

void ReadParameter()
{
    // Lire la valeur mappée du potentiomètre (à appeler après pod.ProcessAnalogControls())
    float freq = myParam.Process(); // Retourne une valeur entre 200 et 4000 Hz

    // Utiliser directement dans un objet DSP
    // flt.SetFreq(freq);
}
```
### Color

`Color` représente une couleur RGB et sert à piloter les LEDs de la DaisyPod. Elle peut être initialisée via des presets ou des valeurs RGB manuelles.

```cpp
#include "daisy_pod.h"
using namespace daisy;

DaisyPod pod;
Color myColor;

void InitColor()
{
    // Utiliser un preset de couleur (8 presets disponibles : RED, GREEN, BLUE, WHITE,
    // PURPLE, CYAN, GOLD, YELLOW)
    myColor.Init(Color::PresetColor::RED);

    // Ou définir une couleur personnalisée avec des composantes R, G, B (entre 0.0 et 1.0)
    myColor.Init(1.0f, 0.5f, 0.0f); // Orange
}

void UseLeds()
{
    // Appliquer une couleur à une LED
    pod.led1.SetColor(myColor);

    // Ou définir directement les composantes RGB
    pod.led2.Set(0.0f, 1.0f, 0.0f); // Vert pur

    // Éteindre une LED
    pod.led1.Set(0.f, 0.f, 0.f);

    // Toujours appeler UpdateLeds() pour envoyer les valeurs au hardware
    pod.UpdateLeds();
}
```

## Exemple d'extension

### Mode aléatoire (Randomize)

Cette extension ajoute un randomiseur de pitchs utilisable à la fois en mode édition et en mode play. La fonction centrale `RandomizePitches()` assigne un pitch aléatoire sur 2 octaves de gamme pentatonique à tous les pas actifs.

| Mode | Contrôle | Comportement |
|---|---|---|
| Édition | Button 1 + Button 2 | Randomise une fois tous les pas actifs |
| Play | Button 2 (press) | Randomise une fois tous les pas actifs |
| Play | Button 1 (maintenu) | Re-randomise automatiquement à chaque tick du métronome |
| Play | LED 2 | Verte = normal / **Rouge** = auto-randomise actif |

```cpp
// À ajouter en haut du fichier, avec les autres includes
#include <cstdlib>  // Pour rand()

// Fonction centrale de randomisation
// Assigne un pitch aléatoire sur 2 octaves de pentatonique à chaque pas actif
void RandomizePitches()
{
    for(int i = 0; i < 8; i++)
    {
        if(active[i])
        {
            int randIndex = rand() % 10; // 0-9 = 2 octaves
            pitch[i] = pent[randIndex % 5] * (randIndex / 5 + 1);
        }
    }
}

// Dans UpdateButtons() — mode édition :
// Button 1 + Button 2 = randomise / Button 1 seul = toggle editCycle (inchangé)
if(pod.button1.Pressed() && pod.button2.RisingEdge())
{
    RandomizePitches();
}
else if(pod.button1.RisingEdge())
{
    editCycle = !editCycle;
}

// Dans UpdateButtons() — mode play :
// Button 2 = randomise une fois
if(pod.button2.RisingEdge())
{
    RandomizePitches();
}

// Dans NextSamples(), dans le bloc if(tick.Process()) :
// Button 1 maintenu = auto-randomise à chaque tick, juste avant le déclenchement
// de la note suivante pour éviter les artefacts audibles
if(pod.button1.Pressed())
{
    RandomizePitches();
}

// Dans UpdateLeds() — mode play :
// LED 2 rouge si auto-randomise actif, verte sinon
if(pod.button1.Pressed())
{
    pod.led2.Set(env_out, 0, 0); // Rouge pulsé
}
else
{
    pod.led2.Set(0, env_out, 0); // Vert pulsé
}
```

> **Note :** `rand()` sans seed produit la même séquence à chaque démarrage. Pour un vrai aléatoire, ajouter `srand((unsigned int)(pod.knob1.Process() * 1000))` dans le `main()` avant de démarrer l'audio.


### Changement de gamme

Le code ci-dessous remplace le tableau `pent[]` fixe par un ensemble de gammes sélectionnables.
En mode **play**, Button 1 permet de cycler entre les gammes disponibles.
Les pitchs de tous les pas actifs sont recalculés immédiatement à chaque changement.

| Mode | Contrôle | Comportement |
|---|---|---|
| Play | Button 1 (press) | Cycle entre les 3 gammes disponibles |
| Play | LED 1 | Blanche = majeure / Bleue = mineure / Cyan = japonaise |

```cpp
// Remplacer pent[] par une banque de gammes
const int NUM_SCALES   = 3;
uint8_t   currentScale = 0;

float scales[NUM_SCALES][5] = {
    {110.f, 123.47f, 138.59f, 164.81f, 185.f},  // Pentatonique majeure (La)
    {110.f, 130.81f, 146.83f, 164.81f, 196.f},  // Pentatonique mineure (La)
    {110.f, 123.47f, 146.83f, 164.81f, 185.f},  // In Sen / japonaise   (La)
};

// Stocker l'index de note par pas au lieu de la fréquence brute
int pitchIndex[8];

// Couleurs de feedback pour chaque gamme
Color scaleColors[NUM_SCALES]; // init : blanc, bleu, cyan

// Convertit un index (0-9) en fréquence Hz selon la gamme active
float IndexToFreq(int index, uint8_t scale)
{
    return scales[scale][index % 5] * (index / 5 + 1);
}

// Recalcule tous les pitchs avec la gamme courante
void ApplyScaleToPitches()
{
    for(int i = 0; i < 8; i++)
        pitch[i] = IndexToFreq(pitchIndex[i], currentScale);
}

// Dans UpdateButtons() — mode play :
if(pod.button1.RisingEdge())
{
    currentScale = (currentScale + 1) % NUM_SCALES;
    ApplyScaleToPitches();
}

// Dans UpdateLeds() — mode play :
// LED 1 indique la gamme active par sa couleur
pod.led1.SetColor(scaleColors[currentScale]);
```

> **Note :** Pour que le recalcul soit fidèle, il faut stocker l'index de gamme
> de chaque pas (0 à 9) dans un tableau `int pitchIndex[8]` au lieu de stocker
> directement la fréquence en Hz. Cela permet de retrouver la note équivalente
> dans n'importe quelle gamme lors du changement.
> La LED 1 peut indiquer la gamme active en mode play via une couleur dédiée
> (ex : blanc → majeure, bleu → mineure, cyan → japonaise).