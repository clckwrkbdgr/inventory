#pragma once

#include <QtGui/QMainWindow>
#include <QtGui/QTabBar>
#include <QtGui/QKeyEvent>
#include <QtGui/QTableView>

#include "models.h"

#include "ui_mainwindow.h"

struct TabIndex {
	int MAIN;
	int PRINT;
	int TYPES;
	int PLACES;
	int PERSONS;
};

class EnterTableView : public QTableView {
	Q_OBJECT
	Q_DISABLE_COPY(EnterTableView)
public:
	EnterTableView(QWidget * parent = 0) : QTableView(parent) {}
	virtual ~EnterTableView() {}
protected:
	virtual void keyPressEvent(QKeyEvent * event) {
		if(state() != QAbstractItemView::EditingState) {
			switch(event->key()) {
				case Qt::Key_Enter:
				case Qt::Key_Return:
					edit(currentIndex());
					break;
				default:
					break;
			}
		}
		QTableView::keyPressEvent(event);
	}
};

class MainWindow : public QMainWindow {
	Q_OBJECT;
	Q_DISABLE_COPY(MainWindow);
public:
	MainWindow(QWidget * parent = 0);
	virtual ~MainWindow();

private slots:
	void saveCurrentIndex();
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

	void on_actionSwitchCheckedOff_triggered();
private:
	QTableView * view;
	QString databaseLocation;
	Ui::MainWindow ui;
	QTabBar * tabs;
	TabIndex tabIndex;
	QSet<int> hiddenColumns;
	QPoint oldIndex;
	
	Inventory::InventoryModel * inventoryModel;
	Inventory::PrintableInventoryModel * printableModel;
	Inventory::ReferenceModel * itemTypesModel;
	Inventory::ReferenceModel * placesModel;
	Inventory::ReferenceModel * personsModel;

	QAction * columnAction(int);
	void setColumnVisibility(int column, bool visible);
};

