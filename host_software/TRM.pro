#-------------------------------------------------
#
# Project created by QtCreator 2024-03-25T00:15:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TRM
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    hidlibrary.h

FORMS    += mainwindow.ui

win32: LIBS += -lsetupapi
linux: LIBS += -lusb-1.0
