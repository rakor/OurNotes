#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDebug>
#include <QPushButton>
#include <QLineEdit>
#include <QDateEdit>
#include <QDate>
#include <QComboBox>
#include <QListWidget>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QFileInfo>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>
#include <QHeaderView>
#include <QApplication>
#include <QSettings>
#include <QFileDialog>
#include <QKeyEvent>
#include <QToolBar>
#include <QMessageBox>
#include <QCheckBox>
#include <QGroupBox>
#include "projektpflege.h"
#include "benutzerpflege.h"
#include "global.h"

#define DATENBANKFILE   "OurNotes.ond"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void guiBauen();

protected:
    bool                gefilter{false};
    QSqlDatabase        datenbank;
    bool                tabellenErstellen();
    QTableWidget*       tabelle{nullptr};
    QComboBox*          bearbeiter{nullptr};
    QComboBox*          projekte{nullptr};
    QLineEdit*          textfeld{nullptr};
    QPushButton*        okButton{nullptr};
    QString             datenbankFile;
    void                keyPressEvent(QKeyEvent *e);
    void                resizeEvent(QResizeEvent* e);
    void                refresh();
    void                comboboxesAufSystemuserAnpassen();
    QTableWidgetItem*   neuesTableItem(QString text, int id=0, bool tooltip=false, QString tooltiptext=QString());
    QAction*            zuDB1Wechseln{nullptr};
    QAction*            zuDB2Wechseln{nullptr};
    int                 aktuelleDatenbank{0};
    int                 zuAendernderDatensatzID{0};
    QAction*            eintragBearbeitenAction{nullptr};
    QAction*            eintragLoeschenAction{nullptr};
    QCheckBox*          suchauswahlText{nullptr};
    QCheckBox*          suchauswahlProjekt{nullptr};
    QPushButton*        suchresetButton{nullptr};
    QLineEdit*          suchfeld{nullptr};
    QComboBox*          themenFilterCombo{nullptr};
    QComboBox*          personenFilterCombo{nullptr};
    int                 datenbankversionTesten();
    bool                datenbankschemaAktuallisieren();


protected slots:
    void inDatenbankSchreiben();
    void tabelleFuellen();
    void projekteFuellen();
    void benutzerFuellen();
    bool neueDatenbankdateiSetzen(int);
    bool datenbankOeffnen(int, bool=false);
    bool inDatenbankWechseln(int);
    void eintragBearbeitenSlot();
    void eintragLoeschenSlot();
    void suchresetButtoGeklickt();


};

#endif // MAINWINDOW_H
