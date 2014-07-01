#-------------------------------------------------
#
# Project created by QtCreator 2014-06-28T11:39:18
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SILBET_Kody_kreskowe
TEMPLATE = app
RC_FILE = app.rc


SOURCES += main.cpp\
        mainwindow.cpp \
    keyboard_hook.cpp

HEADERS  += mainwindow.h \
    keyboard_hook.h

FORMS    += mainwindow.ui
