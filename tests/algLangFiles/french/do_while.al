Algorithme: boucle_repeter;
Variables:
    i, somme : entier;
Debut:
    i <- 1;
    somme <- 0;

    repeter:
        ecrire("i = " + i);
        somme <- somme + i;
        i <- i + 1;
    jusqu_a (i > 5);

    ecrire("Somme: " + somme);
Fin