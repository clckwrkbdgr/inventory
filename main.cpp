#include <QtGui/QApplication>

#include "mainwindow.h"

int main(int argc, char ** argv)
{
	QApplication app(argc, argv);
	app.setOrganizationName("antifin");
	app.setApplicationName("inventory");

	MainWindow wnd;
	wnd.show();
	return app.exec();
}
