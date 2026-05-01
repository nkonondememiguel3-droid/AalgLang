Algorithme: structures_base;

Type:
    Structure Point
        // x, y : entier;
        x : entier;
        y : entier;
    FinStruct

Variables:
    p1, p2 : Point;
Debut:
    // Initialisation
    p1.x <- 10;
    p1.y <- 20;

    p2.x <- 30;
    p2.y <- 40;

    // Affichage
    ecrire("Point 1: (" + p1.x + ", " + p1.y + ")");
    ecrire("Point 2: (" + p2.x + ", " + p2.y + ")");

    // Modification
    p1.x <- p1.x + p2.x;
    p1.y <- p1.y + p2.y;

    ecrire("Somme: (" + p1.x + ", " + p1.y + ")");
Fin