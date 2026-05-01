Algorithme: boucle_while;
Variables:
    i, n : entier;
Debut:
    // Compte jusqu'à 5
    i <- 1;
    tant_que (i <= 5) faire:
        ecrire("Iteration " + i);
        i <- i + 1;
    fintantque

    // Puissances de 2
    ecrire("Puissances de 2:");
    n <- 1;
    tant_que (n <= 100) faire:
        ecrire(n);
        n <- n * 2;
    fintantque
Fin
