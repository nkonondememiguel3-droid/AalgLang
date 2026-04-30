Algorithme: tableau_parametre;

Fonction: sommeTableau(tab: tableau[1..10] de entier): entier;
Variables:
    i, total : entier;
Debut:
    total <- 0;
    pour i <- 1 jusqu_a 10 faire:
        total <- total + tab[i];
    finpour
    retourne total;
Fin
FinFonction;

Variables:
    nombres : tableau[1..10] de entier;
    i, resultat : entier;
Debut:
    // Fill array
    pour i <- 1 jusqu_a 10 faire:
        nombres[i] <- i * 2;
    finpour

    resultat <- sommeTableau(nombres);
    ecrire("Somme: " + resultat);
Fin