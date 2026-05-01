Algorithme: methodes;

Methode: afficherMessage(msg: chaine_charactere):
Debut:
    ecrire("=================");
    ecrire(msg);
    ecrire("=================");
Fin
FinMethode;

Methode: afficherTableMultiplication(n: entier):
Variables:
    i : entier;
Debut:
    ecrire("Table de " + n + ":");
    pour i <- 1 jusqu_a 10 faire:
        ecrire(n + " x " + i + " = " + (n * i));
    finpour
Fin
FinMethode;

Debut:
    afficherMessage("Bienvenue!");
    afficherTableMultiplication(7);
Fin
