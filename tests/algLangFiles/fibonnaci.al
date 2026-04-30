Algorithme: fibonacci;

Fonction: fib(n: entier): entier;
Variables:
    a, b, temp, i : entier;
Debut:
    si n == 0 alors:
        retourne 0;
    finsi

    si n == 1 alors:
        retourne 1;
    finsi

    a <- 0;
    b <- 1;

    pour i <- 2 jusqu_a n faire:
        temp <- a + b;
        a <- b;
        b <- temp;
    finpour

    retourne b;
Fin
FinFonction;

Variables:
    i : entier;
Debut:
    ecrire("Suite de Fibonacci:");
    pour i <- 0 jusqu_a 15 faire:
        ecrire("fib(" + i + ") = " + fib(i));
    finpour
Fin