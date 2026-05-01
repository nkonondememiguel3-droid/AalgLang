Algorithme: tri_bulles;

Methode: triBulles(tab: tableau[1..10] de entier):
Variables:
    i, j, temp : entier;
Debut:
    pour i <- 1 jusqu_a 9 faire:
        pour j <- 1 jusqu_a (10 - i) faire:
            si tab[j] > tab[j + 1] alors:
                // Echange
                temp <- tab[j];
                tab[j] <- tab[j + 1];
                tab[j + 1] <- temp;
            finsi
        finpour
    finpour
Fin
FinMethode;

Methode: afficherTableau(tab: tableau[1..10] de entier):
Variables:
    i : entier;
Debut:
    pour i <- 1 jusqu_a 10 faire:
        ecrire(tab[i]);
    finpour
Fin
FinMethode;

Variables:
    nombres : tableau[1..10] de entier;
Debut:
    nombres[1] <- 64;
    nombres[2] <- 34;
    nombres[3] <- 25;
    nombres[4] <- 12;
    nombres[5] <- 22;
    nombres[6] <- 11;
    nombres[7] <- 90;
    nombres[8] <- 88;
    nombres[9] <- 45;
    nombres[10] <- 50;

    ecrire("Avant tri:");
    afficherTableau(nombres);

    triBulles(nombres);

    ecrire("Apres tri:");
    afficherTableau(nombres);
Fin