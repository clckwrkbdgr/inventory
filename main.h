#pragma once

#include <QtGui/QWidget>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>

class QLineEdit;
class QPushButton;
class QListWidget;

class MainWindow : public QWidget {
	Q_OBJECT
	Q_DISABLE_COPY(MainWindow);
public:
	MainWindow(QWidget * parent = 0);
	virtual ~MainWindow() {}
public slots:
	void changeOldDBPath();
	void changeNewDBPath();
	bool run();
private:
	QLineEdit * editOld;
	QLineEdit * editNew;
	QPushButton * buttonOld;
	QPushButton * buttonNew;
	QPushButton * buttonRun;
	QListWidget * logList;

	void log(const QString & text);
	bool error(const QString & text);
	QString error(QSqlError e);

	bool recreateDatabase(QSqlDatabase db);
};

