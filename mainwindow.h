#pragma once

#include <QtGui/QMainWindow>
#include <QtGui/QTabBar>

#include "models.h"

#include "ui_mainwindow.h"

struct TabIndex {
	int MAIN;
	int PRINT;
	int TYPES;
	int PLACES;
	int PERSONS;
};

class MainWindow : public QMainWindow {
	Q_OBJECT;
	Q_DISABLE_COPY(MainWindow);
public:
	MainWindow(QWidget * parent = 0);
	virtual ~MainWindow();

public slots:
	void setupTab(int index);
	void resetView(bool update = false);

	void on_actionShowHistory_triggered();
	void on_actionAddMultiline_triggered();
	void on_actionPrintCSV_triggered();
	void on_actionAdd_triggered();
	void on_actionRemove_triggered();
	void on_actionHideResponsiblePerson_toggled(bool);

	void on_buttonUseItemTypeFilter_toggled(bool);
	void on_listItemTypeFilter_currentIndexChanged(int);
	void on_buttonUsePlaceFilter_toggled(bool);
	void on_listPlaceFilter_currentIndexChanged(int);
	void on_buttonUseWrittenOffFilter_toggled(bool);
	void on_listWrittenOffFilter_currentIndexChanged(int);
private:
	QString databaseLocation;
	Ui::MainWindow ui;
	QTabBar * tabs;
	TabIndex tabIndex;
	
	Inventory::InventoryModel * inventoryModel;
	Inventory::PrintableInventoryModel * printableModel;
	Inventory::ReferenceModel * itemTypesModel;
	Inventory::ReferenceModel * placesModel;
	Inventory::ReferenceModel * personsModel;
};

