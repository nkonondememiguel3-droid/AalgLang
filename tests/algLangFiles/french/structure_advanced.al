Algorithme: structures_avance;

Type:
    Structure Etudiant
        nom : chaine_charactere;
        age : entier;
        notes : tableau[1..3] de reel;
    FinStruct

Fonction: calculerMoyenne(e: Etudiant): reel;
Variables:
    somme : reel;
    i : entier;
Debut:
    somme <- 0,0;
    pour i <- 1 jusqu_a 3 faire:
        somme <- somme + e.notes[i];
    finpour
    retourne somme / 3;
Fin
FinFonction;

Methode: afficherEtudiant(e: Etudiant):
Variables:
    i : entier;
Debut:
    ecrire("=== " + e.nom + " ===");
    ecrire("Age: " + e.age + " ans");
    ecrire("Notes:");
    pour i <- 1 jusqu_a 3 faire:
        ecrire("  Matiere " + i + ": " + e.notes[i]);
    finpour
    ecrire("Moyenne: " + calculerMoyenne(e));
Fin
FinMethode;

Variables:
    etudiant : Etudiant;
Debut:
    etudiant.nom <- "Alice Dupont";
    etudiant.age <- 20;
    etudiant.notes[1] <- 15,5;
    etudiant.notes[2] <- 18,0;
    etudiant.notes[3] <- 16,5;

    afficherEtudiant(etudiant);
Fin