#include "fenetrepilote.h"
#include "ui_fenetrepilote.h"

/*
  Fenetre affichant les différents boutons et leurs états que le pilote peut utiliser pour modifier l'état de son avion.
  */

fenetrePilote::fenetrePilote(Avion* a,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::fenetrePilote)
{
    ui->setupUi(this);
    this->a = a;

    connect(ui->V12, SIGNAL(clicked()), this, SLOT(modifVT12()));
    connect(ui->V23, SIGNAL(clicked()), this, SLOT(modifVT23()));
    connect(ui->V21, SIGNAL(clicked()), this, SLOT(modifV12()));
    connect(ui->V31, SIGNAL(clicked()), this, SLOT(modifV13()));
    connect(ui->V32, SIGNAL(clicked()), this, SLOT(modifV23()));
    connect(ui->P12, SIGNAL(clicked()), this, SLOT(modifP12()));
    connect(ui->P22, SIGNAL(clicked()), this, SLOT(modifP22()));
    connect(ui->P32, SIGNAL(clicked()), this, SLOT(modifP32()));

    this->move(875,50);
}

fenetrePilote::~fenetrePilote()
{
    delete ui;
}

// Met à jour la fenêtre, cliquabilité des boutons / couleurs des cadres

void fenetrePilote::updateFenetre(bool premierAppel)
{

    this->ui->F1->setPalette(this->a->V[0].getCouleur());
    this->ui->F2->setPalette(this->a->V[1].getCouleur());
    this->ui->F6->setPalette(this->a->V[2].getCouleur());
    this->ui->F7->setPalette(this->a->V[3].getCouleur());
    this->ui->F8->setPalette(this->a->V[4].getCouleur());

    this->ui->F3->setPalette(this->a->R[0].getPompe2().getCouleur());
    this->ui->F4->setPalette(this->a->R[1].getPompe2().getCouleur());
    this->ui->F5->setPalette(this->a->R[2].getPompe2().getCouleur());

    this->ui->P12->setEnabled(this->a->R[0].getPompe2().isMarche());
    this->ui->P22->setEnabled(this->a->R[1].getPompe2().isMarche());
    this->ui->P32->setEnabled(this->a->R[2].getPompe2().isMarche());

   if(premierAppel) // Si le bouton a été cliqué sur cette fenêtre
        this->f1->updateFenetre(false);
}

// Ajoute la fenetre mainwindow à ses paramètres

void fenetrePilote::addfenetre(MainWindow *f1)
{
    this->f1 = f1;
}

// Ferme les deux fenetres

void fenetrePilote::closeEvent(QCloseEvent *)
{
    this->f1->close();
    this->close();
}
/*
  Ensemble des slots appelés lors d'appui sur les boutons
  Chaque slot modifie les paramètres de la classe Avion puis appelle la fonction de mise à jour
  */

// Ferme/ouvre la vanne VT12
// Si on la ferme : vérification de la possibilité d'alimenter les autres reservoirs et des possibles répercutions sur l'avion
// Si on ouvre : vérification si un autre moteur n'etait pas alimenté via cette vanne et des possibles répercutions s'il l'était
void fenetrePilote::modifVT12()
{
    if(a->V[0].getOuvert())
    {
        a->V[0].fermer();
        if(!a->R[0].getRempli() && a->R[1].getRempli() && a->nbReservoirVidange() != 3 ){
           a->R[0].setRempli(true);
           if(a->R[0].getPompe1().getEtat() == 1){
               a->moteur[0][0] = 1;
               if(a->moteur[1][0] == 1){
                   modifP22();
                   modifP22();
               }
               if(a->moteur[2][0]){
                   modifP32();
                   modifP32();
               }
           }
           if(a->R[0].getPompe2().getEtat() == 1){
               modifP12();
               modifP12();
           }
        }
        else if(!a->R[1].getRempli() && a->R[0].getRempli() && !a->R[0].getVidange()){
            a->R[1].setRempli(true);
            if(a->R[1].getPompe1().getEtat() == 1){
                a->moteur[1][1] = 1;
                if(a->moteur[0][1] == 1){
                    modifP12();
                    modifP12();
                }
                if(a->moteur[2][1]){
                    modifP32();
                    modifP32();
                }
            }
            if(a->R[1].getPompe2().getEtat() == 1){
                modifP22();
                modifP22();
            }
            if(!a->V[1].getOuvert() && !a->R[2].getRempli()){
                a->R[2].setRempli(true);
                if(a->R[2].getPompe1().getEtat() == 1){
                    a->moteur[2][2] = 1;
                    if(a->moteur[0][2] == 1){
                        modifP12();
                        modifP12();
                    }
                    if(a->moteur[1][2]){
                        modifP22();
                        modifP22();
                    }
                }
                if(a->R[2].getPompe2().getEtat() == 1){
                    modifP32();
                    modifP32();
                }
            }
        }

    }
    else
    {
        a->V[0].ouvrir();
        if(a->R[0].getVidange())
            f1->vidangeR1();
        if(a->R[1].getVidange())
            f1->vidangeR2();
    }
    updateFenetre();
	//si l'ouverture de la vanne répare la panne en cours, alors on arrête le chrono et met à jour le temps total écoulé
    if(!a->actionNecessaire() && (int)f1->getChrono() != 0)
    {
        time_t tmp;
        time(&tmp);
		//si on met moins de 2 secondes à résoudre une panne, ce n'est pas comptabilisé
		//car cela signifie que le pilote savait qu'elle panne allait survenir et donc pas intéressant pour le score
        if((int)(tmp - f1->getChrono()) > 2)
            f1->setTempsTotalEcoule(f1->getTempsTotalEcoule() + (int) (tmp - f1->getChrono()));
        else
            f1->decrementerNombreDeChrono();
        f1->setChrono(0);
    }else if( (int)f1->getChrono() == 0 && a->actionNecessaire())
        f1->demarrerChrono();
}

// Idem que modifVT12
void fenetrePilote::modifVT23()
{
    if(a->V[1].getOuvert())
    {
        a->V[1].fermer();
        if(!a->R[2].getRempli() && a->R[1].getRempli() && a->nbReservoirVidange() != 3 ){
           a->R[2].setRempli(true);
           if(a->R[2].getPompe1().getEtat() == 1){
               a->moteur[2][2] = 1;
               if(a->moteur[1][2] == 1){
                   modifP22();
                   modifP22();
               }
               if(a->moteur[0][2]){
                   modifP12();
                   modifP12();
               }
           }
           if(a->R[2].getPompe2().getEtat() == 1){
               modifP32();
               modifP32();
           }
        }
        else if(!a->R[1].getRempli() && a->R[2].getRempli() && !a->R[2].getVidange()){
            a->R[1].setRempli(true);
            if(a->R[1].getPompe1().getEtat() == 1){
                a->moteur[1][1] = 1;
                if(a->moteur[0][1] == 1){
                    modifP12();
                    modifP12();
                }
                if(a->moteur[2][1]){
                    modifP32();
                    modifP32();
                }
            }
            if(a->R[1].getPompe2().getEtat() == 1){
                modifP22();
                modifP22();
            }
            if(!a->V[0].getOuvert() && !a->R[0].getRempli()){
                a->R[0].setRempli(true);
                if(a->R[0].getPompe1().getEtat() == 1){
                    a->moteur[0][0] = 1;
                    if(a->moteur[1][0] == 1){
                        modifP22();
                        modifP22();
                    }
                    if(a->moteur[2][0]){
                        modifP32();
                        modifP32();
                    }
                }
                if(a->R[0].getPompe2().getEtat() == 1){
                    modifP12();
                    modifP12();
                }
            }
        }
    }else{
        a->V[1].ouvrir();
        if(a->R[2].getVidange())
            f1->vidangeR3();
        if(a->R[1].getVidange())
            f1->vidangeR2();
    }
    updateFenetre();
    if(!a->actionNecessaire() && (int)f1->getChrono() != 0)
    {
        time_t tmp;
        time(&tmp);
        if((int)(tmp - f1->getChrono()) > 2)
            f1->setTempsTotalEcoule(f1->getTempsTotalEcoule() + (int) (tmp - f1->getChrono()));
        else
            f1->decrementerNombreDeChrono();
        f1->setChrono(0);
    }else if( (int)f1->getChrono() == 0 && a->actionNecessaire())
        f1->demarrerChrono();
}

// Ferme/ouvre la vanne V12
// Si on la ferme : vérifie si on peut alimenter un moteur (qui n'est pas déjà alimenté) de cette manière et si oui l'alimente
// Si on l'ouvre : arrête d'alimenter un moteur si on l'alimentait via cette vanne
void fenetrePilote::modifV12()
{
    if(a->V[2].getOuvert())
    {
        a->V[2].fermer();
        if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getPompe1().getEtat() == 1 && a->moteur[0][2] != 1 && a->moteur[1][1] != 1 && a->moteur[2][1] != 1 && a->R[0].getRempli())
            a->moteur[0][1] = 1;
        if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1 && a->moteur[1][2] != 1 && a->moteur[0][0] != 1 && a->moteur[2][0] != 1 && a->R[1].getRempli())
            a->moteur[1][0] = 1;
    }else{
        a->V[2].ouvrir();
        if(a->moteur[0][1] == 1)
        {
            a->moteur[0][1] = 0;
            if(!a->V[3].getOuvert() && a->moteur[2][2] != 1 && a->moteur[1][2] != 1)
                a->moteur[0][2] = 1;
        }
        if(a->moteur[1][0] == 1)
        {
            a->moteur[1][0] = 0;
            if(!a->V[4].getOuvert() && a->moteur[2][2] != 1 && a->moteur[0][2] != 1)
                a->moteur[1][2] = 1;
        }
    }
    updateFenetre();
    if(!a->actionNecessaire() && (int)f1->getChrono() != 0)
    {
        time_t tmp;
        time(&tmp);
        if((int)(tmp - f1->getChrono()) > 2)
            f1->setTempsTotalEcoule(f1->getTempsTotalEcoule() + (int) (tmp - f1->getChrono()));
        else
            f1->decrementerNombreDeChrono();
        f1->setChrono(0);
    }else if( (int)f1->getChrono() == 0 && a->actionNecessaire())
        f1->demarrerChrono();
}

//Idem que modifV12
void fenetrePilote::modifV13()
{
    if(a->V[3].getOuvert())
    {
        a->V[3].fermer();
        if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getPompe1().getEtat() == 1 && a->moteur[0][1] != 1 && a->moteur[1][2] != 1 && a->moteur[2][2] != 1 && a->R[0].getRempli())
            a->moteur[0][2] = 1;
        if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->moteur[2][1] != 1 && a->moteur[1][0] != 1 && a->moteur[0][0] != 1 && a->R[2].getRempli())
            a->moteur[2][0] = 1;
    }else{
        a->V[3].ouvrir();
        if(a->moteur[0][2] == 1)
        {
            a->moteur[0][2] = 0;
            if(!a->V[2].getOuvert() && a->moteur[1][1] != 1 && a->moteur[2][1] != 1)
                a->moteur[0][1] = 1;
        }
        if(a->moteur[2][0] == 1)
        {
            a->moteur[2][0] = 0;
            if(!a->V[4].getOuvert() && a->moteur[1][1] != 1 && a->moteur[0][1] != 1)
                a->moteur[2][1] = 1;
        }
    }
    updateFenetre();
    if(!a->actionNecessaire() && (int)f1->getChrono() != 0)
    {
        time_t tmp;
        time(&tmp);
        if((int)(tmp - f1->getChrono()) > 2)
            f1->setTempsTotalEcoule(f1->getTempsTotalEcoule() + (int) (tmp - f1->getChrono()));
        else
            f1->decrementerNombreDeChrono();
        f1->setChrono(0);
    }else if( (int)f1->getChrono() == 0 && a->actionNecessaire())
        f1->demarrerChrono();
}

//Idem que modifV12
void fenetrePilote::modifV23()
{
    if(a->V[4].getOuvert())
    {
        a->V[4].fermer();
        if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1 && a->moteur[1][0] != 1 && a->moteur[0][2] != 1 && a->moteur[2][2] != 1 && a->R[1].getRempli())
            a->moteur[1][2] = 1;
        if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->moteur[2][0] != 1 && a->moteur[1][1] != 1 && a->moteur[0][1] != 1 && a->R[2].getRempli())
            a->moteur[2][1] = 1;
    }else{
        a->V[4].ouvrir();
        if(a->moteur[1][2] == 1)
        {
            a->moteur[1][2] = 0;
            if(!a->V[2].getOuvert() && a->moteur[0][0] != 1 && a->moteur[2][0] != 1)
                a->moteur[1][0] = 1;
        }
        if(a->moteur[2][1] == 1)
        {
            a->moteur[2][1] = 0;
            if(!a->V[3].getOuvert() && a->moteur[1][0] != 1 && a->moteur[0][0] != 1)
                a->moteur[2][0] = 1;
        }
    }
    updateFenetre();
    if(!a->actionNecessaire() && (int)f1->getChrono() != 0)
    {
        time_t tmp;
        time(&tmp);
        if((int)(tmp - f1->getChrono()) > 2)
            f1->setTempsTotalEcoule(f1->getTempsTotalEcoule() + (int) (tmp - f1->getChrono()));
        else
            f1->decrementerNombreDeChrono();
        f1->setChrono(0);
    }else if( (int)f1->getChrono() == 0 && a->actionNecessaire())
        f1->demarrerChrono();
}

//Allume/éteint la pompe secondaire du moteur correspondant
// Si on l'allume et que le reservoir est rempli, on alimente le moteur ayant le même nombre si la pompe principale n'est pas
// fonctionnelle ou si il n'y a aucun autre moteur à alimenter, dans le cas contraire on alimente l'autre moteur si la vanne correspondante est fermée
// Si on l'éteint, on coupe l'apport en carburant auprès du moteur alimenté par cette pompe et on regarde si une autre pompe peut alimenter le moteur qu'elle alimentait
void fenetrePilote::modifP12()
{
    if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getRempli())
    {
          a->R[0].getPompe2().arret();
          if(a->R[0].getPompe1().getEtat() != 1)
          {
              a->moteur[0][0] = 0;
              if(a->R[1].getPompe2().getEtat() == 1 && !a->V[2].getOuvert() && a->moteur[1][2] != 1 && a->R[1].getPompe1().getEtat() == 1)
                  a->moteur[1][0] = 1;
              else if(a->R[2].getPompe2().getEtat() == 1 && !a->V[3].getOuvert() && a->moteur[2][1] != 1 && a->R[2].getPompe1().getEtat() == 1)
                  a->moteur[2][0] = 1;
          }else{
              if(a->moteur[0][1] == 1)
              {
                  a->moteur[0][1] = 0;
                  if(a->R[2].getPompe1().getEtat() == 1 && !a->V[4].getOuvert() && a->R[2].getPompe2().getEtat() == 1)
                      a->moteur[2][1] = 1;
              }
              if(a->moteur[0][2] == 1)
              {
                  a->moteur[0][2] = 0;
                  if(a->R[1].getPompe1().getEtat() == 1 && !a->V[4].getOuvert() && a->R[1].getPompe2().getEtat() == 1)
                      a->moteur[1][2] = 1;
              }
          }
    }else if(a->R[0].getPompe2().getEtat() == 0 && a->R[0].getRempli()){
        a->R[0].getPompe2().marche();
        if(a->R[0].getPompe1().getEtat() != 1)
        {
            a->moteur[0][0] = 1;
            if(a->moteur[1][0] == 1)
            {
                a->moteur[1][0] = 0;
                if(a->moteur[2][2] == 0 && !a->V[4].getOuvert())
                    a->moteur[1][2] = 1;
            }
            if(a->moteur[2][0] == 1)
            {
                a->moteur[2][0] = 0;
                if(a->moteur[1][1] == 0 && !a->V[4].getOuvert())
                    a->moteur[2][1] = 1;
            }
        }else if(!a->V[2].getOuvert() && a->moteur[1][1] != 1 && a->moteur[2][1] != 1)
            a->moteur[0][1] = 1;
        else if(!a->V[3].getOuvert() && a->moteur[1][2] != 1 && a->moteur[2][2] != 1)
            a->moteur[0][2] = 1;
    }else if(a->R[0].getPompe2().getEtat() == 0 && !a->R[0].getRempli()){
        a->R[0].getPompe2().marche();
    }else if(a->R[0].getPompe2().getEtat() == 1 && !a->R[0].getRempli()){
        a->R[0].getPompe2().arret();
    }
    updateFenetre();
    if(!a->actionNecessaire() && (int)f1->getChrono() != 0)
    {
        time_t tmp;
        time(&tmp);
        if((int)(tmp - f1->getChrono()) > 2)
            f1->setTempsTotalEcoule(f1->getTempsTotalEcoule() + (int) (tmp - f1->getChrono()));
        else
            f1->decrementerNombreDeChrono();
        f1->setChrono(0);
    }else if( (int)f1->getChrono() == 0 && a->actionNecessaire())
        f1->demarrerChrono();
}

//Idem que modifP12
void fenetrePilote::modifP22()
{
    if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getRempli())
    {
        a->R[1].getPompe2().arret();
        if(a->R[1].getPompe1().getEtat() != 1)
        {
            a->moteur[1][1] = 0;
            if(a->R[0].getPompe2().getEtat() == 1 && !a->V[2].getOuvert() && a->moteur[0][2] != 1 && a->R[0].getPompe1().getEtat() == 1)
                a->moteur[0][1] = 1;
            else if(a->R[2].getPompe2().getEtat() == 1 && !a->V[4].getOuvert() && a->moteur[2][0] != 1 && a->R[2].getPompe1().getEtat() == 1)
                a->moteur[2][1] = 1;
        }else{
            if(a->moteur[1][0] == 1)
            {
                a->moteur[1][0] = 0;
                if(a->R[2].getPompe1().getEtat() == 1 && !a->V[3].getOuvert() && a->R[2].getPompe2().getEtat() == 1)
                    a->moteur[2][0] = 1;
            }
            if(a->moteur[1][2] == 1)
            {
                a->moteur[1][2] = 0;
                if(a->R[0].getPompe1().getEtat() == 1 && !a->V[3].getOuvert() && a->R[0].getPompe2().getEtat() == 1)
                    a->moteur[0][2] = 1;
            }
        }
    }else if(a->R[1].getPompe2().getEtat() == 0 && a->R[1].getRempli()){
        a->R[1].getPompe2().marche();
        if(a->R[1].getPompe1().getEtat() != 1)
        {
            a->moteur[1][1] = 1;
            if(a->moteur[0][1] == 1)
            {
                a->moteur[0][1] = 0;
                if(a->moteur[2][2] == 0 && !a->V[3].getOuvert())
                    a->moteur[0][2] = 1;
            }
            if(a->moteur[2][1] == 1)
            {
                a->moteur[2][1] = 0;
                if(a->moteur[0][0] == 0 && !a->V[3].getOuvert())
                    a->moteur[2][0] = 1;
            }
        }
        else if(!a->V[2].getOuvert() && a->moteur[0][0] != 1 && a->moteur[2][0] != 1)
            a->moteur[1][0] = 1;
        else if(!a->V[4].getOuvert() && a->moteur[0][2] != 1 && a->moteur[2][2] != 1)
            a->moteur[1][2] = 1;
    }else if(a->R[1].getPompe2().getEtat() == 0 && !a->R[1].getRempli()){
        a->R[1].getPompe2().marche();
    }else if(a->R[1].getPompe2().getEtat() == 1 && !a->R[1].getRempli()){
        a->R[1].getPompe2().arret();
    }
    updateFenetre();
    if(!a->actionNecessaire() && (int)f1->getChrono() != 0)
    {
        time_t tmp;
        time(&tmp);
        if((int)(tmp - f1->getChrono()) > 2)
            f1->setTempsTotalEcoule(f1->getTempsTotalEcoule() + (int) (tmp - f1->getChrono()));
        else
            f1->decrementerNombreDeChrono();
        f1->setChrono(0);
    }else if( (int)f1->getChrono() == 0 && a->actionNecessaire())
        f1->demarrerChrono();
}

//Idem que modifP12
void fenetrePilote::modifP32()
{
    if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getRempli())
    {
        a->R[2].getPompe2().arret();
        if(a->R[2].getPompe1().getEtat() != 1)
        {
            a->moteur[2][2] = 0;
            if(a->R[0].getPompe2().getEtat() == 1 && !a->V[3].getOuvert() && a->moteur[0][1] != 1 && a->R[0].getPompe1().getEtat() == 1)
                a->moteur[0][2] = 1;
            else if(a->R[1].getPompe2().getEtat() == 1 && !a->V[4].getOuvert() && a->moteur[1][0] != 1 && a->R[1].getPompe1().getEtat() == 1)
                a->moteur[1][2] = 1;
        }else{
            if(a->moteur[2][0] == 1)
            {
                a->moteur[2][0] = 0;
                if(a->R[1].getPompe1().getEtat() == 1 && !a->V[2].getOuvert() && a->R[1].getPompe2().getEtat() == 1)
                    a->moteur[1][0] = 1;
            }
            if(a->moteur[2][1] == 1)
            {
                a->moteur[2][1] = 0;
                if(a->R[0].getPompe1().getEtat() == 1 && !a->V[2].getOuvert() && a->R[0].getPompe2().getEtat() == 1)
                    a->moteur[0][1] = 1;
            }
        }
    }else if(a->R[2].getPompe2().getEtat() == 0 && a->R[2].getRempli()){
        a->R[2].getPompe2().marche();
        if(a->R[2].getPompe1().getEtat() != 1)
        {
            a->moteur[2][2] = 1;
            if(a->moteur[0][2] == 1)
            {
                a->moteur[0][2] = 0;
                if(a->moteur[1][1] == 0 && !a->V[2].getOuvert())
                    a->moteur[0][1] = 1;
            }
            if(a->moteur[1][2] == 1)
            {
                a->moteur[1][2] = 0;
                if(a->moteur[0][0] == 0 && !a->V[2].getOuvert())
                    a->moteur[1][0] = 1;
            }
        }
        else if(!a->V[3].getOuvert() && a->moteur[0][0] != 1 && a->moteur[1][0] != 1)
            a->moteur[2][0] = 1;
        else if(!a->V[4].getOuvert() && a->moteur[0][1] != 1 && a->moteur[1][1] != 1)
            a->moteur[2][1] = 1;
    }else if(a->R[2].getPompe2().getEtat() == 0 && !a->R[2].getRempli()){
        a->R[2].getPompe2().marche();
    }else if(a->R[2].getPompe2().getEtat() == 1 && !a->R[2].getRempli()){
        a->R[2].getPompe2().arret();
    }
    updateFenetre();
    if(!a->actionNecessaire() && (int)f1->getChrono() != 0)
    {
        time_t tmp;
        time(&tmp);
        if((int)(tmp - f1->getChrono()) > 2)
            f1->setTempsTotalEcoule(f1->getTempsTotalEcoule() + (int) (tmp - f1->getChrono()));
        else
            f1->decrementerNombreDeChrono();
        f1->setChrono(0);
    }else if( (int)f1->getChrono() == 0 && a->actionNecessaire())
        f1->demarrerChrono();
}
