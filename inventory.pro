TARGET = inventory
DESTDIR = bin
CONFIG = uic qt release warn_on
QT += sql

OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
UI_DIR = tmp

FORMS += mainwindow.ui
HEADERS = models.h mainwindow.h
SOURCES = main.cpp models.cpp mainwindow.cpp
