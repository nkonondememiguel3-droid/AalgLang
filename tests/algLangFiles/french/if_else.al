Algorithme: conditions;
Variables:
    score : entier;
    mention : chaine_charactere;
Debut:
    score <- 75;

    si score >= 90 alors:
        mention <- "Excellent";
    sinon si score >= 80 alors:
        mention <- "Tres bien";
    sinon si score >= 70 alors:
        mention <- "Bien";
    sinon si score >= 60 alors:
        mention <- "Passable";
    sinon:
        mention <- "Insuffisant";
    finsi

    ecrire("Score: " + score);
    ecrire("Mention: " + mention);
Fin