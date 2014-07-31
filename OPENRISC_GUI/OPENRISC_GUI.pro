#-------------------------------------------------
#
# Project created by QtCreator 2013-09-24T20:08:55
#
#-------------------------------------------------

QT       += core gui xml network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OPENRISC_GUI
TEMPLATE = app

QMAKE_LFLAGS += -static-libgcc

SOURCES += main.cpp\
        mainwindow.cpp \
    newtab.cpp \
    uartdialog.cpp \
    codeeditor.cpp \
    memorytablewidget.cpp \
    xmlparser.cpp \
    xmlgenerator.cpp \
    csyntaxhighlighter.cpp \
    asmsyntaxhighlighter.cpp

HEADERS  += mainwindow.h \
    newtab.h \
    headers.h \
    uartdialog.h \
    codeeditor.h \
    memorytablewidget.h \
    xmlparser.h \
    xmlgenerator.h \
    csyntaxhighlighter.h \
    asmsyntaxhighlighter.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc
