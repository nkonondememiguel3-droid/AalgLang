Algorithme: arithmetique;
Variables:
    a, b : entier;
    somme, diff, prod, quot, reste : entier;
Debut:
    a <- 17;
    b <- 5;

    somme <- a + b;
    diff <- a - b;
    prod <- a * b;
    quot <- a / b;
    reste <- a mod b;

    ecrire("a = " + a);
    ecrire("b = " + b);
    ecrire("a + b = " + somme);
    ecrire("a - b = " + diff);
    ecrire("a * b = " + prod);
    ecrire("a / b = " + quot);
    ecrire("a mod b = " + reste);
Fin