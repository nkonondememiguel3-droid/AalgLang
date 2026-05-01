Algorithme: lire_fichier;
Variables:
    fichier : entier;
    ligne : chaine_charactere;
Debut:
    fichier <- open("input.txt", "r");

    tant_que (non eof(fichier)) faire:
        ligne <- readLine(fichier);
        ecrire(ligne);
    fintantque

    // close(fichier);
Fin