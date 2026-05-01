Algorithme: ecrire_fichier;
Variables:
    fichier : entier;
    i : entier;
Debut:
    fichier <- open("output.txt", "w");

    writeLine(fichier, "=== Nombres pairs ===");
    pour i <- 2 jusqu_a 20 pas 2 faire:
        writeLine(fichier, "Nombre: " + i);
    finpour

    close(fichier);
    ecrire("Fichier cree avec succes!");
Fin