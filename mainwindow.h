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

private slots:
	void setupTab(int index);
	void resetView(bool update = false);

	void on_actionShowHistory_triggered();
	void on_actionAddMultiline_triggered();
	void on_actionPrintCSV_triggered();
	void on_actionAdd_triggered();
	void on_actionRemove_triggered();
	void on_actionUndo_triggered();

	void on_buttonUseItemTypeFilter_toggled(bool);
	void on_listItemTypeFilter_currentIndexChanged(int);
	void on_buttonUsePlaceFilter_toggled(bool);
	void on_listPlaceFilter_currentIndexChanged(int);
	void on_buttonUseWrittenOffFilter_toggled(bool);
	void on_listWrittenOffFilter_currentIndexChanged(int);

    void on_actionShowItemType_toggled(bool);
    void on_actionShowItemPlace_toggled(bool);
    void on_actionShowResponsiblePerson_toggled(bool);
    void on_actionShowItemName_toggled(bool);
    void on_actionShowINN_toggled(bool);
    void on_actionShowWritingOff_toggled(bool);
    void on_actionShowRepairState_toggled(bool);
    void on_actionShowCheckedItemState_toggled(bool);
    void on_actionShowNote_toggled(bool);

	void on_actionSwitchWrittenOffOn_triggered();
	void on_actionSwitchWrittenOffOff_triggered();
	void on_actionSwitchRepairOn_triggered();
	void on_actionSwitchRepairOff_triggered();
	void on_actionSwitchCheckedOn_triggered();
	void on_actionSwitchCheckedOff_triggered();
private:
	QString databaseLocation;
	Ui::MainWindow ui;
	QTabBar * tabs;
	TabIndex tabIndex;
	QSet<int> hiddenColumns;
	
	Inventory::InventoryModel * inventoryModel;
	Inventory::PrintableInventoryModel * printableModel;
	Inventory::ReferenceModel * itemTypesModel;
	Inventory::ReferenceModel * placesModel;
	Inventory::ReferenceModel * personsModel;

	QAction * columnAction(int);
	void setColumnVisibility(int column, bool visible);
};

