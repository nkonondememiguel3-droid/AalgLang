Algorithme: fonctions_avancees;

Fonction: max(a: entier, b: entier): entier;
Debut:
    si a > b alors:
        retourne a;
    sinon:
        retourne b;
    finsi
Fin
FinFonction;

Fonction: min(a: entier, b: entier): entier;
Debut:
    si a < b alors:
        retourne a;
    sinon:
        retourne b;
    finsi
Fin
FinFonction;

Fonction: abs(n: entier): entier;
Debut:
    si n < 0 alors:
        retourne -n;
    sinon:
        retourne n;
    finsi
Fin
FinFonction;

Variables:
    x, y : entier;
Debut:
    x <- -15;
    y <- 20;

    ecrire("x = " + x);
    ecrire("y = " + y);
    ecrire("max(x, y) = " + max(x, y));
    ecrire("min(x, y) = " + min(x, y));
    ecrire("abs(x) = " + abs(x));
Fin