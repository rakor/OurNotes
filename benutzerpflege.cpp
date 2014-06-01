#include "benutzerpflege.h"

BenutzerPflege::BenutzerPflege(QWidget *parent) :
    QDialog(parent)
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    guiBauen();
    this->show();
}

void BenutzerPflege::guiBauen()
{
    namenWaehler    = new QComboBox(this);
    name            = new QLineEdit(this);
    systemname      = new QLineEdit(this);
    okButton        = new QPushButton("Ok", this);
    abbrechenButton = new QPushButton("Abbrechen", this);

    QGridLayout* layout = new QGridLayout(this);
    this->setLayout(layout);
    layout->addWidget(new QLabel("Benutzer:",this),0,0);
    layout->addWidget(namenWaehler, 0, 1);
    layout->addWidget(new QLabel("Name:",this),1,0);
    layout->addWidget(name, 1, 1);
    layout->addWidget(new QLabel("Systemname:",this),2,0);
    layout->addWidget(systemname, 2, 1 );
    QHBoxLayout* hbox = new QHBoxLayout(this);
    layout->addLayout(hbox, 3, 0, 1, 2);
    hbox->addWidget(okButton);
    hbox->addWidget(abbrechenButton);

    // Benutzerauswahl fuellen
    namenWaehler->addItem("NEUER BENUTZER", QVariant(-1));
    QSqlQuery qu;
    qu.clear();
    if (!qu.exec("SELECT ID, Name FROM Benutzer ORDER BY Name ASC")) qDebug() << qu.lastError().text();
    while (qu.next()){
        namenWaehler->addItem(qu.value(1).toString(), qu.value(0));
    }
    qu.finish();

    systemname->setText(QString(getenv(USERENVVAR)));


    connect (namenWaehler,      SIGNAL(currentIndexChanged(int)),   this, SLOT(andererBenutzerGewaehlt(int)));
    connect (abbrechenButton,   &QPushButton::clicked,              this, &BenutzerPflege::close);
    connect (okButton,          &QPushButton::clicked,              this, &BenutzerPflege::schreibeInDatenbank);
}

void BenutzerPflege::andererBenutzerGewaehlt(int)
{
    if (namenWaehler->currentData().toInt() == -1){ // Neuen Benutzer anlegen
        name->clear();
        systemname->setText(QString(getenv(USERENVVAR)));
    } else {    // Benutzer aendern
        QSqlQuery qu;
        qu.clear();
        if (!qu.exec("SELECT Name, Systemnutzer FROM Benutzer WHERE ID='"+namenWaehler->currentData().toString()+"'"))
            qDebug() << qu.lastError().text();
        if (qu.next()){
            name->setText(qu.value(0).toString());
            systemname->setText(qu.value(1).toString());
        }
        qu.finish();
    }
}

void BenutzerPflege::schreibeInDatenbank()
{
    if (name->text().isEmpty()) return;
    if (namenWaehler->currentData().toInt() == -1){ // Neuer Datensatz
        QSqlQuery qu;
        qu.clear();
        if (!qu.exec("INSERT INTO Benutzer (Name, Systemnutzer, Letztes_Thema) VALUES ("
                     "'"+name->text().replace('\'',"\'\'")+"', "
                     "'"+systemname->text().replace('\'',"\'\'")+"', "
                     "'0'"
                     ")")) qDebug() << qu.lastError().text();
        qu.finish();
    } else {    // Datensatz aendern
        QSqlQuery qu;
        qu.clear();
        if (!qu.exec("UPDATE Benutzer SET "
                        "Name='"+name->text().replace('\'',"\'\'")+
                     "', Systemnutzer='"+systemname->text().replace('\'',"\'\'")+
                     "' WHERE ID='"+namenWaehler->currentData().toString()+"'"))
            qDebug() << qu.lastError().text();

        qu.finish();
    }
    emit aenderungen();
    this->close();
}
