#include "avion.h"
#include "identification.h"
#include <QApplication>
#include <QWidget>
#include <QDebug>
#include <QFont>
using namespace std;

/*
 * Ouverture de l'application
 * Lancement d l'avion
 * Ouverture de la BDD
 * Ouverture de la fenêtre d'identification
 *
 */

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Avion* a = new Avion();

    QSqlDatabase BDD;
    QString path = QCoreApplication::applicationDirPath();
    qDebug() <<"hedhaa"+ path ;
    path = "Simulation.sqlite";
    BDD = QSqlDatabase::addDatabase("QSQLITE");
    BDD.setDatabaseName(path);
    qDebug() <<"hedhaa"+ path ;

    Identification fenetre3(a, BDD);
    fenetre3.setWindowTitle("Simulation Avion");
    QFont af ("Times",20,QFont::Bold ,QFont::StyleItalic  );
    fenetre3.setFont(af);
    fenetre3.show();

    return app.exec();
}
