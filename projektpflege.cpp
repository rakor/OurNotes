#include "projektpflege.h"

ProjektPflege::ProjektPflege(QWidget *parent) :
    QDialog(parent)
{
    this->setAttribute(Qt::WA_DeleteOnClose);

    QGridLayout* layout = new QGridLayout(this);
    projektWaehler      = new QComboBox(this);
    titel               = new QLineEdit(this);
    beschreibung        = new QTextEdit(this);
    aktiv               = new QCheckBox(this);
    bearbeiter          = new QComboBox(this);
    okButton            = new QPushButton("Ok", this);
    abbrechenButton     = new QPushButton("Abbrechen", this);

    layout->addWidget(new QLabel("Projekt:", this), 0, 0);
    layout->addWidget(projektWaehler, 0, 1);
    layout->addWidget(new QLabel("Titel:", this), 1, 0);
    layout->addWidget(titel, 1, 1);
    layout->addWidget(new QLabel("Beschreibung:", this), 2, 0);
    layout->addWidget(beschreibung, 2, 1);
    layout->addWidget(new QLabel("Aktiv:", this), 3, 0);
    layout->addWidget(aktiv, 3, 1);
    layout->addWidget(new QLabel("Bearbeiter:", this), 4, 0);
    layout->addWidget(bearbeiter, 4, 1);
    QHBoxLayout* buttonLayout = new QHBoxLayout(this);
    layout->addLayout(buttonLayout,5,0,1,2);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(abbrechenButton);

    // Projekte Fuellen
    projektWaehler->addItem("Neuer Eintrag", QVariant(-1));
    QSqlQuery qu;
    qu.clear();
    qu.exec("SELECT ID, Name FROM Themen ORDER BY Name ASC");
    while (qu.next()){
        projektWaehler->addItem(qu.value(1).toString(), qu.value(0));
    }
    qu.finish();

    // Bearbeiter fuellen
    qu.clear();
    qu.exec("SELECT ID, Name FROM Benutzer ORDER BY Name ASC");
    while (qu.next()){
        bearbeiter->addItem(qu.value(1).toString(), qu.value(0));
    }
    bearbeiter->setCurrentIndex(-1);
    qu.finish();

    connect( okButton,          &QPushButton::clicked,              this, &ProjektPflege::inDatenbankSchreiben);
    connect( abbrechenButton,   &QPushButton::clicked,              this, &ProjektPflege::close);
    connect( projektWaehler,    SIGNAL(currentIndexChanged(int)),   this, SLOT(gewaehltesProjektGeaendert(int)));
    this->show();
}

void ProjektPflege::inDatenbankSchreiben()
{
    if (projektWaehler->currentData().toInt() == -1){ // create new entry
        QSqlQuery qu;
        qu.clear();
        if (!qu.exec("INSERT INTO Themen (Name, Erstellt_am, Erstellt_von, Beschreibung, Aktiv) VALUES ("
                "'"+titel->text()+"', "
                "'"+QDate::currentDate().toString("yyyyMMdd")+"', "
                "'"+bearbeiter->currentData().toString()+"', "
                "'"+beschreibung->toPlainText().replace('\'',"\'\'")+"', "
                "'"+QString((aktiv->isChecked() ? "1" : "0"))+"'"
                ")")) qDebug() << qu.lastError().text();
        qu.finish();
    } else { // Edit existing entry
        QSqlQuery qu;
        qu.clear();
        if (!qu.exec("UPDATE Themen SET Name='"+titel->text()+"', Erstellt_von='"+bearbeiter->currentData().toString()+"', Beschreibung='"+beschreibung->toPlainText().replace('\'',"\'\'")+"', Aktiv='"+QString(aktiv->isChecked() ? "1" : "0")+"'"
                     " WHERE ID='"+projektWaehler->currentData().toString()+"'")) qDebug() << qu.lastError().text();
        qu.finish();
    }
    emit aenderungen();
    this->close();
}

void ProjektPflege::gewaehltesProjektGeaendert(int)
{
    if (projektWaehler->currentData().toInt() == -1){
        bearbeiter->setCurrentIndex(-1);
        aktiv->setChecked(false);
        titel->clear();
        beschreibung->clear();
    }
    QSqlQuery qu;
    qu.clear();
    if (!qu.exec("SELECT NAME, Erstellt_von, Aktiv, Beschreibung FROM Themen WHERE ID='"+projektWaehler->currentData().toString()+"'"))
        qDebug() << qu.lastError().text();
    if (qu.next()){
        bearbeiter->setCurrentIndex(bearbeiter->findData(qu.value(1)));
        aktiv->setChecked(qu.value(2).toInt() == 1 ? true : false);
        titel->setText(qu.value(0).toString());
        beschreibung->setText(qu.value(3).toString());
    }
    qu.finish();
}
