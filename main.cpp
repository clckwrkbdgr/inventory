#include <QtWidgets/QApplication>
#include <QtCore/QTranslator>

#include "mainwindow.h"

int main(int argc, char ** argv)
{
	QApplication app(argc, argv);
	app.setOrganizationName("clckwrkbdgr");
	app.setApplicationName("qinventory");

	QTranslator translator;
	QString translation_name = QString("%1_%2")
		.arg(app.applicationName())
		.arg(QLocale::system().name());
	if(!translator.load(translation_name, QApplication::applicationDirPath())) {
		QString system_path = "/usr/local/share/clckwrkbdgr-inventory";
		translator.load(translation_name, system_path);
	}
	app.installTranslator(&translator);

	MainWindow wnd;
	wnd.show();
	return app.exec();
}
