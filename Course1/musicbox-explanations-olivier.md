# Explications de MusicBox (Séquenceur Polyphonique CSV)

Ce code lit des fichiers midi convertis en fichiers CSV stockés sur une carte SD et joue les notes de manière autonome grâce à un séquenceur temporel. Le son est généré par un synthétiseur polyphonique à 8 voix. Le signal passe ensuite par un filtre et une réverbération. Les chargements de fichiers sont gérés de manière asynchrone pour garantir un son fluide.

L'exemple original se trouve ici : https://github.com/electro-smith/DaisyExamples/tree/master/pod/MusicBox

Fichier écrit par Olivier CLAVIER (2026).


## Membres de la classe

### Objets

- DaisyPod (`hw`) : Permet de gérer l'interface matérielle spécifique au Pod (boutons, potentiomètres, etc.).
- Oscillator (`osc[MAX_VOICES]`) : Un tableau de 8 générateurs de signaux pour permettre la polyphonie.
- AdEnv (`env[MAX_VOICES]`) : Un tableau de 8 enveloppes Attack/Decay pour sculpter le volume de chaque voix séparément.
- Svf (`filt`) : Un filtre d'état variable (passe-bas) pour adoucir le timbre global.
- ReverbSc (`verb`) : Un module de réverbération stéréo pour ajouter de l'espace au son.
- Parameter (`p_xf`) : Utilitaire pour lisser et convertir la valeur du potentiomètre vers une plage de fréquences logarithmique.
- SdmmcHandler (`sdcard`) & FatFSInterface (`fsi`) : Objets permettant de communiquer avec la carte SD et les fichiers.
- FIL (`fichier`) : Objet représentant le fichier ouvert en cours de lecture.

### Autres variables

- `MAX_VOICES` et `MAX_NOTES` : Constantes définissant les limites du système, 8 voix et 20 000 notes maximum pour le fichier CSV.
- `NoteEvent mon_morceau[MAX_NOTES]` :  Tableau de structures contenant le temps et la hauteur de chaque note. La balise `DSY_SDRAM_BSS` force le stockage dans la grande mémoire externe (64 Mo) pour éviter la saturation du processeur.
- `current_time_sec` et `current_step` : L'horloge du séquenceur et le pointeur indiquant la prochaine note à lire.
- `playlist[30][64]` : Un tableau à deux dimensions stockant les noms de fichiers trouvés sur la carte SD.
- `change_track` : Une variable booléenne déclaré `volatile` permettant au bloc audio de signaler au programme principal qu'il faut changer de musique.


## Fonctions de la classe

### Main 

```
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
```

Dans la fonction `main`, les opérations suivantes sont effectuées :

- Initialisation de la carte, configuration de la mémoire SDRAM et montage matériel de la carte SD (`f_mount`).
- Création de la playlist et chargement en mémoire de la première chanson.
- Initialisation de tous les objets DSP (`osc`, `env`, `filt`, `verb`) avec le bon taux d'échantillonnage (`hw.AudioSampleRate()`).
- `hw.StartAudio(audio)` lance le moteur sonore en tâche de fond.
- La boucle infinie `while(1)` surveille `change_track`. Si l'utilisateur appuie sur le bouton, le chargement du fichier CSV depuis la carte SD est effectué.


### AudioCallback

```
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

```

Dans la fonction AudioCallback, les opérations suivantes sont effectuées :

- Récupère les valeurs des boutons pour mettre en pause ou demander un changement de piste.
- Calcule la vitesse de lecture via une équation linéaire sur le Knob 1, et la fréquence du filtre via l'objet `Parameter` sur le Knob 2.
- Séquenceur temporel : Fait avancer le temps (`current_time_sec`). Une boucle `while` récupères les notes dont l'heure est arrivée et déclenche les enveloppes. L'assignation des voix se fait par un modulo pour éviter de couper les notes précédentes.
- Mixeur polyphonique : Additionne les signaux des 8 oscillateurs multipliés par leurs 8 enveloppes dans une variable `rawsig`.
- Applique le filtre passe-bas, traite la réverbération stéréo, et envoie le résultat final aux sorties `out[i]` et `out[i+1]` avec une réduction de volume (`0.5f`) pour éviter la saturation.

### Autre fonction

```
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
```

- Ouvre le dossier "music" sur la carte SD et lit les fichiers un par un à l'aide de `f_readdir`.
- Ignore les fichiers cachés (commençant par `.`) et filtre les résultats pour ne conserver que les fichiers contenant l'extension `.csv`.


```
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
```

- Créer le chemin du fichier avec `sprintf` et l'ouvre.
- Lit le texte ligne par ligne (`f_gets`).
- Isole les valeurs en remplaçant la virgule par un terminateur nul (`\0`).
- Convertit le texte en réels (`atof`) et entiers (`atoi`) pour les stocker dans le grand tableau `mon_morceau`.


## Exemples pour les différents concepts clés

### Mixage Polyphonique (DSP) 

Au lieu de calculer un seul son, le système boucle sur toutes les voix disponibles pour créer un signal avec des accords.

```
float rawsig = 0; // Remise à zéro impérative à chaque échantillon
for(int v=0; v<MAX_VOICES; v++) {
    // On additionne (mixe) le son de chaque oscillateur multiplié par le volume de son enveloppe
    rawsig += osc[v].Process() * env[v].Process();
}
```

### Allocation des Voix (Round-Robin)

Pour s'assurer qu'une nouvelle note ne coupe pas brusquement la résonance d'une ancienne note, le système distribue les ordres en boucle sur les 8 voix.

```
// On joue la note sur la voix courante
env[current_voice].Trigger();
// On passe à la voix suivante. Arrivé à 7, le modulo (%) ramène la valeur à 0.
current_voice = (current_voice + 1) % MAX_VOICES;
```

### Parsing de Fichier (Extraction de données)

La transformation d'une ligne de texte brute ("1.50, 60") en données numériques s'appuie sur la manipulation directe de la mémoire.

```
char ligne[] = "1.50, 60";
char* v = strchr(ligne, ','); // Trouve la position de la virgule
if (v) {
    *v = '\0'; // Coupe la chaîne en deux en insérant un terminateur
    float temps = atof(ligne); // Lit "1.50"
    int note = atoi(v + 1);    // Décale la lecture et lit " 60"
}
```