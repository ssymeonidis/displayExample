#-------------------------------------------------
#
# Project created by QtCreator 2019-03-15T09:55:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = display
TEMPLATE = app


SOURCES += window.cpp    \
           glWidget.cpp  \
           vid_tiff.c    \
           vid_ctrl.cpp  \
           main.cpp

HEADERS += window.h      \
           glWidget.h    \
           vid_tiff.h    \
           vid_ctrl.h

FORMS    += window.ui

LIBS     += -lGLEW       \
            -ltiff
