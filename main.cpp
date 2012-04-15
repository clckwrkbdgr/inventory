#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

typedef QMainWindow MainWindow;

int main(int argc, char ** argv)
{
	QApplication app(argc, argv);
	
	MainWindow wnd;
	wnd.show();
	return app.exec();
}
