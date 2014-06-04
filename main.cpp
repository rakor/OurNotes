#include "mainwindow.h"
#include <QApplication>
#include "global.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(PROGNAME);
    a.setApplicationDisplayName(PROGNAME);
    MainWindow w;
    w.show();

    return a.exec();
}
