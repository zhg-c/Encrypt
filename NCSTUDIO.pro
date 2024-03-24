#-------------------------------------------------
#
# Project created by QtCreator 2024-03-15T21:25:44
#
#-------------------------------------------------

QT       += core gui
CONFIG += C++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NCSTUDIO
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    database.cpp \
    daysdlg.cpp

HEADERS  += dialog.h \
    database.h \
    daysdlg.h

FORMS    += dialog.ui \
    daysdlg.ui

LIBS += -L$$PWD -lsqlite3

RESOURCES += \
    res.qrc

RC_ICONS += res/logo.ico

