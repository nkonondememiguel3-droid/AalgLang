Algorithme: tableau_structures;

Type:
    Structure Personne
        nom : chaine_charactere;
        age : entier;
    FinStruct

Variables:
    personnes : tableau[1..3] de Personne;
    i : entier;
Debut:
    personnes[1].nom <- "Alice";
    personnes[1].age <- 25;

    personnes[2].nom <- "Bob";
    personnes[2].age <- 30;

    personnes[3].nom <- "Charlie";
    personnes[3].age <- 35;

    ecrire("Liste des personnes:");
    pour i <- 1 jusqu_a 3 faire:
        ecrire(personnes[i].nom + " (" + personnes[i].age + " ans)");
    finpour
Fin
