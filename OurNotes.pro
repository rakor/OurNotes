#-------------------------------------------------
#
# Project created by QtCreator 2014-05-28T19:41:39
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG  += c++11

TARGET = OurNotes
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    projektpflege.cpp \
    benutzerpflege.cpp

HEADERS  += mainwindow.h \
    projektpflege.h \
    benutzerpflege.h

OTHER_FILES += \
    README
