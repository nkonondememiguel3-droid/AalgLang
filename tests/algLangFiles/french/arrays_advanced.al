Algorithme: tableaux_avance;

Fonction: trouverMax(tab: tableau[1..10] de entier): entier;
Variables:
    i, maximum : entier;
Debut:
    maximum <- tab[1];
    pour i <- 2 jusqu_a 10 faire:
        si tab[i] > maximum alors:
            maximum <- tab[i];
        finsi
    finpour
    retourne maximum;
Fin
FinFonction;

Fonction: trouverMin(tab: tableau[1..10] de entier): entier;
Variables:
    i, minimum : entier;
Debut:
    minimum <- tab[1];
    pour i <- 2 jusqu_a 10 faire:
        si tab[i] < minimum alors:
            minimum <- tab[i];
        finsi
    finpour
    retourne minimum;
Fin
FinFonction;

Variables:
    valeurs : tableau[1..10] de entier;
    i : entier;
Debut:
    // Remplissage avec des valeurs aléatoires
    valeurs[1] <- 45;
    valeurs[2] <- 23;
    valeurs[3] <- 89;
    valeurs[4] <- 12;
    valeurs[5] <- 67;
    valeurs[6] <- 34;
    valeurs[7] <- 91;
    valeurs[8] <- 56;
    valeurs[9] <- 78;
    valeurs[10] <- 29;

    ecrire("Valeurs:");
    pour i <- 1 jusqu_a 10 faire:
        ecrire(valeurs[i]);
    finpour

    ecrire("Maximum: " + trouverMax(valeurs));
    ecrire("Minimum: " + trouverMin(valeurs));
Fin