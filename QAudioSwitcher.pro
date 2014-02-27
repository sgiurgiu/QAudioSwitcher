#-------------------------------------------------
#
# Project created by QtCreator 2014-02-22T15:15:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QAudioSwitcher
TEMPLATE = app

SOURCES += main.cpp qaudioswitcher.cpp pulseaudiosinksmanager.cpp \
    pulseaudiosink.cpp \
    pulseaudiosinklistwidgetitem.cpp

HEADERS  += pulseaudiosinksmanager.h qaudioswitcher.h \
    pulseaudiosink.h \
    pulseaudiosinklistwidgetitem.h

FORMS    += qaudioswitcher.ui

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += libpulse
