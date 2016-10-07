#-------------------------------------------------
#
# Project created by QtCreator 2016-04-05T14:33:56
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = PDTool
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dialogb.cpp \
    qcustomplot.cpp \
    qaudiolevel.cpp \
    make_wav.cpp

HEADERS  += mainwindow.h \
    dialogb.h \
    qcustomplot.h \
    qaudiolevel.h \
    definitions.h \
    make_wav.h

FORMS    += mainwindow.ui \
    dialogb.ui
