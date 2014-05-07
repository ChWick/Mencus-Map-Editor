#-------------------------------------------------
#
# Project created by QtCreator 2014-04-24T15:43:36
#
#-------------------------------------------------

QT       += core gui

QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MencusMapEditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    map.cpp \
    maparea.cpp \
    tilesgraphicsview.cpp \
    tilepreviewgraphicsview.cpp \
    entitieslistwidget.cpp \
    objectsgraphicsview.cpp \
    eventslistwidget.cpp \
    editeventdialog.cpp \
    linkslistwidget.cpp

HEADERS  += mainwindow.h \
    map.h \
    grid2d.h \
    maparea.h \
    tilesgraphicsview.h \
    tilepreviewgraphicsview.h \
    entitieslistwidget.h \
    objectsgraphicsview.h \
    toolsdefinition.h \
    eventslistwidget.h \
    editeventdialog.h \
    linkslistwidget.h

FORMS    += mainwindow.ui \
    editeventdialog.ui
