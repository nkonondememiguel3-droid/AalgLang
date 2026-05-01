Algorithme: gestion_notes;

Type:
    Structure Etudiant
        nom : chaine_charactere;
        notes : tableau[1..5] de reel;
    FinStruct

Fonction: calculerMoyenne(e: Etudiant): reel;
Variables:
    i : entier;
    somme : reel;
Debut:
    somme <- 0,0;
    pour i <- 1 jusqu_a 5 faire:
        somme <- somme + e.notes[i];
    finpour
    retourne somme / 5;
Fin
FinFonction;

Methode: afficherEtudiant(e: Etudiant):
Variables:
    i : entier;
Debut:
    ecrire("=== " + e.nom + " ===");
    ecrire("Notes:");
    pour i <- 1 jusqu_a 5 faire:
        ecrire("  Matiere " + i + ": " + e.notes[i]);
    finpour
    ecrire("Moyenne: " + calculerMoyenne(e));
Fin
FinMethode;

Variables:
    etudiant : Etudiant;
    i : entier;
Debut:
    ecrire("Nom de l'etudiant:");
    lire(etudiant.nom);

    ecrire("Entrez 5 notes:");
    pour i <- 1 jusqu_a 5 faire:
        ecrire("Note " + i + ":");
        lire(etudiant.notes[i]);
    finpour

    afficherEtudiant(etudiant);
Fin