TARGET = test
CONFIG = qt release warn_on
QT += sql testlib

OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
UI_DIR = tmp

INCLUDEPATH = ..

HEADERS = ../models.h
SOURCES = test.cpp ../models.cpp

