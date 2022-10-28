#-------------------------------------------------
#
# Project created by QtCreator 2014-02-22T15:15:24
#
#-------------------------------------------------

QT       += core gui dbus widgets

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = QAudioSwitcher
TEMPLATE = app

SOURCES += main.cpp qaudioswitcher.cpp \
    audiodevice.cpp \
    audiomanager.cpp \
    audiomanagerfactory.cpp \
    devicelistwidgetitem.cpp

HEADERS  += qaudioswitcher.h \
    audiodevice.h \
    audiomanager.h \
    audiomanagerfactory.h \
    devicelistwidgetitem.h

CONFIG(PULSEAUDIO) {
    SOURCES += pulseaudiosinksmanager.cpp  paoperation.cpp
    HEADERS += pulseaudiosinksmanager.h paoperation.h
    DEFINES += PULSEAUDIO
    message("pulseaudio configured")
}

FORMS    += qaudioswitcher.ui

QMAKE_CXXFLAGS += -std=c++11 -Wall

unix: CONFIG += link_pkgconfig

CONFIG(PULSEAUDIO) {
    unix: PKGCONFIG += libpulse
}
