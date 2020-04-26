#ifndef AFFICHERTEXTE_H
#define AFFICHERTEXTE_H

#include <QDialog>
#include <QtSql>

namespace Ui {
class afficherTexte;
}

class afficherTexte : public QDialog
{
    Q_OBJECT
    
public:
    explicit afficherTexte(int aAfficher, QSqlDatabase BDD, QString nom, QWidget *parent = 0);
    ~afficherTexte();
    
private slots:
    void on_pushButton_clicked();

private:
    Ui::afficherTexte *ui;
};

#endif // AFFICHERTEXTE_H
