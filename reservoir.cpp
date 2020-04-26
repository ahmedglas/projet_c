#include "reservoir.h"

/*
 * Cette classe représente les reservoirs de l'avion.
 * Un réservoir a deux types d'état, vidange et rempli.
 * En effet si il est alimenté par un réservoir voisin il peut être à la fois vidange et rempli
 * Cependant au moment ou la vanne est réouverte entre les deux réservoirs il retourne aux états vidange et vide
 */


Reservoir :: Reservoir() : p1(1), p2(0)
{
    this->rempli = false;
    this->vidange = false;
}

Reservoir :: Reservoir(bool remplissage) : p1(1), p2(0)
{
	this->rempli = remplissage;
    this->vidange = false;
}


ostream & operator << (ostream& os, Reservoir r)
{
    if(r.getRempli())
        os << "Ceci est un Réservoir rempli" << endl;
    else
        os << "Ceci est un Réservoir vide" << endl;

    return os;
}

//Panne du réservoir

void Reservoir :: vidanger()
{
	this->rempli = false;
    this->vidange = true;
}

//Renvoie le remplissage du reservoir

bool Reservoir :: getRempli()
{
    return this->rempli;
}

//Rempli le réservoir

void Reservoir :: setRempli(bool rempli)
{
    this->rempli = rempli;
}

//Renvoie l'etat de vidange du reservoir

bool Reservoir :: getVidange()
{
    return this->vidange;
}

// Utilisée pour simplifier du code (updateFenetre de mainwindow)

QPalette Reservoir::getCouleur()
{
    if(this->rempli)
        return Qt::green;
    else if(this->vidange)
        return Qt::blue;
    else
        return Qt::red;
}

//Renvoie une référence sur la P1 pour pouvoir la modifier

Pompe &Reservoir::getPompe1()
{
    return this->p1;
}

//Renvoie une référence sur la P2 pour pouvoir la modifier

Pompe &Reservoir::getPompe2()
{
    return this->p2;
}

Reservoir :: ~Reservoir(){}

// Remet le réservoir à sa configuration d'origine

void Reservoir::reset()
{
    this->rempli = true;
    this->vidange = false;
    this->p1.marche();
    this->p2.arret();
}

