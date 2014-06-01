#ifndef BENUTZERPFLEGE_H
#define BENUTZERPFLEGE_H

#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QLineEdit>
#include <QGridLayout>
#include <QLabel>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>

#include "global.h"

class BenutzerPflege : public QDialog
{
    Q_OBJECT
public:
    explicit BenutzerPflege(QWidget *parent = 0);
    void     guiBauen();

protected:
    QComboBox*      namenWaehler{nullptr};
    QLineEdit*      name{nullptr};
    QLineEdit*      systemname{nullptr};
    QPushButton*    okButton{nullptr};
    QPushButton*    abbrechenButton{nullptr};

protected slots:
    void    andererBenutzerGewaehlt(int);
    void    schreibeInDatenbank();

signals:
    void    aenderungen();

public slots:

};

#endif // BENUTZERPFLEGE_H
