using namespace std;
#include <iostream>
#include <string.h>
#include "pompe.h"

/*
 * La classe pompe représente les pompes de l'avion et sont contenues par paires dans un réservoir
 */


Pompe :: Pompe()
{
	this->etat = -1;
}

Pompe :: Pompe(int etat)
{
	this->etat = etat;
}

// Met en panne la pompe

void Pompe :: panne()
{
	this->etat = -1;
}

//Arrête la pompe

void Pompe :: arret()
{
    this->etat = 0;
}

//Met la pompe en marche

void Pompe :: marche()
{
    this->etat = 1;
}

//Retourne l'état de la pompe

int Pompe :: getEtat() const
{
    return this->etat;
}

//Indique si la pompe peut fonctionner

int Pompe::getEnFonction()
{
    if(this->etat == -1)
        return 0;
    else
        return 1;
}

//Similaire à getEnFonction mais nécessaire pour simplifier du code

bool Pompe::isMarche()
{
    if(this->etat == -1)
        return false;
    else
        return true;
}

QPalette Pompe::getCouleur()
{
    if(this->etat == -1)
        return Qt::blue;
    else if(!this->etat)
        return Qt::red;
    else
        return Qt::green;
}


ostream & operator << (ostream & os, Pompe p)
{
    if(p.getEtat() == -1)
        os << "Ceci est une pompe en panne" << endl;
    else if(p.getEtat() == 0)
        os << "Ceci est une pompe à l'arrêt" << endl;
    else
        os << "Ceci est une pompe en marche" << endl;

    return os;
}

Pompe :: ~Pompe() {}


