TARGET = inventory
VERSION = 2.1.0
DESTDIR = bin
CONFIG = uic qt release warn_on
QT += sql

OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
UI_DIR = tmp

TRANSLATIONS = inventory_ru.ts
FORMS += mainwindow.ui
HEADERS = models.h mainwindow.h
SOURCES = main.cpp models.cpp mainwindow.cpp
