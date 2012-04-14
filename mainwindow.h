#pragma once

#include <QtGui/QMainWindow>
#include <QtGui/QTabBar>

#include "models.h"

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow {
	Q_OBJECT;
	Q_DISABLE_COPY(MainWindow);
public:
	MainWindow(QWidget * parent = 0);
	virtual ~MainWindow();
private:
	QString databaseLocation;
	Ui::MainWindow ui;
	QTabBar * tabs;
	
	Inventory::InventoryModel * inventoryModel;
	Inventory::PrintableInventoryModel * printableModel;
	Inventory::ReferenceModel * itemTypesModel;
	Inventory::ReferenceModel * placesModel;
	Inventory::ReferenceModel * personsModel;
};
