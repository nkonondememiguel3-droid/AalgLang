Algorithme: calculatrice;
Variables:
    a, b : reel;
    operation : caractere;
    resultat : reel;
    continuer : booleen;
Debut:
    continuer <- vrai;

    tant_que (continuer) faire:
        ecrire("Entrez le premier nombre:");
        lire(a);

        ecrire("Entrez l'operation (+, -, *, /):");
        lire(operation);

        ecrire("Entrez le deuxieme nombre:");
        lire(b);

        si operation == '+' alors:
            resultat <- a + b;
            ecrire("Resultat: " + resultat);
        sinon si operation == '-' alors:
            resultat <- a - b;
            ecrire("Resultat: " + resultat);
        sinon si operation == '*' alors:
            resultat <- a * b;
            ecrire("Resultat: " + resultat);
        sinon si operation == '/' alors:
            si b != 0,0 alors:
                resultat <- a / b;
                ecrire("Resultat: " + resultat);
            sinon:
                ecrire("Erreur: division par zero!");
            finsi
        sinon:
            ecrire("Operation invalide!");
        finsi

        ecrire("Continuer? (1=oui, 0=non)");
        lire(continuer);
    fintantque

    ecrire("Au revoir!");
Fin