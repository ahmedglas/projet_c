#include "mainwindow.h"
#include "ui_mainwindow.h"

/*
 * Fenetre principale du programme.
 * Permet de lancer/stopper/réinitialiser la simulation, de lancer les pannes auto, de changer d'utilisateur,
 * d'afficher ses scores, de supprimer son historique et d'afficher l'aide.
 * Cette classe contient la majorité des calculs effectués dans le programme.
 * Les attributs timer/ordrePanneAlea sont utilisés pour les pannes automatiques
 * Les attributs chrono/tempsTotalEcoule/nombreDeChrono sont utilisés pour le calcul du score
 */

//

MainWindow::MainWindow(Avion* a, QString nom, QSqlDatabase BDD, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->timer = new QTimer(this);
    ui->setupUi(this);
    this->a = a;
    this->nom = nom;
    this->chrono = 0;
    this->tempsTotalEcoule = 0;
    this->nombreDeChrono = 0;
    this->BDD = BDD;

    // Initialisation du choix aléatoire de panne
    for(int i = 0; i < 9; i++)
        this->ordrePanneAlea[i] = i;

    this->ordrePanneAlea[9] = 0;
    qsrand(time(NULL));
    int k,l, temp;

    for(int i = 0; i < 30; i++)
    {
        k = qrand() % 9;
        l = qrand() % 9;
        temp = ordrePanneAlea[k];
        ordrePanneAlea[k] = ordrePanneAlea[l];
        ordrePanneAlea[l] = temp;
    }
    // Fin de l'initialisation de choix aléatoire de panne

    connect(timer, SIGNAL(timeout()), this, SLOT(panneAlea()));

    connect(ui->P11, SIGNAL(clicked()), this, SLOT(panneP11()));
    connect(ui->P12, SIGNAL(clicked()), this, SLOT(panneP12()));
    connect(ui->P21, SIGNAL(clicked()), this, SLOT(panneP21()));
    connect(ui->P22, SIGNAL(clicked()), this, SLOT(panneP22()));
    connect(ui->P31, SIGNAL(clicked()), this, SLOT(panneP31()));
    connect(ui->P32, SIGNAL(clicked()), this, SLOT(panneP32()));
    connect(ui->R1, SIGNAL(clicked()), this, SLOT(vidangeR1()));
    connect(ui->R2, SIGNAL(clicked()), this, SLOT(vidangeR2()));
    connect(ui->R3, SIGNAL(clicked()), this, SLOT(vidangeR3()));

    this->move(0, 50);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Ajoute la fenêtre fenetrePilote dans les attributs

void MainWindow::addfenetre(fenetrePilote *f1)
{
    this->f1 = f1;
}

//Mets à jour l'interface graphique

void MainWindow::updateFenetre(bool premierAppel) // hedhi wa9telu bech t3awed sumilation wala fama 7eja tbadlet
{
    updateReservoir();
    updatePompe();
    updateVanne();
    updateMoteur(); // fais l'update du moteur et des diodes

    if(premierAppel) // Si c'est un bouton de cette fenetre qui a été cliqué
        this->f1->updateFenetre(false);
}

// Met à jour les diodes et la couleur des rectangles des moteurs

void MainWindow::updateMoteur()
{

    this->ui->M11->setPixmap(this->a->getCouleurAlimMoteur(0,0));
    this->ui->M12->setPixmap(this->a->getCouleurAlimMoteur(1,0));
    this->ui->M13->setPixmap(this->a->getCouleurAlimMoteur(2,0));

    this->ui->M21->setPixmap(this->a->getCouleurAlimMoteur(0,1));
    this->ui->M22->setPixmap(this->a->getCouleurAlimMoteur(1,1));
    this->ui->M23->setPixmap(this->a->getCouleurAlimMoteur(2,1));

    this->ui->M31->setPixmap(this->a->getCouleurAlimMoteur(0,2));
    this->ui->M32->setPixmap(this->a->getCouleurAlimMoteur(1,2));
    this->ui->M33->setPixmap(this->a->getCouleurAlimMoteur(2,2));

    this->ui->F7->setPalette(this->a->isAlimente(0));
    this->ui->F8->setPalette(this->a->isAlimente(1));
    this->ui->F9->setPalette(this->a->isAlimente(2));

}

// Mise à jour graphique des vannes

void MainWindow::updateVanne() // les updates lkol marboutin updatefenetre
{

    this->ui->VT122->setValue(this->a->V[0].getprog());
    this->ui->VT233->setValue(this->a->V[1].getprog());
    this->ui->V12->setPixmap(this->a->V[2].getpixmap());
    this->ui->V13->setPixmap(this->a->V[3].getpixmap());
    this->ui->V23->setPixmap(this->a->V[4].getpixmap());
}

// Mise à jour graphique des pompes

void MainWindow::updatePompe()
{
    this->ui->P11->setEnabled(this->a->R[0].getPompe1().getEnFonction());
    this->ui->P12->setEnabled(this->a->R[0].getPompe2().getEnFonction());

    this->ui->P21->setEnabled(this->a->R[1].getPompe1().getEnFonction());
    this->ui->P22->setEnabled(this->a->R[1].getPompe2().getEnFonction());

    this->ui->P31->setEnabled(this->a->R[2].getPompe1().getEnFonction());
    this->ui->P32->setEnabled(this->a->R[2].getPompe2().getEnFonction());
}

void MainWindow::updateReservoir()
{
    this->ui->R1->setEnabled(!this->a->R[0].getVidange());
    this->ui->F1->setPalette(this->a->R[0].getCouleur());

    this->ui->R2->setEnabled(!this->a->R[1].getVidange());
    this->ui->F2->setPalette(this->a->R[1].getCouleur());

    this->ui->R3->setEnabled(!this->a->R[2].getVidange());
    this->ui->F3->setPalette(this->a->R[2].getCouleur());
}

// Sauvegarde le score et arrête le timer des pannes si il est lancé

void MainWindow::finSimulation()
{
    if(this->timer->isActive())
        timer->stop();
    saveScore();
}

//Sauvegarde le score du joueur si il a résolu plus de 2 pannes et que
//l'avion termine dans un etat correct ou irréparable

void MainWindow::saveScore()
{
    if(this->getNombreDeChrono() < 2){
        QString str("Nombre de pannes reparees insuffisant, score non sauvegarde !");
        QMessageBox::warning(this,"Score", str);

    }else if(a->actionNecessaire()){
        QString str("La simulation n'est pas finie, vous n'avez donc pas de score!");
        QMessageBox::information(this,"Score", str);

    }else{
        int score = 0;
        score = this->getTempsTotalEcoule()/this->getNombreDeChrono();
        int stock = 0;

        if(score <= 3)
        {
            QMessageBox::information(this,"Score", "Votre score est de 10/10 !");
            stock = 10;
        }
        else if (score < 13)
        {
            QString str("Votre score est de ");
            str += QString::number(13 - score) ;
            str += "/10 !";
            QMessageBox::information(this,"Score", str);
            stock = 13 - score;
        }
        else
        {
            QMessageBox::information(this,"Score", "Votre score est de 0/10 !");
            stock = 0;
        }

        QSqlQuery rqt;
        QString usrId;
        if(rqt.exec("SELECT UsrId FROM User Where Nom='" + this->nom + "'"))
        {
            rqt.next();
            usrId.setNum(rqt.value(0).toInt(), 10);
        }
        QString Sstock;
        Sstock.setNum(stock);
        rqt.exec("INSERT INTO Score (Score, IdUser) VALUES('"+ Sstock +"', '"+ usrId +"')");
    }
}

//Renvoi le chrono

time_t MainWindow :: getChrono(){
    return chrono;
}

//Modifie le chrono

void MainWindow :: setChrono(int temps){
    chrono = temps;
}

//Lance un chrono et incrémente le nombre de chrono lancé

void MainWindow :: demarrerChrono(){
    time(&chrono);
    nombreDeChrono++;
}

//Renvoi le temps total écoulé pour résoudre les pannes

int MainWindow :: getTempsTotalEcoule(){
    return tempsTotalEcoule;
}

//Renvoi le nombre de chrono lancé

int MainWindow :: getNombreDeChrono(){
    return nombreDeChrono;
}

//Décrément le nombre de chrono lancé

void MainWindow :: decrementerNombreDeChrono(){
    nombreDeChrono--;
}

//Mofidie le temps total écoulé

void MainWindow :: setTempsTotalEcoule (int temps){
    tempsTotalEcoule = temps;
}

//Ferme les deux fenetres et arrête la simulation

void MainWindow::closeEvent(QCloseEvent *)
{
    this->f1->close();
    this->close();
    finSimulation();
}

// Lancement de la simulation, déblocage des commandes

void MainWindow::on_actionLancer_Simulation_triggered()  // hedhi wa9teli tros 3ala lancer_simulation
{
    this->ui->centralwidget->setEnabled(true); // centralewidget houwa le rest du panneau sauf le menu bar
    this->f1->centralWidget()->setEnabled(true); // fenetre pilote
}

//arrêt de la simulation, blocage des commandes

void MainWindow::on_actionStopper_simulation_triggered() // hedi ki tros stop
{
    this->ui->centralwidget->setEnabled(false);
    this->f1->centralWidget()->setEnabled(false);
    this->timer->stop();
}

// Réinitialisation de la simulation, sauvegarde du score

void MainWindow::on_actionR_initialiser_Simulation_triggered()
{
    finSimulation();
    this->a->reset();
    this->updateFenetre();
    this->chrono = 0;
    this->tempsTotalEcoule = 0;
    this->nombreDeChrono = 0;
}

// Appel de la fenetre Identification pour changer d'utilisateur

void MainWindow::on_actionChanger_Utilisateur_triggered()
{
    finSimulation();
    Identification* fenetre = new Identification(this->a, this->BDD, this);
    fenetre->show();
}

// Affichage de l'aide

void MainWindow::on_actionAfficher_Aide_triggered() // hedhi mta3 l aide
{
    afficherTexte* fenetre = new afficherTexte(1, this->BDD,this->nom, this);
    fenetre->show();
}

//Suppression de l'historique de l'utilisateur en cours

void MainWindow::on_actionSupprimer_historique_triggered()
{
    QSqlQuery rqt;
    QString usrId;

    if(rqt.exec("SELECT UsrId FROM User Where Nom='" + this->nom + "'"))
    {
        rqt.next();
        usrId.setNum(rqt.value(0).toInt(), 10);
    }
    if(rqt.exec("DELETE FROM Score Where IdUser='" + usrId + "'"))
        QMessageBox::warning(this, "Reussite !", "Suppression effectuee !");
}

// Affichage de l'historique de l'utilisateur

void MainWindow::on_actionAfficher_historique_triggered()
{
    afficherTexte* fenetre = new afficherTexte(2, this->BDD,this->nom, this);
    fenetre->show();
}

// Lance le timer utilisé pour les pannes automatiques

void MainWindow::on_actionPannes_automatiques_triggered()
{
    qsrand(time(NULL));
    int alea = (10 + (qrand() % 11)) * 1000;
    this->timer->start(alea);
}

// Met en panne la pompe P11
//On vérifie donc si la pompe secondaire de ce réservoir est allumée, si elle l'est on change le moteur qu'elle alimente
//Car une pompe secondaire alimente en priorité son moteur.
//Si la pompe secondaire n'est pas allumée, alors on regarde si une autre pompe secondaire a la possibilité d'alimenter ce moteur

void MainWindow::panneP11()
{
    if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getRempli()) // si pompe sec en marche et la reservoir est rempli
    {
        int i;
        int moteurAlimente = 0;
        for(i = 1 ; i < 3 ; i++) // nekhdou l moteur eli t5adam fih l pompe sec
            if(a->moteur[0][i] == 1)
                moteurAlimente = i;
        if(moteurAlimente != 0)
        {
            a->moteur[0][moteurAlimente] = 0; // ena7iw liaisson
            switch (moteurAlimente) {
                // 1 - moteur 2       2 - moteur 3
                case 1:
                    //On regarde si une autre pompe secondaire peut alimenter le moteur que la pompe 2 alimentait avant ca c fait automatiquement sans action d'apres le pilote
                //mech l pompe mta3 l panne w mech l pompe teb3a el reservoir mta3 l moteur donc 93adlet choix 1
                    if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->R[2].getRempli() && !a->V[4].getOuvert())
                        a->moteur[2][1] = 1;
                    break;

                case 2:
                    if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1 && a->R[1].getRempli() && !a->V[4].getOuvert())
                        a->moteur[1][2] = 1;
                    break;
            }
        }
    }else if(a->R[0].getPompe1().getEtat() == 1 && a->R[0].getPompe2().getEtat() != 1) // hedi tsir ki l pompe sec mech en marche
    {
        a->moteur[0][0] = 0;
        if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->R[2].getRempli() && !a->V[3].getOuvert() && a->moteur[2][1] != 1)
            a->moteur[2][0] = 1;
        else if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1 && a->R[1].getRempli() && !a->V[2].getOuvert() && a->moteur[1][2] != 1)
            a->moteur[1][0] = 1;

    }







    // hedha dima ysir
    a->R[0].getPompe1().panne();
    updateFenetre();
    //Si l'avion ne peut plus fonctionner alors on ne compte pas le dernier chrono et on réinitialise la simulation
    if(!a->peutFonctionner())
    {
        decrementerNombreDeChrono();
        chrono = 0;
        on_actionR_initialiser_Simulation_triggered();
    }
    else if(a->actionNecessaire() && (int)chrono == 0)
        demarrerChrono();
    else if(!a->actionNecessaire() && (int)chrono != 0){
        time_t tmp;
        time(&tmp);
        if((int)(tmp - chrono) > 2)
            tempsTotalEcoule = tempsTotalEcoule + (int)(tmp - chrono);
        else
            decrementerNombreDeChrono();
        chrono = 0;
    }

}















/*void MainWindow::PannePrin(int x)
{
    if(a->R[x].getPompe2().getEtat() == 1 && a->R[x].getRempli()) // si pompe sec en marche et la reservoir est rempli
    {
        int i;

        int moteurAlimente = 0;
        for(i = 1 ; i < 3 ; i++) // nekhdou l moteur eli t5adam fih l pompe sec
            if(a->moteur[0][i] == 1)
                moteurAlimente = i;
        if(moteurAlimente != 0)
        {
            a->moteur[x][moteurAlimente] = 0; // ena7iw liaisson
                    int yy=0;
                    int xx=0;
            switch (moteurAlimente) {
                // 1 - moteur 2       2 - moteur 3


                switch (x)
                {
                 case 0 : xx=1 ; yy=2;
                 case 1 : xx=0 ; yy=2;
                 case 2 : xx=0 ; yy=1;



                }

                case xx: //1
                    //On regarde si une autre pompe secondaire peut alimenter le moteur que la pompe 2 alimentait avant ca c fait automatiquement sans action d'apres le pilote
                //mech l pompe mta3 l panne w mech l pompe teb3a el reservoir mta3 l moteur donc 93adlet choix 1
                    if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->R[2].getRempli() && !a->V[4-x].getOuvert())
                        a->moteur[2][xx] = 1;
                    break;

                case yy:  //2
                    if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1 && a->R[1].getRempli() && !a->V[4-x].getOuvert())
                        a->moteur[1][yy] = 1;
                    break;
            }
        }
    }else if(a->R[x].getPompe1().getEtat() == 1 && a->R[x].getPompe2().getEtat() != 1) // hedi tsir ki l pompe sec mech en marche
    {
        a->moteur[x][x] = 0;
        if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->R[2].getRempli() && !a->V[3].getOuvert() && a->moteur[2][1] != 1)
            a->moteur[2][x] = 1;
        else if(a->R[x].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1 && a->R[x].getRempli() && !a->V[2].getOuvert() && a->moteur[1][2] != 1)
            a->moteur[1][x] = 1;

    }







    // hedha dima ysir
    a->R[0].getPompe1().panne();
    updateFenetre();
    //Si l'avion ne peut plus fonctionner alors on ne compte pas le dernier chrono et on réinitialise la simulation
    if(!a->peutFonctionner())
    {
        decrementerNombreDeChrono();
        chrono = 0;
        on_actionR_initialiser_Simulation_triggered();
    }
    else if(a->actionNecessaire() && (int)chrono == 0)
        demarrerChrono();
    else if(!a->actionNecessaire() && (int)chrono != 0){
        time_t tmp;
        time(&tmp);
        if((int)(tmp - chrono) > 2)
            tempsTotalEcoule = tempsTotalEcoule + (int)(tmp - chrono);
        else
            decrementerNombreDeChrono();
        chrono = 0;
    }

}


*/













































// Met en panne la pompe P12
//On vérifie si elle était en marche et si oui on regarde ce qu'elle alimentait et si une autre pompe secondaire peut alimenter
//ce qu'elle alimentait
//sinon on la met juste en panne

void MainWindow::panneP12()
{
    if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getRempli())
    {
        int moteurAlimente = 0;
        for(int i = 0 ; i < 3 ; i++)
            if(a->moteur[0][i] == 1)
                moteurAlimente = i;
        if(moteurAlimente == 0)
        {
            if(a->R[0].getPompe1().getEtat() != 1)
            {
                a->moteur[0][0] = 0;
                if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->R[2].getRempli() && !a->V[3].getOuvert() && a->moteur[2][1] != 1)
                    a->moteur[2][0] = 1;
                else if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1 && a->R[1].getRempli() && !a->V[2].getOuvert() && a->moteur[1][2] != 1)
                    a->moteur[1][0] = 1;
            }
        }else if(moteurAlimente == 1)
        {
            a->moteur[0][moteurAlimente] = 0;
            if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->R[2].getRempli() && !a->V[4].getOuvert())
                a->moteur[2][1] = 1;
        }else{
            a->moteur[0][moteurAlimente] = 0;
            if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1 && a->R[1].getRempli() && !a->V[4].getOuvert())
                a->moteur[1][2] = 1;
        }
    }

  this->a->R[0].getPompe2().panne();
    updateFenetre();
    if(!a->peutFonctionner())
    {
        decrementerNombreDeChrono();
        chrono = 0;
        on_actionR_initialiser_Simulation_triggered();
    }
    else if(a->actionNecessaire() && (int)chrono == 0)
        demarrerChrono();
    else if(!a->actionNecessaire() && (int)chrono != 0){
        time_t tmp;
        time(&tmp);
        if((int)(tmp - chrono) > 2)
            tempsTotalEcoule = tempsTotalEcoule + (int)(tmp - chrono);
        else
            decrementerNombreDeChrono();
        chrono = 0;
    }
}

// Met en panne la pompe P21
// Fonctionne comme panneP11

void MainWindow::panneP21()
{
    if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getRempli())
    {
        int i;
        int moteurAlimente = -1;
        for(i = 0 ; i < 3 ; i++)
            if(a->moteur[1][i] == 1 && i != 1)
                moteurAlimente = i;
        if(moteurAlimente != -1)
        {
            a->moteur[1][moteurAlimente] = 0;
            switch (moteurAlimente) {

                case 0:
                    if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->R[2].getRempli() && !a->V[3].getOuvert())
                        a->moteur[2][0] = 1;
                    break;

                case 2:
                    if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getPompe1().getEtat() == 1 && a->R[0].getRempli() && !a->V[3].getOuvert())
                        a->moteur[0][2] = 1;
                    break;
            }
        }
    }else if(a->R[1].getPompe1().getEtat() == 1 && a->R[1].getPompe2().getEtat() != 1)
    {
        a->moteur[1][1] = 0;
        if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->R[2].getRempli() && !a->V[4].getOuvert() && a->moteur[2][0] != 1)
            a->moteur[2][1] = 1;
        else if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getPompe1().getEtat() == 1 && a->R[0].getRempli() && !a->V[2].getOuvert() && a->moteur[0][2] != 1)
            a->moteur[0][1] = 1;

    }
    a->R[1].getPompe1().panne();
    updateFenetre();
    if(!a->peutFonctionner())
    {
        decrementerNombreDeChrono();
        chrono = 0;
        on_actionR_initialiser_Simulation_triggered();
    }
    else if(a->actionNecessaire() && (int)chrono == 0)
        demarrerChrono();
    else if(!a->actionNecessaire() && (int)chrono != 0){
        time_t tmp;
        time(&tmp);
        if((int)(tmp - chrono) > 2)
            tempsTotalEcoule = tempsTotalEcoule + (int)(tmp - chrono);
        else
            decrementerNombreDeChrono();
        chrono = 0;
    }
}

// Met en panne la pompe P22
// Fonctionne comme panneP12

void MainWindow::panneP22()
{
    if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getRempli())
    {
        int i;
        int moteurAlimente = 1;
        for(i = 0 ; i < 3 ; i++)
            if(a->moteur[1][i] == 1 && i != 1)
                moteurAlimente = i;
        if(moteurAlimente == 1)
        {
            if(a->R[1].getPompe1().getEtat() != 1)
            {
                a->moteur[1][1] = 0;
                if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->R[2].getRempli() && !a->V[4].getOuvert() && a->moteur[2][0] != 1)
                    a->moteur[2][1] = 1;
                else if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getPompe1().getEtat() == 1 && a->R[0].getRempli() && !a->V[2].getOuvert() && a->moteur[0][2] != 1)
                    a->moteur[0][1] = 1;
            }
        }else if(moteurAlimente == 0){
            a->moteur[1][moteurAlimente] = 0;
            if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->R[2].getRempli() && !a->V[3].getOuvert())
                a->moteur[2][0] = 1;
        }else{
            a->moteur[1][moteurAlimente] = 0;
            if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getPompe1().getEtat() == 1 && a->R[0].getRempli() && !a->V[3].getOuvert())
                a->moteur[0][2] = 1;
        }

    }
    a->R[1].getPompe2().panne();
    updateFenetre();
    if(!a->peutFonctionner())
    {
        decrementerNombreDeChrono();
        chrono = 0;
        on_actionR_initialiser_Simulation_triggered();
    }
    else if(a->actionNecessaire() && (int)chrono == 0)
        demarrerChrono();
    else if(!a->actionNecessaire() && (int)chrono != 0){
        time_t tmp;
        time(&tmp);
        if((int)(tmp - chrono) > 2)
            tempsTotalEcoule = tempsTotalEcoule + (int)(tmp - chrono);
        else
            decrementerNombreDeChrono();
        chrono = 0;
    }
}

// Met en panne la pompe P31
// Fonctionne comme panneP11

void MainWindow::panneP31()
{
    if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getRempli())
    {
        int i;
        int moteurAlimente = -1;
        for(i = 0 ; i < 2 ; i++)
            if(a->moteur[2][i] == 1)
                moteurAlimente = i;
        if(moteurAlimente != -1)
        {
            a->moteur[2][i] = 0;
            switch (i) {

                case 0:
                    if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1 && a->R[1].getRempli() && !a->V[2].getOuvert())
                        a->moteur[1][0] = 1;
                    break;

                case 1:
                    if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getPompe1().getEtat() == 1 && a->R[0].getRempli() && !a->V[2].getOuvert())
                        a->moteur[0][1] = 1;
                    break;
            }
        }
    }else if(a->R[2].getPompe1().getEtat() == 1 && a->R[2].getPompe2().getEtat() != 1)
    {
        a->moteur[2][2] = 0;
        if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1 && a->R[1].getRempli() && !a->V[4].getOuvert() && a->moteur[1][0] != 1)
            a->moteur[1][2] = 1;
        else if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getPompe1().getEtat() == 1 && a->R[0].getRempli() && !a->V[3].getOuvert() && a->moteur[0][1] != 1)
            a->moteur[0][2] = 1;

    }
    a->R[2].getPompe1().panne();
    updateFenetre();
    if(!a->peutFonctionner())
    {
        decrementerNombreDeChrono();
        chrono = 0;
        on_actionR_initialiser_Simulation_triggered();
    }
    else if(a->actionNecessaire() && (int)chrono == 0)
        demarrerChrono();
    else if(!a->actionNecessaire() && (int)chrono != 0){
        time_t tmp;
        time(&tmp);
        if((int)(tmp - chrono) > 2)
            tempsTotalEcoule = tempsTotalEcoule + (int)(tmp - chrono);
        else
            decrementerNombreDeChrono();
        chrono = 0;
    }
}

// Met en panne la pompe P32
// Fonctionne comme panneP12

void MainWindow::panneP32()
{
    if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getRempli())
    {
        int moteurAlimente = 2;
        for(int i = 0 ; i < 2 ; i++)
            if(a->moteur[2][i] == 1)
                moteurAlimente = i;
        if(moteurAlimente == 2){
           if(a->R[2].getPompe1().getEtat() != 1)
           {
                a->moteur[2][2] = 0;
                if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1 && a->R[1].getRempli() && !a->V[4].getOuvert() && a->moteur[1][0] != 1)
                    a->moteur[1][2] = 1;
                else if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getPompe1().getEtat() == 1 && a->R[0].getRempli() && !a->V[3].getOuvert() && a->moteur[0][1] != 1)
                    a->moteur[0][2] = 1;
           }
        }else if(moteurAlimente == 0){
            a->moteur[2][moteurAlimente] = 0;
            if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1 && a->R[1].getRempli() && !a->V[2].getOuvert())
                a->moteur[1][0] = 1;
        }else{
            a->moteur[2][moteurAlimente] = 0;
            if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getPompe1().getEtat() == 1 && a->R[0].getRempli() && !a->V[2].getOuvert())
                a->moteur[0][1] = 1;
        }
    }
    a->R[2].getPompe2().panne();
    updateFenetre();
    if(!a->peutFonctionner())
    {
        decrementerNombreDeChrono();
        chrono = 0;
        on_actionR_initialiser_Simulation_triggered();
    }
    else if(a->actionNecessaire() && (int)chrono == 0)
        demarrerChrono();
    else if(!a->actionNecessaire() && (int)chrono != 0){
        time_t tmp;
        time(&tmp);
        if((int)(tmp - chrono) > 2)
            tempsTotalEcoule = tempsTotalEcoule + (int)(tmp - chrono);
        else
            decrementerNombreDeChrono();
        chrono = 0;
    }
}

// Vidange le réservoir R1
//On vérifie si le réservoir peut être alimenter par un voisin,
//Si oui on le vidange puis on remet rempli a true
//Sinon on met en arret sa ou ses pompes si elles étaient alimentées (pour qu'elles ne puissent plus alimenter des moteurs, car elles n'ont plus de carburant)
// puis on les remet en marche (car elles restent allumées même avec leur réservoir vide),  puis on vidange
//Ensuite on vérifie que ce réservoir la n'en alimentait pas un autre

void MainWindow::vidangeR1()
{
    if( !a->V[0].getOuvert() && a->R[1].getRempli() && a->nbReservoirVidange() != 3 && ( !a->R[1].getVidange() || (a->R[1].getVidange() && !a->V[1].getOuvert()) ) ){
        a->R[0].vidanger();
        a->R[0].setRempli(true);
    }else{
        if(a->R[0].getPompe1().getEtat() == 1 && a->R[0].getPompe2().getEtat() == 1){
            arretP11();
            arretP12();
            a->R[0].getPompe1().marche();
            a->R[0].getPompe2().marche();
        }else if(a->R[0].getPompe1().getEtat() == 1){
            arretP11();
            a->R[0].getPompe1().marche();
        }else if(a->R[0].getPompe2().getEtat() == 1){
            arretP12();
            a->R[0].getPompe2().marche();
        }
        a->R[0].vidanger();
        updateFenetre();
        if(!a->V[0].getOuvert() && a->R[1].getVidange() && a->R[1].getRempli())
            vidangeR2();
    }
    updateFenetre();
    if(!a->peutFonctionner())
    {
        decrementerNombreDeChrono();
        chrono = 0;
        on_actionR_initialiser_Simulation_triggered();
    }
    else if(a->actionNecessaire() && (int)chrono == 0)
        demarrerChrono();
    else if(!a->actionNecessaire() && (int)chrono != 0){
        time_t tmp;
        time(&tmp);
        if((int)(tmp - chrono) > 2)
            tempsTotalEcoule = tempsTotalEcoule + (int)(tmp - chrono);
        else
            decrementerNombreDeChrono();
        chrono = 0;
    }
}

// Vidange le réservoir R2
//idem que vidangeR1
//Sauf que le reservoir 2 peut alimenter ou être alimenté par les deux autres réservoirs (comme il est au milieu)

void MainWindow::vidangeR2()
{
    if( (!a->V[0].getOuvert() && a->R[0].getRempli() && !a->R[0].getVidange() ) || (!a->V[1].getOuvert() && a->R[2].getRempli() && !a->R[2].getVidange() ) )
    {
        a->R[1].vidanger();
        a->R[1].setRempli(true);
    }else{
        if(a->R[1].getPompe1().getEtat() == 1 && a->R[1].getPompe2().getEtat() == 1){
            arretP21();
            arretP22();
            a->R[1].getPompe1().marche();
            a->R[1].getPompe2().marche();
        }else if(a->R[1].getPompe1().getEtat() == 1){
            arretP21();
            a->R[1].getPompe1().marche();
        }else if(a->R[1].getPompe2().getEtat() == 1){
            arretP22();
            a->R[1].getPompe2().marche();
        }
        a->R[1].vidanger();
        updateFenetre();
        if(!a->V[0].getOuvert() && a->R[0].getRempli() && a->R[0].getVidange())
            vidangeR1();
        if(!a->V[1].getOuvert() && a->R[2].getRempli() && a->R[2].getVidange())
            vidangeR3();
    }
    updateFenetre();
    if(!a->peutFonctionner())
    {
        decrementerNombreDeChrono();
        chrono = 0;
        on_actionR_initialiser_Simulation_triggered();
    }
    else if(a->actionNecessaire() && (int)chrono == 0)
        demarrerChrono();
    else if(!a->actionNecessaire() && (int)chrono != 0){
        time_t tmp;
        time(&tmp);
        if((int)(tmp - chrono) > 2)
            tempsTotalEcoule = tempsTotalEcoule + (int)(tmp - chrono);
        else
            decrementerNombreDeChrono();
        chrono = 0;
    }
}

// Vidange le réservoir R3
//idem que vidangeR1

void MainWindow::vidangeR3()
{
    if( !a->V[1].getOuvert() && a->R[1].getRempli() && a->nbReservoirVidange() != 3 && ( !a->R[1].getVidange() || (a->R[1].getVidange() && !a->V[0].getOuvert()) ) ){
        a->R[2].vidanger();
        a->R[2].setRempli(true);
    }else{
        if(a->R[2].getPompe1().getEtat() == 1 && a->R[2].getPompe2().getEtat() == 1){
            arretP31();
            arretP32();
            a->R[2].getPompe1().marche();
            a->R[2].getPompe2().marche();
        }else if(a->R[2].getPompe1().getEtat() == 1){
            arretP31();
            a->R[2].getPompe1().marche();
        }else if(a->R[2].getPompe2().getEtat() == 1){
            arretP32();
            a->R[2].getPompe2().marche();
        }
        a->R[2].vidanger();
        updateFenetre();
        if(!a->V[1].getOuvert() && a->R[1].getVidange() && a->R[1].getRempli() )
            vidangeR2();
    }
    updateFenetre();
    if(!a->peutFonctionner())
    {
        decrementerNombreDeChrono();
        chrono = 0;
        on_actionR_initialiser_Simulation_triggered();
    }
    else if(a->actionNecessaire() && (int)chrono == 0)
        demarrerChrono();
    else if(!a->actionNecessaire() && (int)chrono != 0){
        time_t tmp;
        time(&tmp);
        if((int)(tmp - chrono) > 2)
            tempsTotalEcoule = tempsTotalEcoule + (int)(tmp - chrono);
        else
            decrementerNombreDeChrono();
        chrono = 0;
    }
}

// idem que panneP11 sauf qu'on ne vérifie pas que l'avion peut marcher (on le revérifiera dans la suite de vidange)
// cela nous évite de réinitialiser car les pompes sont considérer comme en panne
// même si cet état n'est que temporaire

void MainWindow::arretP11()
{
    if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getRempli())
    {
        int i;
        int moteurAlimente = 0;
        for(i = 1 ; i < 3 ; i++)
            if(a->moteur[0][i] == 1)
                moteurAlimente = i;
        if(moteurAlimente != 0)
        {
            a->moteur[0][moteurAlimente] = 0;
            switch (moteurAlimente) {

                case 1:
                    //On regarde si une autre pompe secondaire peut alimenter le moteur que la pompe 2 alimentait avant
                    if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->R[2].getRempli() && !a->V[4].getOuvert())
                        a->moteur[2][1] = 1;
                    break;

                case 2:
                    if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1 && a->R[1].getRempli() && !a->V[4].getOuvert())
                        a->moteur[1][2] = 1;
                    break;
            }
        }
    }else if(a->R[0].getPompe1().getEtat() == 1 && a->R[0].getPompe2().getEtat() != 1)
    {
        a->moteur[0][0] = 0;
        if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->R[2].getRempli() && !a->V[3].getOuvert() && a->moteur[2][1] != 1)
            a->moteur[2][0] = 1;
        else if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1 && a->R[1].getRempli() && !a->V[2].getOuvert() && a->moteur[1][2] != 1)
            a->moteur[1][0] = 1;

    }
    a->R[0].getPompe1().arret();
    updateFenetre();
}

// idem que panneP12 sauf qu'on ne vérifie pas que l'avion peut marcher (on le revérifiera dans la suite de vidange)
// cela nous évite de réinitialiser car les pompes sont considérées comme en panne
// même si cet état n'est que temporaire

void MainWindow::arretP12()
{
    if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getRempli())
    {
        int moteurAlimente = 0;
        for(int i = 0 ; i < 3 ; i++)
            if(a->moteur[0][i] == 1)
                moteurAlimente = i;
        if(moteurAlimente == 0)
        {
            if(a->R[0].getPompe1().getEtat() != 1)
            {
                a->moteur[0][0] = 0;
                if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->R[2].getRempli() && !a->V[3].getOuvert() && a->moteur[2][1] != 1)
                    a->moteur[2][0] = 1;
                else if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1 && a->R[1].getRempli() && !a->V[2].getOuvert() && a->moteur[1][2] != 1)
                    a->moteur[1][0] = 1;
            }
        }else if(moteurAlimente == 1)
        {
            a->moteur[0][moteurAlimente] = 0;
            if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->R[2].getRempli() && !a->V[4].getOuvert())
                a->moteur[2][1] = 1;
        }else{
            a->moteur[0][moteurAlimente] = 0;
            if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1 && a->R[1].getRempli() && !a->V[4].getOuvert())
                a->moteur[1][2] = 1;
        }
    }

    this->a->R[0].getPompe2().arret();
    updateFenetre();
}

// Met en arret la pompe P21
// Fonctionne comme arretP11

void MainWindow::arretP21()
{
    if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getRempli())
    {
        int i;
        int moteurAlimente = -1;
        for(i = 0 ; i < 3 ; i++)
            if(a->moteur[1][i] == 1 && i != 1)
                moteurAlimente = i;
        if(moteurAlimente != -1)
        {
            a->moteur[1][moteurAlimente] = 0;
            switch (moteurAlimente) {

                case 0:
                    if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->R[2].getRempli() && !a->V[3].getOuvert())
                        a->moteur[2][0] = 1;
                    break;

                case 2:
                    if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getPompe1().getEtat() == 1 && a->R[0].getRempli() && !a->V[3].getOuvert())
                        a->moteur[0][2] = 1;
                    break;
            }
        }
    }else if(a->R[1].getPompe1().getEtat() == 1 && a->R[1].getPompe2().getEtat() != 1)
    {
        a->moteur[1][1] = 0;
        if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->R[2].getRempli() && !a->V[4].getOuvert() && a->moteur[2][0] != 1)
            a->moteur[2][1] = 1;
        else if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getPompe1().getEtat() == 1 && a->R[0].getRempli() && !a->V[2].getOuvert() && a->moteur[0][2] != 1)
            a->moteur[0][1] = 1;

    }
    a->R[1].getPompe1().arret();
    updateFenetre();
}

// Met en arret la pompe P22
// Fonctionne comme arretP12

void MainWindow::arretP22()
{
    if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getRempli())
    {
        int i;
        int moteurAlimente = 1;
        for(i = 0 ; i < 3 ; i++)
            if(a->moteur[1][i] == 1 && i != 1)
                moteurAlimente = i;
        if(moteurAlimente == 1)
        {
            if(a->R[1].getPompe1().getEtat() != 1)
            {
                a->moteur[1][1] = 0;
                if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->R[2].getRempli() && !a->V[4].getOuvert() && a->moteur[2][0] != 1)
                    a->moteur[2][1] = 1;
                else if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getPompe1().getEtat() == 1 && a->R[0].getRempli() && !a->V[2].getOuvert() && a->moteur[0][2] != 1)
                    a->moteur[0][1] = 1;
            }
        }else if(moteurAlimente == 0){
            a->moteur[1][moteurAlimente] = 0;
            if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getPompe1().getEtat() == 1 && a->R[2].getRempli() && !a->V[3].getOuvert())
                a->moteur[2][0] = 1;
        }else{
            a->moteur[1][moteurAlimente] = 0;
            if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getPompe1().getEtat() == 1 && a->R[0].getRempli() && !a->V[3].getOuvert())
                a->moteur[0][2] = 1;
        }

    }
    a->R[1].getPompe2().arret();
    updateFenetre();
}

// Met en arret la pompe P31
// Fonctionne comme arretP11

void MainWindow::arretP31()
{
    if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getRempli())
    {
        int i;
        int moteurAlimente = -1;
        for(i = 0 ; i < 2 ; i++)
            if(a->moteur[2][i] == 1)
                moteurAlimente = i;
        if(moteurAlimente != -1)
        {
            a->moteur[2][i] = 0;
            switch (i) {

                case 0:
                    if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1 && a->R[1].getRempli() && !a->V[2].getOuvert())
                        a->moteur[1][0] = 1;
                    break;

                case 1:
                    if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getPompe1().getEtat() == 1 && a->R[0].getRempli() && !a->V[2].getOuvert())
                        a->moteur[0][1] = 1;
                    break;
            }
        }
    }else if(a->R[2].getPompe1().getEtat() == 1 && a->R[2].getPompe2().getEtat() != 1)
    {
        a->moteur[2][2] = 0;
        if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1 && a->R[1].getRempli() && !a->V[4].getOuvert() && a->moteur[1][0] != 1)
            a->moteur[1][2] = 1;
        else if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getPompe1().getEtat() == 1 && a->R[0].getRempli() && !a->V[3].getOuvert() && a->moteur[0][1] != 1)
            a->moteur[0][2] = 1;

    }
    a->R[2].getPompe1().arret();
    updateFenetre();
}

// Met en arrêt la pompe P32
// Fonctionne comme arretP12

void MainWindow::arretP32()
{
    if(a->R[2].getPompe2().getEtat() == 1 && a->R[2].getRempli())
    {
        int moteurAlimente = 2;
        for(int i = 0 ; i < 2 ; i++)
            if(a->moteur[2][i] == 1)
                moteurAlimente = i;
        if(moteurAlimente == 2){
           if(a->R[2].getPompe1().getEtat() != 1)
           {
                a->moteur[2][2] = 0;
                if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1
                        && a->R[1].getRempli() && !a->V[4].getOuvert() && a->moteur[1][0] != 1)
                    a->moteur[1][2] = 1;
                else if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getPompe1().getEtat() == 1 && a->R[0].getRempli() && !a->V[3].getOuvert() && a->moteur[0][1] != 1)
                    a->moteur[0][2] = 1;
           }
        }else if(moteurAlimente == 0){
            a->moteur[2][moteurAlimente] = 0;
            if(a->R[1].getPompe2().getEtat() == 1 && a->R[1].getPompe1().getEtat() == 1 && a->R[1].getRempli() && !a->V[2].getOuvert())
                a->moteur[1][0] = 1;
        }else{
            a->moteur[2][moteurAlimente] = 0;
            if(a->R[0].getPompe2().getEtat() == 1 && a->R[0].getPompe1().getEtat() == 1 && a->R[0].getRempli() && !a->V[2].getOuvert())
                a->moteur[0][1] = 1;
        }
    }
    a->R[2].getPompe2().arret();
    updateFenetre();
}

// Fait tomber aléatoirement en panne un élément de l'avion

void MainWindow::panneAlea()
{
    bool termine = false;

    do
    {
        switch(ordrePanneAlea[ordrePanneAlea[9]])
        {
        case 0:
            if(!this->a->R[0].getVidange())
            {
                termine = true;
                vidangeR1();
            }
            ordrePanneAlea[9]++;

            break;
        case 1:
            if(!this->a->R[1].getVidange())
            {
                termine = true;
                vidangeR2();
            }
            ordrePanneAlea[9]++;

            break;
        case 2:
            if(!this->a->R[2].getVidange())
            {
                termine = true;
                vidangeR3();
            }
            ordrePanneAlea[9]++;

            break;
        case 3:
        if(this->a->R[0].getPompe1().getEtat() != -1)
            {
                termine = true;
                panneP11();
            }
            ordrePanneAlea[9]++;

            break;
        case 4:
            if(this->a->R[0].getPompe2().getEtat() != -1)
            {
                termine = true;
                panneP12();
            }
            ordrePanneAlea[9]++;

            break;
        case 5:
            if(this->a->R[1].getPompe1().getEtat() != -1)
            {
                termine = true;
                panneP21();
            }
            ordrePanneAlea[9]++;

            break;
        case 6:
            if(this->a->R[1].getPompe2().getEtat() != -1)
            {
                termine = true;
                panneP22();
            }
            ordrePanneAlea[9]++;

            break;
        case 7:
            if(this->a->R[2].getPompe1().getEtat() != -1)
            {
                termine = true;
                panneP31();
            }
            ordrePanneAlea[9]++;

            break;
        case 8:
            if(this->a->R[2].getPompe2().getEtat() != -1)
            {
                termine = true;
                panneP32();
            }
            ordrePanneAlea[9]++;

            break;
        }
    } while(termine == false);

    int alea = (10 + (qrand() % 11)) * 1000;
    this->timer->setInterval(alea);
}
