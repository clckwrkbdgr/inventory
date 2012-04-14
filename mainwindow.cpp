#include <QtCore/QSettings>
#include <QtGui/QMessageBox>
#include <QtGui/QFileDialog>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget * parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	tabs = new QTabBar();
	tabs->addTab(tr("Main"));
	tabs->addTab(tr("Print"));
	tabs->addTab(tr("Item types"));
	tabs->addTab(tr("Places"));
	tabs->addTab(tr("Persons"));

	QBoxLayout * box = static_cast<QBoxLayout*>(ui.centralwidget->layout());
	if(box) {
		box->insertWidget(0, tabs);
	}

	// Settings.
	QSettings settings;
	resize(settings.value("mainwindow/size", size()).toSize());
	move(settings.value("mainwindow/pos", pos()).toPoint());
	if(settings.value("mainwindow/maximized", false).toBool())
		setWindowState(Qt::WindowMaximized);

	QString databaseLocation = settings.value("database/location", "").toString();
	if(databaseLocation.isEmpty()) {
		databaseLocation = QFileDialog::getSaveFileName(this, tr("Database location"),
				QDir(QApplication::applicationDirPath()).absoluteFilePath("inventory.sqlite"),
				tr("SQLite3 database files (*.sqlite);;All files (*.*)")
				); 
	}

	// App logic.
	Inventory::Database::setDatabaseName(databaseLocation);
	if(!Inventory::Database::reopen()) {
		QMessageBox::critical(this, tr("Database"), tr("Cannot open database at path '%1'!").arg(Inventory::Database::databaseName()));
		exit(1);
	}
	inventoryModel = new Inventory::InventoryModel();
	printableModel = new Inventory::PrintableInventoryModel();
	itemTypesModel = new Inventory::ReferenceModel(Inventory::ReferenceModel::ITEM_TYPES);
	placesModel    = new Inventory::ReferenceModel(Inventory::ReferenceModel::PLACES);
	personsModel   = new Inventory::ReferenceModel(Inventory::ReferenceModel::PERSONS);
}

MainWindow::~MainWindow()
{
	// Settings.
	QSettings settings;
	settings.setValue("mainwindow/maximized",
			windowState().testFlag(Qt::WindowMaximized));
	if(!windowState().testFlag(Qt::WindowMaximized))
	{
		settings.setValue("mainwindow/size", size());
		settings.setValue("mainwindow/pos", pos());
	}
	settings.setValue("database/location", Inventory::Database::databaseName());

	// Database.
	Inventory::Database::close();
}
