Algorithme: ajouter_fichier;
Variables:
    fichier : entier;
    nom : chaine_charactere;
Debut:
    ecrire("Entrez votre nom:");
    lire(nom);

    fichier <- open("visiteurs.txt", "a");
    writeLine(fichier, nom);
    close(fichier);

    ecrire("Nom ajoute au fichier!");
Fin