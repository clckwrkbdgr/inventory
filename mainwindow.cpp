#include <QtGui/QStatusBar>
#include <QtGui/QMenu>
#include <QtCore/QSettings>

#include "inventory.h"
#include "listdialog.h"
#include "historydialog.h"
#include "filterdialog.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);
	connect(ui.actionAdd, SIGNAL(triggered()), this, SLOT(addItem()));
	connect(ui.actionDeactivate, SIGNAL(triggered()), this, SLOT(deactivate()));
	connect(ui.actionPlaces, SIGNAL(triggered()), this, SLOT(editPlaces()));
	connect(ui.actionItemTypes, SIGNAL(triggered()), this, SLOT(editItemTypes()));
	connect(ui.actionFilter, SIGNAL(triggered()), this, SLOT(setFilter()));
	connect(ui.actionHistory, SIGNAL(triggered()), this, SLOT(showHistory()));
	connect(ui.actionPrint, SIGNAL(triggered()), this, SLOT(print()));
	connect(ui.view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(popupMenu(QPoint)));

	itemMenu = new QMenu(tr("Item menu"), this);
	itemMenu->addAction(ui.actionDeactivate);
	itemMenu->addSeparator();
	itemMenu->addAction(ui.actionHistory);

	// Settings.
	QSettings settings;
	resize(settings.value("mainwindow/size", size()).toSize());
	move(settings.value("mainwindow/pos", pos()).toPoint());
	if(settings.value("mainwindow/maximized", false).toBool())
		setWindowState(Qt::WindowMaximized);

	// Database.
	Inventory::open("inventory.db3");
	model = new InventoryModel(this);
	model->updateList(Inventory::instance()->items());

	ui.view->setModel(model);
	ui.view->setItemDelegate(new InventoryDelegate(this));
	ui.view->resizeColumnsToContents();
	connect(model, SIGNAL(layoutChanged()), ui.view, SLOT(resizeColumnsToContents()));
	connect(model, SIGNAL(modelReset()), ui.view, SLOT(resizeColumnsToContents()));
	connect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(resizeView(QModelIndex, QModelIndex)));
}

MainWindow::~MainWindow() {
	// Settings.
	QSettings settings;
	settings.setValue("mainwindow/maximized",
			windowState().testFlag(Qt::WindowMaximized));
	if(!windowState().testFlag(Qt::WindowMaximized))
	{
		settings.setValue("mainwindow/size", size());
		settings.setValue("mainwindow/pos", pos());
	}

	// Database.
	Inventory::close();
}

void MainWindow::resizeView(const QModelIndex&, const QModelIndex&) {
	ui.view->resizeColumnsToContents();
}

void MainWindow::addItem() {
	model->insertRow(model->rowCount());
}

void MainWindow::deactivate() {
	QModelIndex index = model->index(ui.view->currentIndex().row(), 5); // 'Active' field.
	model->setData(index, false, Qt::EditRole);
}

void MainWindow::editPlaces() {
	ListDialog *dialog = new ListDialog(true, this);
	dialog->exec();
	delete dialog;
}

void MainWindow::editItemTypes() {
	ListDialog *dialog = new ListDialog(false, this);
	dialog->exec();
	delete dialog;
}

void MainWindow::setFilter() {
	FilterDialog *dialog = new FilterDialog(this);
	if(dialog->exec()) {
		Place *place = NULL;
		ItemType *itemType = NULL;
		bool *activity = NULL;

		if(dialog->usesPlace() && dialog->placeId() > -1) {
			place = new Place(dialog->placeId());
		}
		if(dialog->usesItemType() && dialog->itemTypeId() > -1) {
			itemType = new ItemType(dialog->itemTypeId());
		}
		if(dialog->usesActivity()) {
			activity = new bool(dialog->activity());
		}
		model->updateList(Inventory::instance()->filteredItems(place, itemType, activity));

		if(place)
			delete place;
		if(itemType)
			delete itemType;
		if(activity)
			delete activity;
	}
	delete dialog;
}

void MainWindow::showHistory() {
	HistoryDialog *dialog = NULL;
	try {
		Item item(model->idAt(ui.view->currentIndex().row()));
		dialog = new HistoryDialog(item, this);
		dialog->exec(); 
	} catch(...) {}
	
	if(dialog)
		delete dialog;
}

void MainWindow::print() {
	statusBar()->showMessage(tr("Print")); // @todo: print to pdf, i guess
}

void MainWindow::popupMenu(const QPoint &pos) {
	try {
		Item item(model->idAt(ui.view->currentIndex().row()));
		ui.actionDeactivate->setEnabled(item.isActive());
	} catch(...) {}

	itemMenu->popup(ui.view->mapToGlobal(pos));
}

