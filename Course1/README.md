# Cours 1 : études des DaisyExamples pour Pod

Pour ce cours, vous allez étudier des exemples déjà fonctionnels et les expliquer à vos collègues.
Pour ce faire, tout d'abord, choisissez un exemple des DaisyExamples pour le Pod parmi ceux repris ci-dessous : 

```
DaisyExamples
├── pod
│   ├── ChordMachine
│   ├── EuclideanDrums
│   ├── Looper
│   ├── MultiEffect
│   ├── MusicBox
│   ├── SimpleOscillator
│   ├── StepSequencer
│   └── SynthVoice
```

A partir de cet exemple : 
1. Etudiez-le : 

2. Expliquer les différentes parties en remplissant le template `NomExemple-explanations-DONT_MODIFY_BUT_DUPLICATE.md`
	1. Vous devez tout d'abord cloner le dépôt Github sur lequel vous vous trouvez : 
	
	```
	git clone https://github.com/loicreboursiere/Courses-DaisySeedAndPod.git
	```
	
	2. Créez une branche pour votre exemple avec la commande : 
	
	```
	git switch --create NomExemple-explanations 
	```
	
	3. Dans le dossier `Course1`dupliquer le fichier `NomExemple-explanations-DONT_MODIFY_BUT_DUPLICATE.md`, renommer le en fonction du nom de votre exemple
	4. Les éléments de formatage présents dans ce fichier devraient être suffisants pour écrire le document final.

2. Dans le fichier, il vous est demandé : 
	1. D'expliquer les différentes parties du code (fonctions et/ou parties de fonctions)
	2. Proposer un exemple, commenté lorsque nécessaire, pour chaque object pertinent de votre code (à priori, tout type en dehors des types de bases C++) 
	démontrant les différentes fonctions pouvant être utilisées avec cet objet et les différents paramètres

3. Une fois que vous avez fini votre fichier, vous allez pousser le contenu de votre branche sur le dépôt Github principal :

```
git commit -a -m "Création de la branche NomExemple-explanations et du document d'explication - Vos prénom et nom" && git push origin NomExemple-explanations
```

4. Pour des informations plus spécifiques sur les éléments d'UI présents sur le Pod, vous pouvez vous référer aux exemples ci-dessous
 
```
DaisyExamples
├── pod
│   ├── Encoder
│   ├── Midi
│   ├── SimpleButton
│   ├── SimpleLed
```