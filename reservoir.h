#ifndef RESERVOIR_H
#define RESERVOIR_H
using namespace std;
#include <iostream>
#include "pompe.h"
#include <QPalette>
#include <iostream>

class Reservoir
{
private :
	bool rempli;
    bool vidange;
    Pompe p1;
    Pompe p2;
	
public :
	Reservoir();
    Reservoir(bool remplissage);

    friend ostream & operator << (ostream& os, Reservoir r);

    void vidanger();
    bool getRempli();
    void setRempli(bool rempli);
    bool getVidange();
    QPalette getCouleur();
    Pompe& getPompe1();
    Pompe& getPompe2();

    void reset();
	
	~Reservoir();
	
};

#endif
