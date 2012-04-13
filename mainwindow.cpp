#include <QtCore/QSettings>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget * parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// Settings.
	QSettings settings;
	resize(settings.value("mainwindow/size", size()).toSize());
	move(settings.value("mainwindow/pos", pos()).toPoint());
	if(settings.value("mainwindow/maximized", false).toBool())
		setWindowState(Qt::WindowMaximized);
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

	// Database.
	Inventory::Database::close();
}
