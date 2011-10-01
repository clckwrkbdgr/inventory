#include <QtDebug>
#include <QtGui/QStatusBar>
#include <QtGui/QMenu>
#include <QtCore/QSettings>
#include <QtGui/QPrinter>
#include <QtGui/QPrintDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QPainter>
#include <QtSql/QSqlError>

#include "inventory.h"
#include "listdialog.h"
#include "historydialog.h"
#include "filterdialog.h"
#include "printpreviewdialog.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	ui.setupUi(this);
	connect(ui.actionAdd, SIGNAL(triggered()), this, SLOT(addItem()));
	connect(ui.actionDeactivate, SIGNAL(triggered()), this, SLOT(deactivate()));
	connect(ui.actionRemoveItem, SIGNAL(triggered()), this, SLOT(removeItem()));
	connect(ui.actionPlaces, SIGNAL(triggered()), this, SLOT(editPlaces()));
	connect(ui.actionItemTypes, SIGNAL(triggered()), this, SLOT(editItemTypes()));
	connect(ui.actionFilter, SIGNAL(triggered()), this, SLOT(setFilter()));
	connect(ui.actionHistory, SIGNAL(triggered()), this, SLOT(showHistory()));
	connect(ui.actionPrint, SIGNAL(triggered()), this, SLOT(print()));
	connect(ui.actionEdit, SIGNAL(triggered()), this, SLOT(editField()));
	connect(ui.view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(popupMenu(QPoint)));

	itemMenu = new QMenu(tr("Item menu"), this);
	itemMenu->addAction(ui.actionDeactivate);
	itemMenu->addAction(ui.actionRemoveItem);
	itemMenu->addSeparator();
	itemMenu->addAction(ui.actionHistory);

	// Settings.
	QSettings settings;
	resize(settings.value("mainwindow/size", size()).toSize());
	move(settings.value("mainwindow/pos", pos()).toPoint());
	if(settings.value("mainwindow/maximized", false).toBool())
		setWindowState(Qt::WindowMaximized);

	// Database.
	try {
		Inventory::open("inventory.db3");
		model = new InventoryModel(this);
		model->updateList(Inventory::instance()->items());
	} catch(IDatabaseObject::DBErrorException e) {
		QMessageBox::critical(this, tr("Error"), e.query.lastError().databaseText() + "\n" + e.query.lastError().driverText());
		exit(1);
	}

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
	ui.view->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::removeItem() {
	if(QMessageBox::question(this, tr("Remove item"), tr("Do you really want to remove selected item?\n"
							"All associated history also will be deleted."),
							QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Yes) {
		model->removeRow(ui.view->currentIndex().row());
	}
}

void MainWindow::addItem() {
	model->insertRow(model->rowCount());
}

void MainWindow::deactivate() {
	if(QMessageBox::question(this, tr("Deactivate item"), tr("Do you really want to deactivate selected item?"),
							QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Yes) {
		QModelIndex index = model->index(ui.view->currentIndex().row(), 4); // 'Active' field.
		model->setData(index, false, Qt::EditRole);
	}
}

void MainWindow::editField() {
	ui.view->edit(ui.view->currentIndex());
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
	dialog->setFilter(Inventory::getLastFilter());
	if(dialog->exec()) {
		try {
			model->updateList(Inventory::instance()->filteredItems(dialog->filter()));
		} catch(IIdObject::InvalidIdException e) {
			QMessageBox::critical(this, tr("Error"), tr("Database invalid id exception!"));
		} catch(IDatabaseObject::DBErrorException e) {
			QMessageBox::critical(this, tr("Error"), e.query.lastError().databaseText() + "\n" + e.query.lastError().driverText());
		}
	}
	delete dialog;
}

void MainWindow::showHistory() {
	HistoryDialog *dialog = NULL;

	Item item(model->idAt(ui.view->currentIndex().row()));
	dialog = new HistoryDialog(item, this);
	dialog->exec(); 
	delete dialog;
}

void MainWindow::print() {
	PrintPreviewDialog *dialog = new PrintPreviewDialog(this);
	dialog->exec();
	delete dialog;
}

void MainWindow::popupMenu(const QPoint &pos) {
	try {
		Item item(model->idAt(ui.view->currentIndex().row()));
		ui.actionDeactivate->setEnabled(item.isActive());
	} catch(IDatabaseObject::DBErrorException e) {
		QMessageBox::critical(this, tr("Error"), e.query.lastError().databaseText() + "\n" + e.query.lastError().driverText());
	}

	itemMenu->popup(ui.view->mapToGlobal(pos));
}

