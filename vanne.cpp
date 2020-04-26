#include "vanne.h"

/* Classe représentant les vannes dans l'avion
 * Une vanne ne sait pas à quoi elle est reliée.
 * Son traitement se fait donc principalement dans avion/mainwindow
 */

Vanne :: Vanne()
{
    this->ouvert = true;
}

Vanne :: Vanne(bool ouverture)
{
	this->ouvert = ouverture;
}

ostream & operator << (ostream& os, Vanne v)
{
    if(v.getOuvert())
        os << "Ceci est une vanne ouverte" << endl;
    else
        os << "Ceci est une vanne fermée" << endl;

    return os;
}

// Fermeture de la vanne

void Vanne :: fermer()
{
	this->ouvert = false;
}

//Ouverture de la vanne

void Vanne :: ouvrir()
{
	this->ouvert = true;
}

//Renvoie l'état de la vanne

bool Vanne :: getOuvert()
{
    return this->ouvert;
}

//Fonction utilisée pour simplifier du code (updateFenetre de mainwindow)

QPixmap Vanne::getpixmap()
{
    if(this->ouvert)
        return QPixmap(":/icones/rouge");
    else
        return QPixmap(":/icones/vert");
}
int Vanne::getprog()
{

    if(this->ouvert)
        return 0;
    else
        return 100;


}


// Fonction utilisée pour simplifier du code (updateFenetre de mainwindow)

QPalette Vanne::getCouleur()
{
    if(this->ouvert)
        return Qt::red;
    else
        return Qt::green;
}

Vanne :: ~Vanne(){}
