Algorithme: comparaisons;
Variables:
    x, y : entier;
    resultat : booleen;
Debut:
    x <- 10;
    y <- 5;

    resultat <- x > y;
    ecrire(x + " > " + y + " = " + resultat);

    resultat <- x < y;
    ecrire(x + " < " + y + " = " + resultat);

    resultat <- x == y;
    ecrire(x + " == " + y + " = " + resultat);

    resultat <- (x > 0) et (y > 0);
    ecrire("Les deux positifs = " + resultat);

    resultat <- (x < 0) ou (y < 0);
    ecrire("Au moins un negatif = " + resultat);
Fin