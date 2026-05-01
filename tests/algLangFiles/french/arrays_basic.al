Algorithme: tableaux_base;
Variables:
    nombres : tableau[1..5] de entier;
    i, somme : entier;
Debut:
    // Initialisation
    nombres[1] <- 10;
    nombres[2] <- 20;
    nombres[3] <- 30;
    nombres[4] <- 40;
    nombres[5] <- 50;

    // Affichage
    ecrire("Elements du tableau:");
    pour i <- 1 jusqu_a 5 faire:
        ecrire("nombres[" + i + "] = " + nombres[i]);
    finpour

    // Calcul de la somme
    somme <- 0;
    pour i <- 1 jusqu_a 5 faire:
        somme <- somme + nombres[i];
    finpour
    ecrire("Somme: " + somme);
    ecrire("Moyenne: " + (somme / 5));
Fin