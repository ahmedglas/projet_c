using namespace std;
#include "identification.h"
#include "ui_identification.h"

/* Classe permettant la connexion ou l'inscription d'une personne auprès de l'application
  */

Identification::Identification(Avion *a, QSqlDatabase BDD, QWidget *parent) :
    QDialog(parent), ui(new Ui::Identification)
{
    this->parent = parent;
    this->BDD = BDD;
    ui->setupUi(this);
    QPixmap pic("C:/Users/ahmed/Desktop/finall/Simulation/Images/avion3.png");
    ui->label_pic->setPixmap(pic);
    ui->label_pic->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    //Chemin BDD
    QString path = QCoreApplication::applicationDirPath();
    path = "Simulation.sqlite";

    if(this->parent == 0)
    {
    //init BDD
    QFileInfo verifier_BDD(path);

        if(!verifier_BDD.isFile())
        {
            QMessageBox::warning(this, "Warning", "Base de donnée inexistante");
            this->close();
        }
        else
        {
            if(BDD.open()){}
            else
            {
                QMessageBox::warning(this, "Warning", "Base de donnée non chargée");
                this->close();
            }
        }
    }
    this->a = a;
}


Identification::~Identification()
{
    delete ui;
}

//Lors d'un clic sur OK
// Vérification si l'utilisateur est bien dans la base de donnée si "Inscription" n'est pas coché
// Création du compte si l'utilisateur a coché la case Inscription et que l'identifiant n'est pas déjà pris

void Identification::on_validation_accepted()
{
    bool etapeSuivante = false;
    //Inscription
    if(ui->inscription->isChecked())
    {
        if(!BDD.isOpen())
            QMessageBox::warning(this, "Echec de l'ouverture", "Pas de BDD");

        QSqlQuery rqt;

        rqt.exec("SELECT Nom FROM User Where Nom='" + ui->Nom->text() + "'");
        if(!rqt.next())
        {//Si il n'y a rien
            if(rqt.exec("INSERT INTO User (Nom, Mdp) VALUES('"+ ui->Nom->text() +"', '"+ ui->Mdp->text() +"')"))
            {
                QMessageBox::warning(this, "Réussite !", "Inscription reussie");
                etapeSuivante = true;
            }
            else
                QMessageBox::warning(this, "Echec !", "L'inscription a rate");
        }
        else
            QMessageBox::warning(this, "Echec !", "Nom déjà utilise !");


        //Rajouter l'user dans la base si il y est pas déjà

    }
    else
    {
        if(!BDD.isOpen())
            QMessageBox::warning(this, "Echec de l'ouverture", "Pas de BDD");
        QSqlQuery rqt;
        if(rqt.exec("SELECT Nom, Mdp FROM User WHERE Nom='" + ui->Nom->text() + "' AND Mdp='" + ui->Mdp->text() + "'"))
        {
            if(!rqt.next())
                QMessageBox::warning(this, "Echec !", "Mauvais Nom ou mot de passe");
            else
                etapeSuivante = true;

        }
        //Vérifier si l'user correspond
    }
    if(etapeSuivante)
    {
        MainWindow* fenetre1 = new MainWindow(this->a, ui->Nom->text(), this->BDD);
        fenetrePilote* fenetre2 = new fenetrePilote(this->a);
        fenetre1->addfenetre(fenetre2);
        fenetre2->addfenetre(fenetre1);
        fenetre2->show();
        fenetre1->show();

        this->close();
        if(this->parent != 0)
            this->parent->close();
    }



}

//Fermeture de la fenetre si l'utilisateur clique sur "cancel"

void Identification::on_validation_rejected()
{
    this->close();
}
