#include <QtDebug>
#include <QtGui/QApplication>
#include <QtCore/QTranslator>

#include "mainwindow.h"

int main(int argc, char **argv) {
	QApplication app(argc, argv);
	app.setOrganizationName("antifin");
	app.setApplicationName("inventory");

	QLocale locale = QLocale::system();
	QTranslator translator;
	translator.load(app.applicationName() + "_" + locale.name());
	app.installTranslator(&translator);

	MainWindow wnd;
	wnd.show();
	return app.exec();
}
