#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>
#include "global.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName(PROGNAME);
    a.setApplicationDisplayName(PROGNAME);
    a.setApplicationVersion(PROGVERSION);

    // Deutsche Menues
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    MainWindow w;

    return a.exec();
}
