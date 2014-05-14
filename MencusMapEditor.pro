#-------------------------------------------------
#
# Project created by QtCreator 2014-04-24T15:43:36
#
#-------------------------------------------------

QT       += core gui
QT+=testlib

QMAKE_CXXFLAGS += -std=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MencusMapEditor
TEMPLATE = app

LIBS += -L/usr/local/quazip/lib -lquazip -lz
INCLUDEPATH += /usr/local/quazip/include


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
    linkslistwidget.cpp \
    eventdatamask.cpp \
    edittextdialog.cpp \
    languageresources.cpp \
    editexecutabledialog.cpp \
    editmapsizedialog.cpp \
    editendangeredtiledialog.cpp

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
    linkslistwidget.h \
    eventdatamask.h \
    edittextdialog.h \
    languageresources.h \
    editexecutabledialog.h \
    editmapsizedialog.h \
    editendangeredtiledialog.h

FORMS    += mainwindow.ui \
    editeventdialog.ui \
    edittextdialog.ui \
    editexecutabledialog.ui \
    editmapsizedialog.ui \
    editendangeredtiledialog.ui
