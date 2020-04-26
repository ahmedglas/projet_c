#ifndef POMPE_H
#define POMPE_H
using namespace std;
#include <iostream>
#include <QPalette>

class Pompe
{
private :
	int etat;
	// -1 panne / 0 arret / 1 marche
	
public :
	Pompe();
    Pompe(int etat);
	
    friend ostream& operator << (ostream & os, Pompe p);
	void panne();
	void marche();
    void arret();
    int getEnFonction();
	
    int getEtat() const;
    bool isMarche();
    QPalette getCouleur();

	
	~Pompe();
	
};

#endif
