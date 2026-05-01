Algorithme: fonctions_simples;

Fonction: carre(n: entier): entier;
Debut:
    retourne n * n;
Fin
FinFonction;

Fonction: cube(n: entier): entier;
Debut:
    retourne n * n * n;
Fin
FinFonction;

Variables:
    x : entier;
Debut:
    x <- 5;
    ecrire("x = " + x);
    ecrire("carre(x) = " + carre(x));
    ecrire("cube(x) = " + cube(x));
Fin