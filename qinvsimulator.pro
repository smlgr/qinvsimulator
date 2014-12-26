#-------------------------------------------------
#
# Project created by QtCreator 2014-12-25T23:26:50
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = qinvsimulator
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    qinvsimulator.cpp

HEADERS += \
    qinvsimulator.hpp

QMAKE_CXXFLAGS += -std=c++0x
