#-------------------------------------------------
#
# Project created by QtCreator 2014-06-17T15:46:44
#
#-------------------------------------------------

QT -= core gui


TARGET = OpenMeshDecimater
CONFIG   += console
CONFIG   -= app_bundle
CONFIG+=c++11

INCLUDEPATH += $$PWD/
#INCLUDEPATH += $$PWD/Algorithms
HEADERS += \
            #RemesherPlugin.hh \

SOURCES += \
    main.cpp \
            #RemesherPlugin.cc \



unix: LIBS += -L/usr/local/lib/OpenMesh/ -lOpenMeshCore
unix: LIBS += -L/usr/local/lib/OpenMesh/ -lOpenMeshTools

INCLUDEPATH += /usr/local/include/
DEPENDPATH += /usr/local/include/
