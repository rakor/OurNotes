#include "mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    datenbank = QSqlDatabase::addDatabase("QSQLITE");

    if (!datenbankOeffnen(1)){
        if(!datenbankOeffnen(2)){
            do {
                if (!neueDatenbankdateiSetzen(1)){
                    QMessageBox::critical(this, "Fehler", "Datenbank konnte nicht geöffnet werden.");
                    exit(0);
                }
            }while (!datenbankOeffnen(1));
        }
    }

    guiBauen();
    this->resize(800,500);
    textfeld->setFocus();

    this->show();
    tabelle->resizeRowsToContents();
}

MainWindow::~MainWindow()
{
    datenbank.close();
}

void MainWindow::keyPressEvent(QKeyEvent *e){
    if (e->key() == Qt::Key_F5){
            refresh();
        }
    if (e->key() == Qt::Key_Escape){
            textfeld->clear();
            suchresetButtoGeklickt();
    }
    QMainWindow::keyPressEvent(e);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    tabelleFuellen();
    QMainWindow::resizeEvent(e);
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

QTableWidgetItem *MainWindow::neuesTableItem(QString text, int id, bool tooltip, QString tooltiptext)
{
    QTableWidgetItem* item{nullptr};
    item = new QTableWidgetItem(text);
    if (gefilter) item->setBackgroundColor(Qt::cyan);
    item->setTextAlignment(Qt::AlignTop|Qt::AlignLeft);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    if (tooltip) item->setToolTip(tooltiptext.isEmpty() ? text : tooltiptext);
    if (id != 0) item->setData(Qt::UserRole, id);
    return item;
}

void MainWindow::guiBauen()
{
    // MainMenu
    QAction* benutzerverwaltung     = new QAction("Benutzer", this);
    QAction* projektverwaltung      = new QAction("Projekte", this);
    QAction* datenbank1Wechseln     = new QAction("Datenbankdatei setzen DB1", this);
    QAction* datenbank2Wechseln     = new QAction("Datenbankdatei setzen DB2", this);

    this->setWindowIcon(QIcon(":/Bilder/Bilder/OurNotes.png"));

    QMenuBar* menu = this->menuBar();
    QMenu* filemen = menu->addMenu("Datei");
    filemen->addAction(datenbank1Wechseln);
    filemen->addAction(datenbank2Wechseln);
    QMenu* sub = menu->addMenu("Pflege");
    sub->addAction(benutzerverwaltung);
    sub->addAction(projektverwaltung);
    menu->addSeparator();
    QMenu* ueber = menu->addMenu("Ueber");
    QAction* aboutQt = new QAction("Über Qt", this);
    ueber->addAction(aboutQt);

    connect(benutzerverwaltung, &QAction::triggered, [this]{BenutzerPflege* b = new BenutzerPflege(this);
                                                            connect(b, &BenutzerPflege::aenderungen, this, &MainWindow::benutzerFuellen);});
    connect(projektverwaltung,  &QAction::triggered, [this]{ProjektPflege* p = new ProjektPflege(this);
                                                           connect(p, &ProjektPflege::aenderungen, this, &MainWindow::projekteFuellen);});
    connect(datenbank1Wechseln,  &QAction::triggered, [this]{neueDatenbankdateiSetzen(1); inDatenbankWechseln(1);});
    connect(datenbank2Wechseln,  &QAction::triggered, [this]{neueDatenbankdateiSetzen(2); inDatenbankWechseln(2);});
    connect(aboutQt, &QAction::triggered, [this]{QMessageBox::aboutQt(this);});


    // Toolbar bauen
    QToolBar*   toolbar     = new QToolBar("Datenbankauswahl", this);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    zuDB1Wechseln = new QAction("Datenbank 1", this);
    zuDB2Wechseln = new QAction("Datenbank 2", this);
    toolbar->addAction(zuDB1Wechseln);
    toolbar->addAction(zuDB2Wechseln);
    this->addToolBar(toolbar);
    connect(zuDB1Wechseln, &QAction::triggered, [this]{inDatenbankWechseln(1);});
    connect(zuDB2Wechseln, &QAction::triggered, [this]{inDatenbankWechseln(2);});
    zuDB1Wechseln->setEnabled(aktuelleDatenbank == 1 ? false : true);
    QSettings settings(PROGNAME, PROGNAME);
    if (!settings.contains("Datenbank2") || aktuelleDatenbank == 2){
        zuDB2Wechseln->setEnabled(false);
    }


    // Generierung des Fensterinhalts
    QFrame* frame = new QFrame(this);
    this->setCentralWidget(frame);
    QVBoxLayout* vbox   = new QVBoxLayout(frame);
    QHBoxLayout* hbox   = new QHBoxLayout(frame);
    QHBoxLayout* gboxes = new QHBoxLayout(frame);
    frame->setLayout(vbox);
    vbox->addLayout(gboxes);

    // Suchleiste
    suchfeld = new QLineEdit(this);
    suchresetButton     = new QPushButton("zurücksetzen", this);
    suchauswahlProjekt  = new QCheckBox("Projekt", this);
    suchauswahlProjekt->setChecked(true);
    suchauswahlText     = new QCheckBox("Text", this);
    suchauswahlText->setChecked(true);

    QGroupBox*      suchenGroup         = new QGroupBox("Suchen", this);
    QGridLayout*    suchenGrid          = new QGridLayout(suchenGroup);
    QHBoxLayout*    suchbereicheHBox    = new QHBoxLayout(suchenGroup);
    gboxes->addWidget(suchenGroup);
    suchenGroup->setLayout(suchenGrid);
    suchenGrid->addWidget(new QLabel("Suche:", this),   0, 0);
    suchenGrid->addWidget(suchfeld, 0, 1);
    suchenGrid->addWidget(new QLabel("Bereich:", this), 1, 0);
    suchenGrid->addLayout(suchbereicheHBox,             1, 1);
    suchbereicheHBox->addWidget(suchauswahlProjekt);
    suchbereicheHBox->addWidget(suchauswahlText);

    QGroupBox*  filternGroup = new QGroupBox("Filtern", this);
    QGridLayout* grid = new QGridLayout(filternGroup);
    filternGroup->setLayout(grid);
    gboxes->addWidget(filternGroup);
    themenFilterCombo   = new QComboBox(this);
    personenFilterCombo = new QComboBox(this);
    grid->addWidget(new QLabel("Themen:", this), 0, 0);
    grid->addWidget(themenFilterCombo, 0, 1);
    grid->addWidget(new QLabel("Personen:", this), 1, 0);
    grid->addWidget(personenFilterCombo, 1, 1);
    gboxes->addWidget(suchresetButton);

    // Rest ;)
    tabelle     = new QTableWidget(this);
    bearbeiter  = new QComboBox(this);
    projekte    = new QComboBox(this);
    textfeld    = new QLineEdit(this);
    okButton    = new QPushButton("Ok", this);

    projekte->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    bearbeiter->setSizeAdjustPolicy(QComboBox::AdjustToContents);

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

    // Kontextmenu
    tabelle->setContextMenuPolicy(Qt::ActionsContextMenu);
    eintragBearbeitenAction = new QAction("Eintrag bearbeiten", this);
    tabelle->addAction(eintragBearbeitenAction);
    eintragLoeschenAction = new QAction("Eintrag löschen", this);
    tabelle->addAction(eintragLoeschenAction);

    tabelleFuellen();

    // Projekte Fuellen
    projekteFuellen();
    projekte->setCurrentIndex(-1);

    // Benutzer Fuellen
    benutzerFuellen();

    comboboxesAufSystemuserAnpassen();

    connect(okButton,               &QPushButton::clicked,      this,       &MainWindow::inDatenbankSchreiben);
    connect(textfeld,               &QLineEdit::returnPressed,  this,       &MainWindow::inDatenbankSchreiben);
    connect(eintragBearbeitenAction,&QAction::triggered,        this,       &MainWindow::eintragBearbeitenSlot);
    connect(eintragLoeschenAction,  &QAction::triggered,        this,       &MainWindow::eintragLoeschenSlot);
    connect(suchfeld,               &QLineEdit::textChanged,    this,       &MainWindow::tabelleFuellen);
    connect(suchresetButton,        &QPushButton::clicked,      this,       &MainWindow::suchresetButtoGeklickt);
    connect(personenFilterCombo,    SIGNAL(currentIndexChanged(int)), this,  SLOT(tabelleFuellen()));
    connect(themenFilterCombo,      SIGNAL(currentIndexChanged(int)), this,  SLOT(tabelleFuellen()));
    connect(suchauswahlProjekt,     &QCheckBox::toggled,        this,       &MainWindow::tabelleFuellen);
    connect(suchauswahlText,        &QCheckBox::toggled,        this,       &MainWindow::tabelleFuellen);
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

bool MainWindow::neueDatenbankdateiSetzen(int dbNummer)
{
    QSettings settings(PROGNAME, PROGNAME);
    QString settingsWert = QString("Datenbank")+QString::number(dbNummer);
    QString suchpfad;
    if (settings.contains(settingsWert)){
        suchpfad = settings.value(settingsWert).toString();
    } else {
        suchpfad = qApp->applicationDirPath() + "/" PROGNAME ".ond";
    }

    datenbankFile = QFileDialog::getSaveFileName(this, "Datenbankfile", suchpfad, "Datenbank (*.ond)", 0, QFileDialog::DontConfirmOverwrite);
    if (datenbankFile.isEmpty()) return false;
    settings.setValue(settingsWert, datenbankFile);

    if (!QFile::exists(datenbankFile)){
        datenbankOeffnen(dbNummer, true);
        if (!tabellenErstellen()) QMessageBox::critical(this, "Fehler", "Fehler beim Erzeugen der Datenbanktabellen!");
    }
    return true;
}

bool MainWindow::datenbankOeffnen(int dbNummer, bool initial)
{
    aktuelleDatenbank = dbNummer;
    QSettings settings(PROGNAME, PROGNAME);
    QString settingsWert = QString("Datenbank")+QString::number(dbNummer);
    if (!settings.contains(settingsWert)) {
        return false;
    }
    QString datenbankFile = settings.value(settingsWert).toString();
    if (!initial && !QFile::exists(datenbankFile)) return false;
    datenbank.setDatabaseName(datenbankFile);
    if (datenbank.open()){
        return true;
    } else {
        QMessageBox::warning(this, "Datenbank konnte nicht geoeffnet werden", "Die Datenbank unter "+datenbankFile+" konnte nicht geöffnet werden.");
        return false;
    }
}

bool MainWindow::inDatenbankWechseln(int dbNummer)
{
    QSettings settings(PROGNAME, PROGNAME);
    bool ergebnis{true};
    if (dbNummer == 1){
        zuDB1Wechseln->setEnabled(false);
        if (settings.contains("Datenbank2")){
            zuDB2Wechseln->setEnabled(true);
        }
    } else {
        zuDB1Wechseln->setEnabled(true);
        zuDB2Wechseln->setEnabled(false);
    }
    if (!datenbankOeffnen(dbNummer)){
        inDatenbankWechseln( dbNummer == 1 ? 2 : 1);
        ergebnis = false;
    }
    refresh();
    return ergebnis;
}

void MainWindow::eintragBearbeitenSlot()
{
    QSqlQuery qu;
    qu.clear();
    if (!qu.exec("SELECT Eintraege.Text, Benutzer.Name, Themen.Name, Eintraege.Eingetragen_am, Themen.Beschreibung, Eintraege.ID FROM Eintraege"
                 " JOIN Benutzer on Eingetragen_von=Benutzer.ID "
                 " JOIN Themen on Thema=Themen.ID "
                 " WHERE Eintraege.ID='"+tabelle->currentItem()->data(Qt::UserRole).toString()+"' "
                 " ORDER BY Eingetragen_am ASC ")) qDebug() << qu.lastError().text();

    if (qu.next()){
        textfeld->setText(qu.value(0).toString());
        bearbeiter->setCurrentIndex(bearbeiter->findText(qu.value(1).toString()));
        projekte->setCurrentIndex(projekte->findText(qu.value(2).toString()));
        zuAendernderDatensatzID=qu.value(5).toInt();
        QPalette mypalette = this->palette();
        mypalette.setColor(QPalette::Text, Qt::red);
        textfeld->setPalette(mypalette);
        textfeld->setFocus();
    }
}

void MainWindow::eintragLoeschenSlot()
{
    if (QMessageBox::question(this, "Löschen", "Wollten Sie den Datensatz wirklich löschen.") != QMessageBox::Yes) return;
    QSqlQuery qu;
    qu.clear();
    qu.exec("DELETE FROM Eintraege WHERE ID='"+tabelle->currentItem()->data(Qt::UserRole).toString()+"'");
    qu.finish();
    refresh();
}

void MainWindow::suchresetButtoGeklickt()
{
    gefilter = false;
    disconnect(personenFilterCombo,    SIGNAL(currentIndexChanged(int)), this,  SLOT(tabelleFuellen()));
    disconnect(themenFilterCombo,      SIGNAL(currentIndexChanged(int)), this,  SLOT(tabelleFuellen()));
    personenFilterCombo->setCurrentIndex(0);
    themenFilterCombo->setCurrentIndex(0);
    suchfeld->clear();
    connect(personenFilterCombo,    SIGNAL(currentIndexChanged(int)), this,  SLOT(tabelleFuellen()));
    connect(themenFilterCombo,      SIGNAL(currentIndexChanged(int)), this,  SLOT(tabelleFuellen()));
    tabelleFuellen();
}


void MainWindow::inDatenbankSchreiben()
{
    if (projekte->currentIndex() < 0 || textfeld->text().isEmpty() || bearbeiter->currentIndex() < 0) return;
    QSqlQuery qu;
    qu.clear();
    if (zuAendernderDatensatzID == 0){
        if (!qu.exec("INSERT INTO Eintraege (Eingetragen_von, Eingetragen_am, Thema, Text) VALUES ("
                "'"+bearbeiter->itemData(bearbeiter->currentIndex()).toString()+"', "
                "'"+QDate::currentDate().toString("yyyyMMdd")+"', "
                "'"+projekte->itemData(projekte->currentIndex()).toString()+"', "
                "'"+textfeld->text().replace('\'',"\'\'")+"'"
                ")")) qDebug() << qu.lastError().text();
    } else {
        if (!qu.exec("UPDATE Eintraege SET Eingetragen_von='"+bearbeiter->itemData(bearbeiter->currentIndex()).toString()+"', "
                     "Thema='"+projekte->itemData(projekte->currentIndex()).toString()+"', "
                     "Text='"+textfeld->text().replace('\'',"\'\'")+"' "
                     " WHERE Eintraege.ID='"+QString::number(zuAendernderDatensatzID)+"'"
                )) qDebug() << qu.lastError().text();
        textfeld->setPalette(this->palette());

    }
    qu.finish();
    qu.clear();
    if (!qu.exec("UPDATE Benutzer SET Letztes_Thema='"+projekte->itemData(projekte->currentIndex()).toString()+
                 "' WHERE ID='"+bearbeiter->itemData(bearbeiter->currentIndex()).toString()+"'"))
        qDebug() << qu.lastError().text();

    qu.finish();
    textfeld->clear();
    tabelleFuellen();
    zuAendernderDatensatzID = 0;
}

void MainWindow::tabelleFuellen()
{
    gefilter = false;
    for (int j{tabelle->rowCount()-1}; j >= 0; j--){
        tabelle->removeRow(j);
    }
    QSqlQuery qu;
    qu.clear();
    QString sqlstring = "SELECT Eintraege.Text, Benutzer.Name, Themen.Name, Eintraege.Eingetragen_am, Themen.Beschreibung, Eintraege.ID FROM Eintraege"
            " JOIN Benutzer on Eingetragen_von=Benutzer.ID "
            " JOIN Themen on Thema=Themen.ID";
    if (!suchfeld->text().isEmpty() && !(!suchauswahlProjekt->isChecked() && !suchauswahlText->isChecked())){
        gefilter = true;
        if (suchauswahlProjekt->isChecked() || suchauswahlText->isChecked()) sqlstring.append(" WHERE ((");
        if (suchauswahlProjekt->isChecked()) sqlstring.append(" Themen.Name LIKE '%"+suchfeld->text()+"%' OR Themen.Beschreibung LIKE '%"+suchfeld->text()+"%'");
        if (suchauswahlProjekt->isChecked() && suchauswahlText->isChecked()) sqlstring.append(" OR ");
        if (suchauswahlText->isChecked()) sqlstring.append(" Eintraege.Text LIKE '%"+suchfeld->text()+"%')");
    }

    // Filtern auf die Filtercomboboxes
    if (themenFilterCombo->currentIndex() > 0 || personenFilterCombo->currentIndex() > 0){
        if (gefilter){
            sqlstring.append(" AND ");
        } else {
            gefilter = true;
            sqlstring.append(" WHERE (");
        }
        if (themenFilterCombo->currentIndex() > 0){
            sqlstring.append(" Themen.ID='"+themenFilterCombo->currentData().toString()+"' ");
        }
        if (themenFilterCombo->currentIndex() > 0 && personenFilterCombo->currentIndex() > 0){
            sqlstring.append(" AND ");
        }
        if (personenFilterCombo->currentIndex() > 0){
            sqlstring.append(" Benutzer.ID='"+personenFilterCombo->currentData().toString()+"' ");
        }    
    }
    if (gefilter)
        sqlstring.append(")");

    sqlstring.append(" ORDER BY Eingetragen_am ASC");

    qDebug() << sqlstring;
    if (!qu.exec(sqlstring)) qDebug() << sqlstring << " brachte den Fehler: " << qu.lastError().text();
    int i{0};
    while (qu.next()){
        tabelle->setRowCount(tabelle->rowCount()+1);
        int id = qu.value(5).toInt();
        tabelle->setItem( i,   0, neuesTableItem(qu.value(3).toString(), id));
        tabelle->setItem( i,   1, neuesTableItem(qu.value(2).toString(), id, true, qu.value(4).toString()));
        tabelle->setItem( i,   2, neuesTableItem(qu.value(0).toString(), id, false, QString()));
        tabelle->setItem( i++, 3, neuesTableItem(qu.value(1).toString(), id));
    }
    qu.finish();
    tabelle->resizeRowsToContents();
    tabelle->scrollToBottom();
}

void MainWindow::projekteFuellen()
{
    projekte->clear();
    themenFilterCombo->clear();
    themenFilterCombo->addItem("- Themen -");
    QSqlQuery qu;
    qu.clear();
    qu.exec("SELECT ID, Name FROM Themen WHERE Aktiv='1'");
    while (qu.next()){
        projekte->addItem(qu.value(1).toString(), qu.value(0));
        themenFilterCombo->addItem(qu.value(1).toString(), qu.value(0));
    }
    qu.finish();
}

void MainWindow::benutzerFuellen()
{
    bearbeiter->clear();
    personenFilterCombo->clear();
    personenFilterCombo->addItem("- Personen -");
    QSqlQuery qu;
    qu.clear();
    qu.exec("SELECT ID, Name FROM Benutzer");
    while (qu.next()){
        bearbeiter->addItem(qu.value(1).toString(), qu.value(0));
        personenFilterCombo->addItem(qu.value(1).toString(), qu.value(0));
    }
    qu.finish();
}

