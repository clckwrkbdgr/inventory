TARGET = inventory
DESTDIR = ../bin
CONFIG += qt release

INCLUDEPATH = ../src

RCC_DIR = tmp
UI_DIR = tmp
MOC_DIR = tmp
OBJECTS_DIR = tmp

FORMS = mainwindow.ui
SOURCES = main.cpp mainwindow.cpp
HEADERS = mainwindow.h

SOURCES += ../src/db.cpp ../src/models.cpp
HEADERS += ../src/db.h ../src/models.h

