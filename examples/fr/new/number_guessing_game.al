Algorithme: jeu_devinette;
Variables:
    nbr : entier;
    essai : entier;
    tentatives : entier;
    rejouer : booleen;
Debut:
    rejouer <- vrai;

    tant_que (rejouer) faire:
        nbr <- 42;  // In real game, use random number
        tentatives <- 0;

        ecrire("=== Jeu de devinette ===");
        ecrire("Devinez le nombre entre 1 et 100");

        repeter:
            ecrire("Votre essai:");
            lire(essai);
            tentatives <- tentatives + 1;

            si essai < nbr alors:
                ecrire("Trop petit!");
            sinon si essai > nbr alors:
                ecrire("Trop grand!");
            finsi
        jusqu_a (essai == nbr);

        ecrire("Bravo! Trouve en " + tentatives + " tentatives!");

        ecrire("Rejouer? (1=oui, 0=non)");
        lire(rejouer);
    fintantque
Fin