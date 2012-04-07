TARGET = test
DESTDIR = ../bin
CONFIG += qt release qtestlib

INCLUDEPATH = ../src

RCC_DIR = tmp
UI_DIR = tmp
MOC_DIR = tmp
OBJECTS_DIR = tmp

SOURCES = test.cpp ../src/db.cpp
HEADERS = ../src/db.h
