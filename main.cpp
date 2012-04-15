#include <QtDebug>
#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlQuery>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QListWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QFileDialog>

#include "main.h"

int main(int argc, char ** argv)
{
	QApplication app(argc, argv);
	
	MainWindow wnd;
	wnd.show();
	return app.exec();
}

MainWindow::MainWindow(QWidget * parent)
	: QWidget(parent)
{
	QVBoxLayout * vbox = new QVBoxLayout();
		QHBoxLayout * hboxOld = new QHBoxLayout();
			hboxOld->addWidget(editOld = new QLineEdit());
			editOld->setReadOnly(true);
			hboxOld->addWidget(buttonOld = new QPushButton(tr("Old database")));
			connect(buttonOld, SIGNAL(clicked()), this, SLOT(changeOldDBPath()));
		vbox->addLayout(hboxOld);
		QHBoxLayout * hboxNew = new QHBoxLayout();
			hboxNew->addWidget(editNew = new QLineEdit());
			editNew->setReadOnly(true);
			hboxNew->addWidget(buttonNew = new QPushButton(tr("New database")));
			connect(buttonNew, SIGNAL(clicked()), this, SLOT(changeNewDBPath()));
		vbox->addLayout(hboxNew);
		vbox->addWidget(buttonRun = new QPushButton(tr("Run")));
		buttonRun->setEnabled(false);
		connect(buttonRun, SIGNAL(clicked()), this, SLOT(run()));
		vbox->addWidget(logList = new QListWidget());
	setLayout(vbox);
}

void MainWindow::log(const QString & text)
{
	logList->addItem(text);
	logList->reset();
	QApplication::processEvents();
}

bool MainWindow::error(const QString & text)
{
	logList->addItem(tr("[ERROR] ") + text);
	return false;
}

void MainWindow::changeOldDBPath()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Old database..."));
	if(!path.isEmpty()) {
		editOld->setText(path);
	}
	buttonRun->setEnabled(!editOld->text().isEmpty() && !editNew->text().isEmpty());
}

void MainWindow::changeNewDBPath()
{
	QString path = QFileDialog::getSaveFileName(this, tr("New database..."));
	if(!path.isEmpty()) {
		editNew->setText(path);
	}
	buttonRun->setEnabled(!editOld->text().isEmpty() && !editNew->text().isEmpty());
}

bool MainWindow::recreateDatabase(QSqlDatabase db)
{
	log(tr("Cleaning new database..."));
	QSqlQuery q(db);
	if(!q.exec("DROP TABLE IF EXISTS History"))
		return error(tr("Dropping table failed!"));
	if(!q.exec("DROP TABLE IF EXISTS Inventory"))
		return error(tr("Dropping table failed!"));
	if(!q.exec("DROP TABLE IF EXISTS Places"))
		return error(tr("Dropping table failed!"));
	if(!q.exec("DROP TABLE IF EXISTS ItemTypes"))
		return error(tr("Dropping table failed!"));
	if(!q.exec("DROP TABLE IF EXISTS Persons"))
		return error(tr("Dropping table failed!"));

	log(tr("Creating new database structure..."));
	if(!q.exec(" CREATE TABLE IF NOT EXISTS "
			" Places ( "
			" id                INTEGER  PRIMARY KEY AUTOINCREMENT, "
			" name              TEXT NOT NULL DEFAULT '' "
			" ); "
			))
		return error(tr("Create table statement failed!"));
	if(!q.exec(" CREATE TABLE IF NOT EXISTS "
			" Persons ( "
			" id                INTEGER  PRIMARY KEY AUTOINCREMENT, "
			" name              TEXT NOT NULL DEFAULT '' "
			" ); "
			))
		return error(tr("Create table statement failed!"));
	if(!q.exec(" CREATE TABLE IF NOT EXISTS "
			" ItemTypes ( "
			" id                INTEGER  PRIMARY KEY AUTOINCREMENT, "
			" name              TEXT NOT NULL DEFAULT '' "
			" ); "
			))
		return error(tr("Create table statement failed!"));
	if(!q.exec(" CREATE TABLE IF NOT EXISTS "
			" Inventory ( "
			" id                INTEGER  PRIMARY KEY AUTOINCREMENT, "
			" itemType          INTEGER  NOT NULL, "
			" place             INTEGER  NOT NULL, "
			" responsiblePerson INTEGER  NOT NULL, "
			" name              TEXT     NOT NULL DEFAULT '', "
			" inn               INTEGER  NOT NULL DEFAULT 0, "
			" writtenOff        INTEGER  NOT NULL DEFAULT 0, "
			" underRepair       INTEGER  NOT NULL DEFAULT 0, "
			" checked           INTEGER  NOT NULL DEFAULT 0, "
			" note              TEXT     NOT NULL DEFAULT '', "
			" FOREIGN KEY (itemType)          REFERENCES ItemTypes(id), "
			" FOREIGN KEY (place)             REFERENCES Place    (id), "
			" FOREIGN KEY (responsiblePerson) REFERENCES Persons  (id) "
			" ); "
			))
		return error(tr("Create table statement failed!"));
	if(!q.exec(" CREATE TABLE IF NOT EXISTS "
			" History ( "
			"   id                INTEGER  PRIMARY KEY AUTOINCREMENT, "
			"   item              INTEGER  NOT NULL, "
			"   changeTime        TEXT     NOT NULL DEFAULT '', "
			"   field             INTEGER  NOT NULL DEFAULT 10, "
			"   oldValue          TEXT     NOT NULL DEFAULT '', "
			"   newValue          TEXT     NOT NULL DEFAULT '', "
			"   FOREIGN KEY (item) REFERENCES Inventory(id) "
			" ); "
			))
		return error(tr("Create table statement failed!"));

	return true;
}

struct InterfaceLocker {
	QWidget * widget;
	InterfaceLocker(QWidget * w) : widget(w) {
		widget->setEnabled(false);
	}
	~InterfaceLocker() {
		widget->setEnabled(true);
	}
};

enum { HISTORY_TYPE, HISTORY_PLACE, HISTORY_PERSON, HISTORY_NAME, HISTORY_INN,
	HISTORY_WRITTEN_OFF, HISTORY_UNDER_REPAIR, HISTORY_CHECKED, HISTORY_NOTE, HISTORY_FIELD_COUNT };
enum ItemField {InvalidField = 0, ItemTypeField = 1, NameField = 2, NoteField = 3, InnField = 4, PlaceField = 5, ActiveField = 6};

bool MainWindow::run()
{
	QString oldPath = editOld->text();
	QString newPath = editNew->text();

	InterfaceLocker runLocker(buttonRun);
	InterfaceLocker oldLocker(buttonOld);
	InterfaceLocker newLocker(buttonNew);

	logList->clear();
	log(tr("Start converting."));
	if(editOld->text().isEmpty())
		return error(tr("Old database path is empty!"));
	if(editNew->text().isEmpty())
		return error(tr("New database path is empty!"));

	log(tr("Opening databases..."));
	QSqlDatabase dbOld = QSqlDatabase::addDatabase("QSQLITE", "old");
	dbOld.setDatabaseName(oldPath);
	if(!dbOld.open())
		return error(tr("Cannot open database file %1").arg(oldPath));
	
	QSqlDatabase dbNew = QSqlDatabase::addDatabase("QSQLITE", "new");
	dbNew.setDatabaseName(newPath);
	if(!dbNew.open())
		return error(tr("Cannot open database file %1").arg(newPath));

	if(!recreateDatabase(dbNew))
		return false;

	int personId;
	QMap<int, int> itemTypeMap;
	QMap<int, int> placeMap;
	QMap<int, int> itemMap;
	QSqlQuery q(dbNew);
	QSqlQuery select(dbOld);

	log(tr("Adding default person..."));
	if(!q.exec("INSERT INTO Persons (name) VALUES ('None');"))
		return error(tr("Insert statement failed!"));
	personId = q.lastInsertId().toInt();

	log(tr("Converting item types..."));
	if(!select.exec("SELECT id, name FROM ItemTypes;"))
		return error(tr("Select statement failed!"));
	while(select.next()) {
		int id = select.value(0).toInt();
		QString name = select.value(1).toString();

		q.prepare("INSERT INTO ItemTypes (name) VALUES (:name);");
		q.bindValue(":name", name);
		if(!q.exec())
			return error(tr("Insert statement failed!"));
		itemTypeMap[id] = q.lastInsertId().toInt();
	}
	qDebug() << itemTypeMap;

	log(tr("Converting places..."));
	if(!select.exec("SELECT id, name FROM Places;"))
		return error(tr("Select statement failed!"));
	while(select.next()) {
		int id = select.value(0).toInt();
		QString name = select.value(1).toString();

		q.prepare("INSERT INTO Places (name) VALUES (:name);");
		q.bindValue(":name", name);
		if(!q.exec())
			return error(tr("Insert statement failed!"));
		placeMap[id] = q.lastInsertId().toInt();
	}
	qDebug() << placeMap;

	log(tr("Converting items..."));
	if(!select.exec("SELECT id, idItemType, name, note, inn, idPlace, active FROM Items;"))
		return error(tr("Select statement failed!"));
	while(select.next()) {
		int id         = select.value(0).toInt();
		int idItemType = select.value(1).toInt();
		QString name   = select.value(2).toString();
		QString note   = select.value(3).toString();
		int inn        = select.value(4).toInt();
		int idPlace    = select.value(5).toInt();
		int active     = select.value(6).toInt();
		qDebug() << id << idItemType << name << note << inn << idPlace << active;

		q.prepare("INSERT INTO Inventory (itemType, place, responsiblePerson, name, inn, writtenOff, note) VALUES "
				" (:itemType, :place, :person, :name, :inn, :writtenOff, :note);");
		q.bindValue(":itemType",   itemTypeMap[idItemType]);
		q.bindValue(":place",      placeMap[idPlace]);
		q.bindValue(":person",     personId);
		q.bindValue(":name",       name);
		q.bindValue(":inn",        inn);
		q.bindValue(":writtenOff", 1 - active);
		q.bindValue(":note",       note);
		qDebug() << "\t" << itemTypeMap[idItemType] << placeMap[idPlace] << personId << name << inn << 1 - active << note;
		if(!q.exec())
			return error(tr("Insert statement failed!"));
		itemMap[id] = q.lastInsertId().toInt();
	}

	log(tr("Converting history..."));
	if(!select.exec("SELECT id, eventDateTime, idItem, changedField, oldValue, newValue FROM History;"))
		return error(tr("Select statement failed!"));
	while(select.next()) {
		QString dt = select.value(1).toString();
		int idItem = select.value(2).toInt();
		int changedField = select.value(3).toInt();
		QString oldValue = select.value(4).toString();
		if(oldValue.isNull())
			oldValue = "";
		QString newValue = select.value(5).toString();
		if(newValue.isNull())
			newValue = "";

		int field = HISTORY_NOTE;
		switch(changedField) {
			case ItemTypeField: field = HISTORY_TYPE; newValue = itemTypeMap[newValue.toInt()]; newValue = itemTypeMap[newValue.toInt()]; break;
			case NameField    : field = HISTORY_NAME; break;
			case NoteField    : field = HISTORY_NOTE; break;
			case InnField     : field = HISTORY_INN; break;
			case PlaceField   : field = HISTORY_PLACE; oldValue = placeMap[oldValue.toInt()]; newValue = placeMap[newValue.toInt()]; break;
			case ActiveField  : field = HISTORY_WRITTEN_OFF; oldValue = (oldValue == "1") ? "0" : "1"; newValue = (newValue == "1") ? "0" : "1"; break;
			default: field = HISTORY_NOTE;
		}

		q.prepare("INSERT INTO History (item, changeTime, field, oldValue, newValue) VALUES "
				" (:item, :changeTime, :field, :oldValue, :newValue);");
		q.bindValue(":item",       itemMap[idItem]);
		q.bindValue(":changeTime", dt);
		q.bindValue(":field",      field);
		q.bindValue(":oldValue",   oldValue);
		q.bindValue(":newValue",   newValue);
		if(!q.exec()) {
			qDebug() << error(q.lastError());
			return error(tr("Insert statement failed!"));
		}
	}

	log(tr("Finished."));
	return true;
}

typedef QMap<QSqlError::ErrorType, QString> ErrorMap;

ErrorMap getErrorTypes()
{
	ErrorMap map;
	map[QSqlError::NoError] = "No error";
	map[QSqlError::ConnectionError] = "Connection error";
	map[QSqlError::StatementError] = "SQL statement syntax error";
	map[QSqlError::TransactionError] = "Transaction failed";
	map[QSqlError::UnknownError] = "Unknown";
	return map;
}

QString MainWindow::error(QSqlError e)
{
	static ErrorMap errorTypes = getErrorTypes();
	return tr("Type: %1\nNumber: %2\nDriver: %3\nDatabase: %4").
		arg(errorTypes[e.type()]).
		arg(e.number()).
		arg(e.driverText()).
		arg(e.databaseText());
}
