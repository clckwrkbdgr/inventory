TARGET = inventory
VERSION = 3.0.0
DESTDIR = bin
CONFIG += uic release warn_on
QT += widgets sql

OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
UI_DIR = tmp

TRANSLATIONS = qinventory_ru.ts qinventory_uk.ts
FORMS += mainwindow.ui
HEADERS = models.h mainwindow.h
SOURCES = main.cpp models.cpp mainwindow.cpp

runAction.target = run
runAction.depends = all
runAction.commands = bin/inventory
QMAKE_EXTRA_TARGETS += runAction

translateAction.target = translate
translateAction.depends = all
translateAction.commands = lupdate inventory.pro \
		$$escape_expand(\n\t) lconvert -o bin/qinventory_ru.qm qinventory_ru.ts \
		$$escape_expand(\n\t) lconvert -o bin/qinventory_uk.qm qinventory_uk.ts
QMAKE_EXTRA_TARGETS += translateAction
