Algorithme: nombres_premiers;

Fonction: estPremier(n: entier): booleen;
Variables:
    i : entier;
Debut:
    si n < 2 alors:
        retourne faux;
    finsi

    si n == 2 alors:
        retourne vrai;
    finsi

    i <- 2;
    tant_que (i * i <= n) faire:
        si n mod i == 0 alors:
            retourne faux;
        finsi
        i <- i + 1;
    fintantque

    retourne vrai;
Fin
FinFonction;

Variables:
    i : entier;
Debut:
    ecrire("Nombres premiers jusqu'a 50:");
    pour i <- 2 jusqu_a 50 faire:
        si estPremier(i) alors:
            ecrire(i);
        finsi
    finpour
Fin