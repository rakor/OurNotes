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
    QTableWidgetItem*   neuesTableItem(QString text, bool tooltip=false, QString tooltiptext=QString(), int id=0);
    QAction*            zuDB1Wechseln{nullptr};
    QAction*            zuDB2Wechseln{nullptr};
    int                 aktuelleDatenbank{0};
    int                 zuAendernderDatensatzID{0};


protected slots:
    void inDatenbankSchreiben();
    void tabelleFuellen();
    void projekteFuellen();
    void benutzerFuellen();
    bool neueDatenbankdateiSetzen(int);
    bool datenbankOeffnen(int, bool=false);
    bool inDatenbankWechseln(int);
    void eintragBearbeitenSlot();
};

#endif // MAINWINDOW_H
