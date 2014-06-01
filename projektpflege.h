#ifndef PROJEKTPFLEGE_H
#define PROJEKTPFLEGE_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDate>
#include <QDebug>
#include <QTextEdit>

#include "global.h"

class ProjektPflege : public QDialog
{
    Q_OBJECT
public:
    explicit ProjektPflege(QWidget *parent = 0);

protected:
    QPushButton*    okButton{nullptr};
    QPushButton*    abbrechenButton{nullptr};
    QComboBox*      bearbeiter{nullptr};
    QComboBox*      projektWaehler{nullptr};
    QLineEdit*      titel{nullptr};
    QTextEdit*      beschreibung{nullptr};
    QCheckBox*      aktiv{nullptr};

protected slots:
    void inDatenbankSchreiben();
    void gewaehltesProjektGeaendert(int);

signals:
    void    aenderungen();

public slots:

};

#endif // PROJEKTPFLEGE_H
