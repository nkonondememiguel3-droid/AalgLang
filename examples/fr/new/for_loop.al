Algorithme: boucle_for;
Variables:
    i, somme : entier;
Debut:
    // Somme de 1 à 10
    somme <- 0;
    pour i <- 1 jusqu_a 10 faire:
        somme <- somme + i;
    finpour
    ecrire("Somme 1 a 10: " + somme);

    // Compte à rebours
    ecrire("Compte a rebours:");
    pour i <- 10 jusqu_a 1 pas -1 faire:
        ecrire(i);
    finpour
    ecrire("Decollage!");

    // Sauts de 2
    ecrire("Nombres pairs:");
    pour i <- 0 jusqu_a 10 pas 2 faire:
        ecrire(i);
    finpour
Fin