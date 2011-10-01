TEMPLATE = app
TARGET = inventory
VERSION = 0.9.0
QT += sql
CONFIG += qt release

MOC_DIR = tmp
UI_DIR = tmp
OBJECTS_DIR = tmp
RCC_DIR = tmp

TRANSLATIONS = inventory_ru.ts
RESOURCES = icons.qrc
FORMS = addlistdialog.ui \
	filterdialog.ui \
	historydialog.ui \
	listdialog.ui \
	mainwindow.ui
HEADERS = inventory.h \
	models.h \
	filterdialog.h \
	historydialog.h \
	listdialog.h \
	mainwindow.h
SOURCES = inventory.cpp \
	models.cpp \
	filterdialog.cpp \
	historydialog.cpp \
	listdialog.cpp \
	mainwindow.cpp \
	main.cpp

