#ifndef AVION_H
#define AVION_H

#include "reservoir.h"
#include "vanne.h"
#include <QPixmap>
#include <QPalette>

class Avion
{
private:
    Reservoir R[3];
    Vanne V[5];
    int moteur[3][3]; // 1ere dimension : numéro du moteur
                      // 2eme dimension : les reservoir qui l'alimentent



public:
    Avion();
    void reset();
    QPixmap getCouleurAlimMoteur(int i, int j);
    QPalette isAlimente(int i);

    friend class MainWindow;
    friend class fenetrePilote;

    int nbReservoirVidange();
    bool peutFonctionner();
    bool actionNecessaire();

};

#endif
