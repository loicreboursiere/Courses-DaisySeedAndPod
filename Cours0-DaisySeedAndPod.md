# Cours 0 : Daisy Seed et Daisy Pod

## 1. Introduction générale

Les cartes Daisy Seed et Daisy Pod d'Electrosmith sont des plateformes
matérielles dédiées au développement d'effets audios, de synthétiseurs
et d'instruments numériques. Elles permettent de programmer en C++, avec
un accès direct aux entrées/sorties audio, aux potentiomètres, aux
boutons, aux LEDs et aux GPIO.

## 2. Présentation de la Daisy Seed

![Seed_angle_square.webp](Cours/Recurring%20presentation%20global%20elements/Daisy%20Seed%20-%20Presentation/37b6246c-634d-46c4-af34-cdfcacc264d0.png)

![Daisy_Seed_pinout_square.webp](Cours/Recurring%20presentation%20global%20elements/Daisy%20Seed%20-%20Presentation/Daisy_Seed_pinout_square.webp)

### Caractéristiques techniques :

- MCU : STM32H750, ARM Cortex-M7 @ 480 MHz
- RAM interne : 1 Mo
- RAM externe : 64 Mo
- Stockage : MicroSD + 8 Mo QSPI Flash
- Audio : 24-bit, 96 kHz stéréo
- Connectivité : UART, SPI, I2C, USB, MIDI
- Les patchs Pure Data et Max MSP peuvent être directement uploadés sur la carte

### Comparaison par rapport à la Teensy 4.1:

- Directement dédié à l’audio : 2  entrées et 2 sorties stéréo directement accessibles alors que Teensy est d’abord généraliste puis à développer une partie audio
    - Par contre, code audio sur la Teensy implémenté plus “proprement” pour certaines parties
- Mémoire plus importante
- Fréquence d’échantillonnage plus importante
- Daisy Pod permet d’avoir une plateforme de test directement

### Librairies :

Comme le montre la partie ci-dessus, la Daisy Seed n’est pas la seule plateforme embarquée pour prototyper des effets audio et/ou des interfaces de contrôles. D’autres plateformes, telles que PJRC Teensy 4.1, Rebel Technology Genius, Akso (basé sur le projet Axoloti), etc. existent. Ces cartes ou plateformes matérielles viennent chacune avec des caractéristiques techniques différentes, mais elles viennent aussi avec un **framework de programmation différent**. Pour la partie audio la carte Teensy s’appuie sur la [Teensy Audio Library](https://www.pjrc.com/teensy/td_libs_Audio.html), le Genius de Rebel Technology sur https://github.com/RebelTechnology/OpenWare, Akso sur la [Akso Object Library](https://zrna.org/akso).

La Daisy Seed s’appuie sur deux librairies principales https://github.com/electro-smith/libDaisy et https://github.com/electro-smith/DaisySP. Chacun de ces frameworks proposent une approche de la programmation audio et interactive. Un traitement programmé sur l’un ne sera pas utilisable avec un autre.

### Installation et upload du code (à partir de VSCode) :

- Principal lien pour le setup de base : [https://daisy.audio/tutorials/cpp-dev-env/](https://daisy.audio/tutorials/cpp-dev-env/)
- Les étapes :
  
1. Télécharger et Installer la toolchain Daisy correspondant à votre OS : 

[Toolchain Mac](https://daisy.nyc3.cdn.digitaloceanspaces.com/installers/DaisyToolchain-macos-installer-x64-0.2.0.pkg.zip) | [Toolchain Windows](https://daisy.nyc3.cdn.digitaloceanspaces.com/installers/DaisyToolchain-1.1.0-win64.exe) (+ [git for Windows](https://gitforwindows.org/)) | [Toolchain Linux](https://daisy.audio/tutorials/toolchain-linux/)
    
2. Télécharger et installer VSCode
- Pour Windows, quelques éléments supplémentaires pour VSCode : [https://daisy.audio/tutorials/cpp-dev-env/#3c-more-setup-windows-only](https://daisy.audio/tutorials/cpp-dev-env/#3c-more-setup-windows-only)
3. Clone des Daisy Examples 
        
  (**Dans un terminal (ou git for windows)**)
        
	1. Placer vous dans le dossier de votre choix via la commande `cd` (si vous n’êtes pas à l’aise avec le terminal, vous devriez pouvoir glisser/déposer le dossier de votre choix à partir de votre explorateur de fichiers)
	2. Exécuter cette commande :
     
```git
git clone --recurse-submodules https://github.com/electro-smith/DaisyExamples
```

	3. Tapez la commande : `ls -l` (Mac Linux) ou `dir` (Windows)

   `libDaisy >> Librairie daisy de base de l’écosystème Daisy`
                
   `DaisySP >> Librairie contenant le code lié au DSP`
                
   `seed >> Examples pour la Daisy Seed`
                
   `pod >> Examples pour la Daisy Pod`
                
   `cube, field, petal, patch_sm, patch, versio >> ces dossiers sont des exemples pour d’autres cartes ou matériel de la marque`
                
4. Uploader le code et faire fonctionner le code Blink LED : 

	1. Dans VSCode : `File > Open Folder` et sélectionnez le dossier `DaisyExamples/seed/Blink`
    2. Passer la Daisy Seed en bootloader mode (après l’avoir connectée en USB
		1. Pressez et Restez appuyé sur Boot
        2. Pressez et Restez appuyé sur Reset
        3. Relâcher Reset
        4. Relâcher Boot
    3. Lancez la palette de commandes de VSCode : `CTRL + P` (Windows ou Linux) ou `Command + P`(Mac)
		1.  Si c’est la première fois (ou si le code de libDaisy et/ou DaisySP a changé), tapez : `task build_all` puis `task build_and_program_dfu`
		2. Sinon seulement `task build_and_program_dfu`
		3. *Si vous utilisez un programmeur STM (comme le STLINK v3 mini, par exemple) la commande à utiliser est `task build_and_program` et non `task build_and_program_dfu`*
   		4. Après l’upload du code, la LED embarquée sur la carte devrait se mettre à clignoter

## 3. Daisy Pod

### Présentation

La carte Daisy Pod est une carte fille pour la Daisy Seed, c’est-à-dire qu’elle ne fonctionne que si une Daisy Seed lui est connectée. Cette carte donne accès directement à des contrôles et à des entrées/sorties audio et midi, ce qui en fait une très bonne plateforme de test lorsque que l’on veut développer de l’audio pour des plateformes embarquées 

Fichiers de références : `libDaisy/src/daisy_pod.h` et `libDaisy/src/daisy_pod.cpp`

### Caractéristiques :

- 2 potentiomètres
- 1 encodeur rotatif avec fonction push
- 2 boutons
- 2 LEDs RGB
- Entrée/sortie audio stéréo
- Entrée MIDI
- Sortie casque avec contrôle du volume

### Creation d’un nouveau projet C++ :

- Le dépôt https://github.com/electro-smith/DaisyExamples propose un script python pour créer facilement un projet C++ de base pour une carte ou plateforme donnée
- Etapes pour créer un projet C++ de base pour la Daisy Pod :
    - Lancez un `terminal` (Mac OS, linux) ou `git bash` (Windows)
    - Utilisez la commande `cd` pour vous placer dans le dossier DaisyExamples
    - Tapez : `python helper.py create MyProjects/TestPod --board pod`
    - Cette commande créée un dossier `MyProjects` (à l’intérieur du dossier DaisyExamples) avec un sous-dossier `HelloDaisyPod`

### Code - HelloDaisyPod

- Ajouté le dossier au workshpace VSCode
- Dans le code créé, ajoutez les lignes qui diffèrent par rapport aux lignes ci-dessous

```jsx
#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisyPod hw;
Color led1Color, led2Color;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	hw.ProcessAllControls();
	for (size_t i = 0; i < size; i++)
	{
		out[0][i] = in[0][i];
		out[1][i] = in[1][i];
	}

	//Color::PresetColor::RED;
	hw.ClearLeds();
	hw.led1.SetColor(led1Color);
	hw.led2.SetColor(led2Color);
	hw.UpdateLeds();

}

int main(void)
{
	hw.Init();
	led1Color.Init(Color::PresetColor::BLUE);
	led2Color.Init(Color::PresetColor::GREEN);
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAdc();
	hw.StartAudio(AudioCallback);
	while(1) {}

}
```

## Code HelloAudioDaisyPod

- Créez un nouveau projet C++ nommé HelloAudioDaisyPod

```cpp
#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisyPod hw;

Oscillator osc;

void AudioCallback(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size)
{

    hw.ProcessAllControls();

    float amp = hw.knob1.Process();
    float freq = hw.knob2.Process() * 800 + 20;

    osc.SetAmp(amp);
    osc.SetFreq(freq);

    for(size_t i = 0; i < size; i++)
    {

        float sig = osc.Process();
        out[ i ] = sig;
        out[ i + 1 ] = sig;
    }

}

int main(void)
{

    hw.Init();
	  hw.SetAudioBlockSize(4);

    osc.Init(hw.AudioSampleRate());
    osc.SetWaveform(osc.WAVE_SIN);

	  hw.StartAdc();
    hw.StartAudio(AudioCallback);

    while(1) {}

}
```

### 4. Afficher des informations dans la console

Si vous souhaitez afficher des infos dans la console, vous pouvez utilisez les fonctions StartLog( true ) et PrintLine() comme suit : 

```cpp
int main(void)

{

  hw.Init();
	//hw.seed.PrintLine( "Hello Seed from Pod - HelloAudioDaisyPod" );
	hw.SetAudioBlockSize(4);

	hw.StartLog( true );
	hw.PrintLine("We are printing on the console - SEED");

	hw.StartAdc();
  hw.StartAudio(AudioCallback);

  while(1) {}

}
```

Les deux fonctions font parties de l’objet Seed. Dans le cas de l’utilisation du Pod, changez les en :

```cpp
	hw.seed.StartLog( true );
	hw.seed.PrintLine( "We are printing on the console - POD" );
```

L’utilisation de `hw.StartLog( true );` ou `hw.seed.StartLog( true );` pause le programme tant qu’une console série n’est pas connectée. Repérer le port de votre Daisy Seed et connectez la console série (logiciel `Putty` sur Windows ou `SerialTools` sur MacOS, par exemple) pour visualiser les valeurs printées.

Pour afficher des nombres : 

```cpp
	int my_number = 1234;
	float version = 1.0;
	hw.seed.StartLog( true );
	hw.seed.PrintLine( "We are printing number : %d and version : %f", 
						my_number, 
						version );
```

Pour que l’affichage des floats fonctionne, il faut ajouter ce flag dans le Makefile du projet C++ :

```makefile
# Flags
LDFLAGS = -u _printf_float
```

### 5. Synthétiseur basique complet : Saw + ADSR + Filtre Résonant

Dans cet exemple nous allons utiliser l’objet `MoogLadder` (émulation d’un filtre Moog résonant) qui est présent dans le dossier `DaisySP/DaisySP-LGPL`.

Une fois que vous avez créer un nouveau projet Pod avec l’aide du script helper.py, modifiez le Makefile générer pour y intégrer la ligne suivante (juste après la ligne commençant par `TARGET =`): 

```makefile
USE_DAISYSP_LGPL = 1
```

Remplacez le code présent par ce code : 

```cpp
#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisyPod pod;
Oscillator osc;
MoogLadder flt;
Adsr env;
bool gate = false;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)

{

    pod.ProcessAllControls();

    float freq_param = pod.knob1.Process() * 1000.0f + 40.0f;
    float cutoff = pod.knob2.Process() * 8000.0f + 100.0f;

    float resonance = pod.encoder.Increment() * 0.02f;

    osc.SetFreq(freq_param);
	
    flt.SetFreq(cutoff);
    flt.SetRes(resonance);

    if(pod.button1.RisingEdge())
        gate = true;

    if(pod.button2.RisingEdge())
        gate = false;

    for(size_t i = 0; i < size; i++)
    {

        float sig = osc.Process();
        float amp = env.Process(gate);
        float filt = flt.Process(sig * amp);

        out[ 0 ][ i ] = filt;
        out[ 1 ][ i ] = filt;

    }

}

int main(void)

{

	pod.Init();

	osc.Init(pod.AudioSampleRate());
	osc.SetWaveform(Oscillator::WAVE_SAW);
	flt.Init(pod.AudioSampleRate());
	
	env.Init(pod.AudioSampleRate());
	env.SetTime(ADSR_SEG_ATTACK, 0.01f);
	env.SetTime(ADSR_SEG_DECAY, 0.2f);
	env.SetTime(ADSR_SEG_RELEASE, 0.5f);
	env.SetSustainLevel(0.6f);
	
	pod.StartAdc();
	pod.StartAudio(AudioCallback);
	
	while(1) {}

}
```

### 6. Ressources utiles

Documentation officielle Electrosmith :

- [https://electro-smith.github.io/libDaisy/](https://electro-smith.github.io/libDaisy/)

- [https://github.com/electro-smith/DaisyWiki](https://github.com/electro-smith/DaisyWiki)

Bibliothèques & exemples :

- [https://github.com/electro-smith/libDaisy](https://github.com/electro-smith/libDaisy)

- [https://github.com/electro-smith/DaisySP](https://github.com/electro-smith/DaisySP)

- [https://github.com/electro-smith/DaisyExamples](https://github.com/electro-smith/DaisyExamples)

Forum & communauté :

- [https://forum.electro-smith.com](https://forum.electro-smith.com/)

- [https://discord.gg/electrosmith](https://discord.gg/electrosmith)
