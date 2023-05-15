#include <QtWidgets/QApplication>
#include <QtCore/QTranslator>

#include "mainwindow.h"

int main(int argc, char ** argv)
{
	QApplication app(argc, argv);
	app.setOrganizationName("clckwrkbdgr");
	app.setApplicationName("qinventory");

	QTranslator translator;
	translator.load(QString("%1_%2").arg(app.applicationName()).arg(QLocale::system().name()));
	app.installTranslator(&translator);

	MainWindow wnd;
	wnd.show();
	return app.exec();
}
