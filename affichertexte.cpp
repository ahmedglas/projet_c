#include "affichertexte.h"
#include "ui_affichertexte.h"

/*
  Classe permettant d'afficher une fenêtre avec du texte et un bouton de fermeture.
  Utilisée pour afficher les fenetres d'aide et d'affichage des scores.
*/

afficherTexte::afficherTexte(int aAfficher, QSqlDatabase BDD, QString nom, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::afficherTexte)
{
    ui->setupUi(this);
    if(!BDD.isOpen())
        qDebug() << "Test";
    if(aAfficher == 1) // AFFICHAGE DE L'AIDE
    {
        this->setWindowTitle("Aide");
        QString aEcrire = "les etats\n";
        aEcrire += "- rondVert : en fonctionnement/approvisione en carburant\n";
        aEcrire += "- rectangle : eteint/non-approvisione en carburant\n";
        aEcrire += "- cadreBleu : en panne\n";
        this->ui->texte->setText(aEcrire);
    }
    if(aAfficher == 2) // AFFICHAGE DES SCORES
    {

        this->setWindowTitle("Scores");
        QString aEcrire = "Dix derniers scores pour ", usrId;
        aEcrire += nom + " : \n";
        QSqlQuery rqt;
        if(rqt.exec("SELECT UsrId FROM User Where Nom='" + nom + "'"))
        { // Récupération de l'id utilisateur
            rqt.next();
            usrId.setNum(rqt.value(0).toInt(), 10);
        }

        rqt.exec("SELECT Score FROM Score WHERE IdUser='"+ usrId +"'");
        int max = 0, total = 0, nbScores = 0, temp = 0;

            while(rqt.next())
            { // parcours de tous les résultats
                temp = rqt.value(0).toInt();
                if( nbScores < 10)
                { // affichage des 10 derniers
                aEcrire += QString::number(nbScores) + ") ";
                aEcrire += QString::number(temp);
                aEcrire += "\n\n";
                }
                if(temp > max)
                    max = temp;
                total += temp;
                nbScores++;
            }
        //Affichage du max + la moyenne
        if(nbScores == 0)
            aEcrire += "Aucun score enregistre";
        else
        {
            aEcrire += "Meilleur score : " + QString::number(max) + " ! \n\n";
            aEcrire += "Moyenne des scores : " + QString::number((double) (total / nbScores)) +" ! \n";
        }

    this->ui->texte->setText(aEcrire);
    }

}

afficherTexte::~afficherTexte()
{
    delete ui;
}

void afficherTexte::on_pushButton_clicked()
{
    this->close();
}
