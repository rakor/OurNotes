#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    datenbank = QSqlDatabase::addDatabase("QSQLITE");
    QSettings settings(PROGNAME, PROGNAME);
    if (settings.contains("Datenbankfile")){
        datenbankFile = settings.value("Datenbankfile").toString();
        datenbank.setDatabaseName(datenbankFile);
        if (!datenbank.open()){
            QMessageBox::critical(this, "Fehler", "Datenbank konnte nicht geöffnet werden.");
            exit(2);
        }
    } else {
        neueDatenbankOeffnen();
    }

    guiBauen();
    this->resize(800,500);
    textfeld->setFocus();
}

MainWindow::~MainWindow()
{
    datenbank.close();
}

void MainWindow::keyPressEvent(QKeyEvent *e){
    if (e->key() == Qt::Key_F5){
            refresh();
        }
    QMainWindow::keyPressEvent(e);
}

void MainWindow::refresh()
{
    tabelleFuellen();
    projekteFuellen();
    benutzerFuellen();
    comboboxesAufSystemuserAnpassen();
}

void MainWindow::comboboxesAufSystemuserAnpassen()
{
    QString username = QString(getenv(USERENVVAR));
    QSqlQuery qu;
    qu.clear();
    if (!qu.exec("SELECT ID, Letztes_Thema FROM Benutzer WHERE Systemnutzer='"+username+"'"))
        qDebug() << qu.lastError().text();
    if (qu.next()){
        bearbeiter->setCurrentIndex(bearbeiter->findData(qu.value(0)));
        projekte->setCurrentIndex(projekte->findData(qu.value(1)));
    }
    qu.finish();
}

void MainWindow::guiBauen()
{
    // MainMenu
    QAction* benutzerverwaltung     = new QAction("Benutzer", this);
    QAction* projektverwaltung      = new QAction("Projekte", this);
    QAction* datenbankWechseln      = new QAction("Datenbank wechseln", this);

    this->setWindowIcon(QIcon(":/Bilder/Bilder/OurNotes.png"));

    QMenuBar* menu = this->menuBar();
    QMenu* filemen = menu->addMenu("Datei");
    filemen->addAction(datenbankWechseln);
    QMenu* sub = menu->addMenu("Pflege");
    sub->addAction(benutzerverwaltung);
    sub->addAction(projektverwaltung);

    connect(benutzerverwaltung, &QAction::triggered, [this]{BenutzerPflege* b = new BenutzerPflege(this);
                                                            connect(b, &BenutzerPflege::aenderungen, this, &MainWindow::benutzerFuellen);});
    connect(projektverwaltung,  &QAction::triggered, [this]{ProjektPflege* p = new ProjektPflege(this);
                                                            connect(p, &ProjektPflege::aenderungen, this, &MainWindow::projekteFuellen);});
    connect(datenbankWechseln,  &QAction::triggered, this, &MainWindow::neueDatenbankOeffnen);


    QFrame* frame = new QFrame(this);
    this->setCentralWidget(frame);
    QVBoxLayout* vbox = new QVBoxLayout(frame);
    QHBoxLayout* hbox = new QHBoxLayout(frame);
    frame->setLayout(vbox);


    tabelle     = new QTableWidget(this);
    bearbeiter  = new QComboBox(this);
    projekte    = new QComboBox(this);
    textfeld    = new QLineEdit(this);
    okButton    = new QPushButton("Ok", this);
    vbox->addWidget(tabelle);
    vbox->addLayout(hbox);
    hbox->addWidget(projekte);
    hbox->addWidget(bearbeiter);
    hbox->addWidget(textfeld);
    hbox->addWidget(okButton);

    tabelle->setColumnCount(4);
    tabelle->setHorizontalHeaderLabels(QStringList() << "Datum" << "Projekt" << "Text" << "Benutzer");
    tabelle->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    tabelle->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    tabelle->setWordWrap(true);
    tabelleFuellen();

    // Projekte Fuellen
    projekteFuellen();
    projekte->setCurrentIndex(-1);

    // Benutzer Fuellen
    benutzerFuellen();

    comboboxesAufSystemuserAnpassen();

    connect(okButton,  &QPushButton::clicked,      this, &MainWindow::inDatenbankSchreiben);
    connect(textfeld,  &QLineEdit::returnPressed,  this, &MainWindow::inDatenbankSchreiben);
}

bool MainWindow::tabellenErstellen()
{
    bool allesOK{true};
    QSqlQuery qu;
    qu.clear();
    allesOK = allesOK && qu.exec("CREATE TABLE Themen (    ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                    " Name          TEXT, "
                                    " Erstellt_am   TEXT, "
                                    " Erstellt_von  INTEGER, "
                                    " Beschreibung  TEXT, "
                                    " Aktiv         INTEGER)");
    qu.finish();

    qu.clear();
    allesOK = allesOK && qu.exec("CREATE TABLE Benutzer(   ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                    " Name          TEXT, "
                                    " Systemnutzer  TEXT, "
                                    " Letztes_Thema INTEGER)");
    qu.finish();

    qu.clear();
    allesOK = allesOK && qu.exec("CREATE TABLE Eintraege(  ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                    " Eingetragen_von   INTEGER, "
                                    " Eingetragen_am    TEXT, "
                                    " Thema             INTEGER,"
                                    " Text              TEXT)");
    qu.finish();

    qu.clear();
    allesOK = allesOK && qu.exec("CREATE TABLE Status (    ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                    " Schluessel        TEXT,"
                                    " Wert              TEXT)");
    qu.finish();

    qu.clear();
    allesOK = allesOK && qu.exec("INSERT INTO Status ("
                                    " Schluessel, Wert"
                                    ") VALUES ("
                                    "'Datenbankschema', "
                                    "'" DATENBANKVERSION "'"
                                    ")");
    qu.finish();

    return allesOK;
}

bool MainWindow::neueDatenbankOeffnen()
{
    QSettings settings(PROGNAME, PROGNAME);
        datenbankFile = QFileDialog::getSaveFileName(this, "Datenbankfile",qApp->applicationDirPath() + "/" PROGNAME ".ond", "Datenbank (*.ond)", 0, QFileDialog::DontConfirmOverwrite);
    if (datenbankFile.isEmpty()) return false;
    settings.setValue("Datenbankfile", datenbankFile);

    bool neueDatenbank{false};
    if (!QFileInfo(datenbankFile).exists()){
        if (QMessageBox::question(this, "Datenbank nicht vorhanden",
                                  "Die Datenbank ist nicht vorhanden. Soll eine neue erstellt werden?") == QMessageBox::Yes){
            neueDatenbank = true;
        } else {
            return false;
        }
    }
    if (datenbank.isOpen()) datenbank.close();
    datenbank.setDatabaseName(datenbankFile);
    if (!datenbank.open()){
        QMessageBox::critical(this, "Fehler", "Datenbank konnte nicht geöffnet werden.");
        return false;
    }
    if (neueDatenbank) {
        if (!tabellenErstellen()) QMessageBox::critical(this, "Fehler", "Fehler beim Erzeugen der Datenbanktabellen!");
    }
    if (tabelle != nullptr && projekte != nullptr && bearbeiter != nullptr){
        refresh();
    }
    return true;
}

void MainWindow::inDatenbankSchreiben()
{
    if (projekte->currentIndex() < 0 || textfeld->text().isEmpty() || bearbeiter->currentIndex() < 0) return;
    QSqlQuery qu;
    qu.clear();
    if (!qu.exec("INSERT INTO Eintraege (Eingetragen_von, Eingetragen_am, Thema, Text) VALUES ("
            "'"+bearbeiter->itemData(bearbeiter->currentIndex()).toString()+"', "
            "'"+QDate::currentDate().toString("yyyyMMdd")+"', "
            "'"+projekte->itemData(projekte->currentIndex()).toString()+"', "
            "'"+textfeld->text().replace('\'',"\'\'")+"'"
            ")")) qDebug() << qu.lastError().text();
    qu.finish();
    qu.clear();
    if (!qu.exec("UPDATE Benutzer SET Letztes_Thema='"+projekte->itemData(projekte->currentIndex()).toString()+
                 "' WHERE ID='"+bearbeiter->itemData(bearbeiter->currentIndex()).toString()+"'"))
        qDebug() << qu.lastError().text();

    qu.finish();
    textfeld->clear();
    tabelleFuellen();
}

void MainWindow::tabelleFuellen()
{
    for (int j{tabelle->rowCount()-1}; j >= 0; j--){
        tabelle->removeRow(j);
    }
    QSqlQuery qu;
    qu.clear();
    if (!qu.exec("SELECT Eintraege.Text, Benutzer.Name, Themen.Name, Eintraege.Eingetragen_am, Themen.Beschreibung FROM Eintraege"
                 " JOIN Benutzer on Eingetragen_von=Benutzer.ID "
                 " JOIN Themen on Thema=Themen.ID ORDER BY Eingetragen_am ASC")) qDebug() << qu.lastError().text();
    int i{0};
    while (qu.next()){
        tabelle->setRowCount(tabelle->rowCount()+1);
        tabelle->setItem( i,   0, new QTableWidgetItem(qu.value(3).toString()));
        tabelle->setItem( i,   1, new QTableWidgetItem(qu.value(2).toString()));
        tabelle->item( i,1 )->setToolTip(qu.value(4).toString());
        tabelle->setItem( i,   2, new QTableWidgetItem(qu.value(0).toString()));
        tabelle->item( i,2 )->setToolTip(qu.value(0).toString());
        tabelle->setItem( i++, 3, new QTableWidgetItem(qu.value(1).toString()));
    }
    qu.finish();
    tabelle->scrollToBottom();
}

void MainWindow::projekteFuellen()
{
    projekte->clear();
    QSqlQuery qu;
    qu.clear();
    qu.exec("SELECT ID, Name FROM Themen WHERE Aktiv='1'");
    while (qu.next()){
        projekte->addItem(qu.value(1).toString(), qu.value(0));
    }
    qu.finish();
}

void MainWindow::benutzerFuellen()
{
    bearbeiter->clear();
    QSqlQuery qu;
    qu.clear();
    qu.exec("SELECT ID, Name FROM Benutzer");
    while (qu.next()){
        bearbeiter->addItem(qu.value(1).toString(), qu.value(0));
    }
    qu.finish();
}

