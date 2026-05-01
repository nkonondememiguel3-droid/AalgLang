Algorithme: factorielle_iterative;

Fonction: factorielle(n: entier): entier;
Variables:
    resultat, i : entier;
Debut:
    resultat <- 1;
    pour i <- 1 jusqu_a n faire:
        resultat <- resultat * i;
    finpour
    retourne resultat;
Fin
FinFonction;

Variables:
    n : entier;
Debut:
    ecrire("Factorielles de 0 a 10:");
    pour n <- 0 jusqu_a 10 faire:
        ecrire("fact(" + n + ") = " + factorielle(n));
    finpour
Fin
