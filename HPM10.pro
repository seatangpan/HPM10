#-------------------------------------------------
#
# Project created by QtCreator 2023-12-07T14:08:40
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += serialport

TARGET = HPM10
TEMPLATE = app

CONFIG += c++17

#DEFINES += QT_NO_DEBUG_OUTPUT

SOURCES += main.cpp\
        mainwindow.cpp \
    commhpm10.cpp \
    TranslateLang.cpp \
    ProgressDisplayWidget.cpp \
    usblisten.cpp \
    CheckBoxHeaderView.cpp

HEADERS  += mainwindow.h \
    commhpm10.hpp \
    TranslateLang.h \
    ProgressDisplayWidget.h \
    usblisten.h \
    CheckBoxHeaderView.h

FORMS    += mainwindow.ui

RC_ICONS = logo.ico
