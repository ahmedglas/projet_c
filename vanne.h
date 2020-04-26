#ifndef VANNE_H
#define VANNE_H

using namespace std;
#include <iostream>
#include <QPixmap>
#include <QPalette>
#include <string.h>

class Vanne
{
private :
	bool ouvert;
	
public :
	Vanne();
    Vanne(bool ouverture);

    friend ostream & operator << (ostream& os, Vanne v);

	void ouvrir();
	void fermer();

	bool getOuvert();
    QPixmap getpixmap();
    QPalette getCouleur();
    int getprog();
	
	~Vanne();
	
};

#endif
