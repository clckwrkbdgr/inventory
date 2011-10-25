#include <QtDebug>
#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtCore/QVariant>

#include "inventory.h"

namespace Database {
	InventoryViewFilter lastFilter;

	Inventory *inventory = NULL;
	const QStringList createQuery = QStringList()
		<< "DROP TABLE IF EXISTS Places;"
		<< "DROP TABLE IF EXISTS ItemTypes;"
		<< "DROP TABLE IF EXISTS Items;"
		<< "DROP TABLE IF EXISTS History;"
		<< "DROP TABLE IF EXISTS Log;"
		<< "CREATE TABLE Places ("
		"	id INTEGER PRIMARY KEY,"
		"	name TEXT NOT NULL"
		");"
		<< "CREATE TABLE ItemTypes ("
		"	id INTEGER PRIMARY KEY,"
		"	name TEXT NOT NULL"
		");"
		<< "CREATE TABLE Items ("
		"	id INTEGER PRIMARY KEY,"
		"	idItemType INTEGER NOT NULL,"
		"	name TEXT NOT NULL,"
		"	note TEXT NOT NULL,"
		"	inn INTEGER NULL,"
		"	idPlace INTEGER NOT NULL,"
		"	active BOOLEAN NOT NULL,"
		""
		"	FOREIGN KEY(idItemType) REFERENCES ItemTypes(id),"
		"	FOREIGN KEY(idPlace) REFERENCES Places(id)"
		");"
		<< "CREATE TABLE History ("
		"	id INTEGER PRIMARY KEY,"
		"	eventDateTime TEXT NOT NULL,"
		"	idItem INTEGER NOT NULL,"
		"	changedField INTEGER NOT NULL,"
		"	oldValue TEXT NOT NULL,"
		"	newValue TEXT NOT NULL,"
		""
		"	FOREIGN KEY(idItem) REFERENCES Items(id)"
		");"
		<< "CREATE TABLE Log ("
		"	id INTEGER PRIMARY KEY,"
		"	logDateTime TEXT NOT NULL,"
		"	logText TEXT NOT NULL"
		");";
	QString fileName;
};

//----------------------------------------------------------------------------------------------------

Item::Item(int id) {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT COUNT(*) FROM Items WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	if(query.value(0).toInt() != 1)
		throw InvalidIdException();

	this->id = id;
}

Item::Item(const Item &other) {
	this->id = other.id;
}

const Item& Item::operator=(const Item &other) {
	this->id = other.id;
	return *this;
}

Item::~Item() {

}

int Item::getId() const {
	return id;
}

ItemType Item::itemType() const {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT idItemType FROM Items WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	return ItemType(query.value(0).toInt());
}

QString Item::name() const {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT name FROM Items WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	return query.value(0).toString();
}

QString Item::note() const {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT note FROM Items WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	return query.value(0).toString();

}

int Item::inn() const {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT inn FROM Items WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	return query.value(0).toInt();

}

bool Item::innExists() const {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT COUNT(*) FROM Items WHERE id = :id AND inn IS NOT NULL");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	return query.value(0).toInt() == 1;

}

Place Item::place() const {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT idPlace FROM Items WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	return Place(query.value(0).toInt());

}

bool Item::isActive() const {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT active FROM Items WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	return query.value(0).toInt() == 1;

}

void Item::setItemType(const ItemType &value) {
	if(!isActive())
		throw InactiveException();
	ItemType oldValue = itemType();
	if(value.getId() == oldValue.getId())
		return;

	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("UPDATE Items SET idItemType = :idItemType WHERE id = :id");
	query.bindValue(":id", id);
	query.bindValue(":idItemType", value.getId());
	if(!query.exec())
		throw DBErrorException(query);
}

void Item::setName(const QString &value) {
	if(!isActive())
		throw InactiveException();
	if(value.isEmpty())
		throw EmptyNameException();
	QString oldValue = name();
	if(value == oldValue)
		return;

	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("UPDATE Items SET name = :name WHERE id = :id");
	query.bindValue(":id", id);
	query.bindValue(":name", value);
	if(!query.exec())
		throw DBErrorException(query);
}

void Item::setNote(const QString &value) {
	if(!isActive())
		throw InactiveException();
	QString oldValue = note();
	if(value == oldValue)
		return;

	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("UPDATE Items SET note = :note WHERE id = :id");
	query.bindValue(":id", id);
	query.bindValue(":note", value);
	if(!query.exec())
		throw DBErrorException(query);

	addHistory(History::NoteField, oldValue, value);
}

void Item::setInn(int value) {
	if(!isActive())
		throw InactiveException();
	QString oldValue;
	if(innExists()) {
		if(value == inn())
			return;
		oldValue = QString::number(inn());
	} else
		oldValue = QObject::tr("Empty");

	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("UPDATE Items SET inn = :inn WHERE id = :id");
	query.bindValue(":id", id);
	query.bindValue(":inn", value);
	if(!query.exec())
		throw DBErrorException(query);

	addHistory(History::InnField, oldValue, QString::number(value));
}

void Item::setPlace(const Place &value) {
	if(!isActive())
		throw InactiveException();
	Place oldValue = place();
	if(value.getId() == oldValue.getId())
		return;

	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("UPDATE Items SET idPlace = :idPlace WHERE id = :id");
	query.bindValue(":id", id);
	query.bindValue(":idPlace", value.getId());
	if(!query.exec())
		throw DBErrorException(query);

	addHistory(History::PlaceField, oldValue.name(), value.name());
}

void Item::deactivate() {
	if(!isActive())
		return;

	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("UPDATE Items SET active = 0 WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	addHistory(History::ActiveField, QObject::tr("Active"), QObject::tr("Inactive"));
}

bool Item::operator==(const Item &other) const {
	return (other.id == this->id);
}

Item Item::add(const ItemType &newItemType, const QString &newName, const Place &newPlace) {
	if(newName.isEmpty())
		throw EmptyNameException();

	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("INSERT INTO Items VALUES(NULL, :idItemType, :name, '', NULL, :idPlace, 1)");
	query.bindValue(":name", newName);
	query.bindValue(":idItemType", newItemType.getId());
	query.bindValue(":idPlace", newPlace.getId());
	if(!query.exec())
		throw DBErrorException(query);

	if(!query.exec(QString("SELECT MAX(id) FROM Items")))
		throw DBErrorException(query);
	query.next();
	int id = query.value(0).toInt();

	Log::add(QObject::tr("Items #%1 of itemType '%3' in place '%4' is added with name '%2'").
			arg(id).arg(newName).
			arg(newItemType.name()).
			arg(newPlace.name()));
	return Item(id);
}

Item Item::add(const QString &newName) {
	if(newName.isEmpty())
		throw EmptyNameException();

	// Get top (as random) values of itemType and place.
	int itemTypeId = 0;
	if(Database::lastFilter.useItemType)
		itemTypeId = Database::lastFilter.itemTypeId;
	else {
		QSqlQuery query(QSqlDatabase::database(Database::fileName));
		if(!query.exec("SELECT COUNT(*) FROM ItemTypes"))
			throw DBErrorException(query);
		query.next();
		if(query.value(0).toInt() == 0)
			ItemType::add(QObject::tr("New item type"));

		if(!query.exec("SELECT id FROM ItemTypes"))
			throw DBErrorException(query);
		query.next();
		itemTypeId = query.value(0).toInt();
	}

	int placeId = 0;
	if(Database::lastFilter.usePlace)
		placeId = Database::lastFilter.placeId;
	else {
		QSqlQuery query(QSqlDatabase::database(Database::fileName));
		if(!query.exec("SELECT COUNT(*) FROM Places"))
			throw DBErrorException(query);
		query.next();
		if(query.value(0).toInt() == 0)
			ItemType::add(QObject::tr("New place"));

		if(!query.exec("SELECT id FROM Places"))
			throw DBErrorException(query);
		query.next();
		placeId = query.value(0).toInt();
	}

	// Query.
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("INSERT INTO Items VALUES(NULL, :idItemType, :name, '', NULL, :idPlace, 1)");
	query.bindValue(":name", newName);
	query.bindValue(":idItemType", itemTypeId);
	query.bindValue(":idPlace", placeId);
	if(!query.exec())
		throw DBErrorException(query);

	if(!query.exec(QString("SELECT MAX(id) FROM Items")))
		throw DBErrorException(query);
	query.next();
	int id = query.value(0).toInt();

	Log::add(QObject::tr("Item #%1 of itemType '%3' in place '%4' is added with name '%2'").
			arg(id).arg(newName).
			arg(ItemType(itemTypeId).name()).
			arg(Place(placeId).name()));
	return Item(id);
}

void Item::remove(int id) {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("DELETE FROM History WHERE idItem = :idItem");
	query.bindValue(":idItem", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.prepare("DELETE FROM Items WHERE id = :idItem");
	query.bindValue(":idItem", id);
	if(!query.exec())
		throw DBErrorException(query);

	Log::add(QObject::tr("Item #%1 was removed from database.").
			arg(id));
}

void Item::addHistory(int changedField, const QString &oldValue, const QString &newValue) {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("INSERT INTO History VALUES(NULL, datetime('now'), :idItem, :changedField, :oldValue, :newValue)");
	query.bindValue(":idItem", id);
	query.bindValue(":changedField", changedField);
	query.bindValue(":oldValue", oldValue);
	query.bindValue(":newValue", newValue);
	if(!query.exec())
		throw DBErrorException(query);
}

//----------------------------------------------------------------------------------------------------

Place::Place(int id) {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT COUNT(*) FROM Places WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	if(query.value(0).toInt() != 1)
		throw InvalidIdException();

	this->id = id;
}

Place::Place(const Place &other) {
	this->id = other.id;
}

const Place& Place::operator=(const Place &other) {
	this->id = other.id;
	return *this;
}

Place::~Place() {

}

int Place::getId() const {
	return id;
}

QString Place::name() const {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT name FROM Places WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	return query.value(0).toString();
}

void Place::setName(const QString &value) {
	if(value.isEmpty())
		throw EmptyNameException();
	if(value == name())
		return;

	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("UPDATE Places SET name = :name WHERE id = :id");
	query.bindValue(":id", id);
	query.bindValue(":name", value);
	if(!query.exec())
		throw DBErrorException(query);

	Log::add(QObject::tr("Place #%1 changes name to '%2'").arg(id).arg(value));
}

bool Place::operator==(const Place &other) const {
	return (other.id == this->id);
}

Place Place::add(const QString &newName) {
	if(newName.isEmpty())
		throw EmptyNameException();

	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("INSERT INTO Places VALUES(NULL, :name)");
	query.bindValue(":name", newName);
	if(!query.exec())
		throw DBErrorException(query);

	if(!query.exec(QString("SELECT MAX(id) FROM Places")))
		throw DBErrorException(query);
	query.next();
	int id = query.value(0).toInt();

	Log::add(QObject::tr("Place #%1 is added with name '%2'").arg(id).arg(newName));
	return Place(id);
}

bool Place::remove(int id) {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT COUNT(*) FROM Items WHERE idPlace = :idPlace");
	query.bindValue(":idPlace", id);
	if(!query.exec())
		throw DBErrorException(query);
	query.next();
	if(query.value(0).toInt() != 0)
		return false;

	query.prepare("DELETE FROM Places WHERE id = :idPlace");
	query.bindValue(":idPlace", id);
	if(!query.exec())
		throw DBErrorException(query);

	Log::add(QObject::tr("Place #%1 was removed from database.").
			arg(id));
	return true;
}

//----------------------------------------------------------------------------------------------------

ItemType::ItemType(int id) {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT COUNT(*) FROM ItemTypes WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	if(query.value(0).toInt() != 1)
		throw InvalidIdException();

	this->id = id;
}

ItemType::ItemType(const ItemType &other) {
	this->id = other.id;
}

const ItemType& ItemType::operator=(const ItemType &other) {
	this->id = other.id;
	return *this;
}

ItemType::~ItemType() {

}

int ItemType::getId() const {
	return id;
}

QString ItemType::name() const {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT name FROM ItemTypes WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	return query.value(0).toString();
}

void ItemType::setName(const QString &value) {
	if(value.isEmpty())
		throw EmptyNameException();
	if(value == name())
		return;

	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("UPDATE ItemTypes SET name = :name WHERE id = :id");
	query.bindValue(":id", id);
	query.bindValue(":name", value);
	if(!query.exec())
		throw DBErrorException(query);

	Log::add(QObject::tr("ItemType #%1 changes name to '%2'").arg(id).arg(value));
}

bool ItemType::operator==(const ItemType &other) const {
	return (other.id == this->id);
}

ItemType ItemType::add(const QString &newName) {
	if(newName.isEmpty())
		throw EmptyNameException();

	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("INSERT INTO ItemTypes VALUES(NULL, :name)");
	query.bindValue(":name", newName);
	if(!query.exec())
		throw DBErrorException(query);

	if(!query.exec(QString("SELECT MAX(id) FROM ItemTypes")))
		throw DBErrorException(query);
	query.next();
	int id = query.value(0).toInt();

	Log::add(QObject::tr("ItemType #%1 is added with name '%2'").arg(id).arg(newName));
	return ItemType(id);
}

bool ItemType::remove(int id) {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT COUNT(*) FROM Items WHERE idItemType = :idItemType");
	query.bindValue(":idItemType", id);
	if(!query.exec())
		throw DBErrorException(query);
	query.next();
	if(query.value(0).toInt() != 0)
		return false;

	query.prepare("DELETE FROM ItemTypes WHERE id = :idItemType");
	query.bindValue(":idItemType", id);
	if(!query.exec())
		throw DBErrorException(query);

	Log::add(QObject::tr("Item type #%1 was removed from database.").
			arg(id));
	return true;
}

//----------------------------------------------------------------------------------------------------

History::History(int id) {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT COUNT(*) FROM History WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	if(query.value(0).toInt() != 1)
		throw InvalidIdException();

	this->id = id;
}

History::History(const History &other) {
	this->id = other.id;
}

const History& History::operator=(const History &other) {
	this->id = other.id;
	return *this;
}

History::~History() {

}

int History::getId() const {
	return id;
}

Item History::item() const {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT idItem FROM History WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	return Item(query.value(0).toInt());
}

QDateTime History::dateTime() const {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT eventDateTime FROM History WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	return query.value(0).toDateTime();
}

History::ItemField History::changedField() const {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT changedField FROM History WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	switch(query.value(0).toInt()) {
		case ItemTypeField:  return ItemTypeField;
		case NameField: return NameField;
		case NoteField: return NoteField;
		case InnField: return InnField;
		case PlaceField: return PlaceField;
		case ActiveField: return ActiveField;
		default: return InvalidField;
	}
}

QString History::oldValue() const {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT oldValue FROM History WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	return query.value(0).toString();
}

QString History::newValue() const {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT newValue FROM History WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	return query.value(0).toString();
}

//----------------------------------------------------------------------------------------------------

Log::Log(int id) {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT COUNT(*) FROM Log WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	if(query.value(0).toInt() != 1)
		throw InvalidIdException();

	this->id = id;
}

Log::Log(const Log &other) {
	this->id = other.id;
}

const Log& Log::operator=(const Log &other) {
	this->id = other.id;
	return *this;
}

Log::~Log() {

}

int Log::getId() const {
	return id;
}

QDateTime Log::dateTime() const {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT logDateTime FROM Log WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	return query.value(0).toDateTime();
}

QString Log::text() const {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT logText FROM Log WHERE id = :id");
	query.bindValue(":id", id);
	if(!query.exec())
		throw DBErrorException(query);

	query.next();
	return query.value(0).toString();
}

Log Log::add(const QString &newText) {
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("INSERT INTO Log VALUES(NULL, datetime('now'), :logText)");
	query.bindValue(":logText", newText);
	if(!query.exec())
		throw DBErrorException(query);

	if(!query.exec(QString("SELECT MAX(id) FROM Log")))
		throw DBErrorException(query);
	query.next();
	return Log(query.value(0).toInt());
}

//----------------------------------------------------------------------------------------------------

QList<Item> Inventory::items() const {
	QList<Item> result;
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	if(!query.exec("SELECT id FROM Items"))
		throw DBErrorException(query);

	while(query.next())
		result << Item(query.value(0).toInt());

	return result;
}

QList<Item> Inventory::filteredItems(const InventoryViewFilter &filter) const {
	Database::lastFilter = filter;

	QStringList whereList;
	if(filter.usePlace) whereList << "idPlace = :idPlace";
	if(filter.useItemType) whereList << "idItemType = :idItemType";
	if(filter.useActivity) whereList << "active = :active";
	QString where;
	if(!whereList.isEmpty()) {
		where = QString(" WHERE ") + whereList.join(" AND ");
	}

	QList<Item> result;
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare(QString("SELECT id FROM Items") + where);

	if(filter.usePlace) query.bindValue(":idPlace", filter.placeId);
	if(filter.useItemType) query.bindValue(":idItemType", filter.itemTypeId);
	if(filter.useActivity) query.bindValue(":active", (filter.active) ? 1 : 0);

	if(!query.exec())
		throw DBErrorException(query);

	while(query.next())
		result << Item(query.value(0).toInt());

	return result;
}

QList<Item> Inventory::filteredItems(const Place *place, const ItemType *itemType, const bool* activity) const {
	QStringList whereList;
	if(place) whereList << "idPlace = :idPlace";
	if(itemType) whereList << "idItemType = :idItemType";
	if(activity) whereList << "active = :active";
	QString where;
	if(!whereList.isEmpty()) {
		where = QString(" WHERE ") + whereList.join(" AND ");
	}

	QList<Item> result;
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare(QString("SELECT id FROM Items") + where);

	if(place) query.bindValue(":idPlace", place->getId());
	if(itemType) query.bindValue(":idItemType", itemType->getId());
	if(activity) query.bindValue(":active", (*activity) ? 1 : 0);

	if(!query.exec())
		throw DBErrorException(query);

	while(query.next())
		result << Item(query.value(0).toInt());

	return result;
}

QList<Item> Inventory::itemsByPlace(const Place &place) const {
	QList<Item> result;
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT id FROM Items WHERE idPlace = :idPlace");
	query.bindValue(":idPlace", place.getId());
	if(!query.exec())
		throw DBErrorException(query);

	while(query.next())
		result << Item(query.value(0).toInt());

	return result;
}

QList<Item> Inventory::itemsByItemType(const ItemType &itemType) const {
	QList<Item> result;
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT id FROM Items WHERE idItemType = :idItemType");
	query.bindValue(":idItemType", itemType.getId());
	if(!query.exec())
		throw DBErrorException(query);

	while(query.next())
		result << Item(query.value(0).toInt());

	return result;
}

QList<Item> Inventory::itemsByActivity(bool active) const {
	QList<Item> result;
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT id FROM Items WHERE active = :active");
	query.bindValue(":active", active ? 1 : 0);
	if(!query.exec())
		throw DBErrorException(query);

	while(query.next())
		result << Item(query.value(0).toInt());

	return result;
}

QList<Place> Inventory::places() const {
	QList<Place> result;
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	if(!query.exec("SELECT id, name FROM Places ORDER BY name ASC"))
		throw DBErrorException(query);

	while(query.next())
		result << Place(query.value(0).toInt());

	return result;
}

QList<ItemType> Inventory::itemTypes() const {
	QList<ItemType> result;
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	if(!query.exec("SELECT id, name FROM ItemTypes ORDER BY name ASC"))
		throw DBErrorException(query);

	while(query.next())
		result << ItemType(query.value(0).toInt());

	return result;
}

QList<Log> Inventory::logByTime(QDateTime startTime, QDateTime stopTime) const {
	if(startTime > stopTime) {
		QDateTime tmp = startTime;
		startTime = stopTime;
		stopTime = tmp;
	}

	QList<Log> result;
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT id FROM Log WHERE logDateTime BETWEEN datetime(:startTime) AND datetime(:stopTime)");
	query.bindValue(":startTime", startTime.toString(Qt::ISODate));
	query.bindValue(":stopTime", stopTime.toString(Qt::ISODate));
	if(!query.exec())
		throw DBErrorException(query);

	while(query.next())
		result << Log(query.value(0).toInt());

	return result;
}

QList<History> Inventory::historyOf(const Item &item) const {
	QList<History> result;
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare("SELECT id FROM History WHERE idItem = :idItem");
	query.bindValue(":idItem", item.getId());
	if(!query.exec())
		throw DBErrorException(query);

	while(query.next())
		result << History(query.value(0).toInt());

	return result;
}

QSqlQuery Inventory::getQuery(int sortingColumn, Qt::SortOrder order) {
	QStringList whereList;
	if(Database::lastFilter.usePlace) whereList << " idPlace = :idPlace ";
	if(Database::lastFilter.useItemType) whereList << " idItemType = :idItemType ";
	if(Database::lastFilter.useActivity) whereList << " active = :active ";
	QString whereClause;
	if(!whereList.isEmpty()) {
		whereClause = QString(" WHERE ") + whereList.join(" AND ");
	}

	QString orderByClause = (sortingColumn < 0) ? ("") : QString(" ORDER BY %1 %2 ").
		arg(sortingColumn + 1).
		arg( (order == Qt::AscendingOrder) ? " ASC " : " DESC " );

	QList<Item> result;
	QSqlQuery query(QSqlDatabase::database(Database::fileName));
	query.prepare(QString("SELECT T.name, I.name, COUNT(*), I.inn, P.name, "
				" REPLACE(REPLACE(I.active, '0', '%1'), '1', '') "
				" FROM Items AS I "
				" INNER JOIN ItemTypes AS T ON T.id = I.idItemType "
				" INNER JOIN Places AS P ON P.id = I.idPlace ").arg(QObject::tr("Inactive")) +
			whereClause +
			" GROUP BY T.name, I.name, I.inn, P.name, I.active " +
			orderByClause);

	if(Database::lastFilter.usePlace) query.bindValue(":idPlace", Database::lastFilter.placeId);
	if(Database::lastFilter.useItemType) query.bindValue(":idItemType", Database::lastFilter.itemTypeId);
	if(Database::lastFilter.useActivity) query.bindValue(":active", (Database::lastFilter.active) ? 1 : 0);
	query.exec();
	qDebug() << query.executedQuery();

	return query;
}

bool Inventory::isOpen() {
	return QSqlDatabase::database(Database::fileName).isOpen();
}

void Inventory::open(const QString &databaseFileName) {
	Database::fileName = databaseFileName;

	bool databaseFileNotExists = !QFile::exists(databaseFileName);

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", databaseFileName);
	db.setDatabaseName(databaseFileName);
	if(!db.open())
		throw DBOpenErrorException();

	if(databaseFileNotExists) {
		QSqlQuery query(QSqlDatabase::database(Database::fileName));
		foreach(QString text, Database::createQuery)
			if(!query.exec(text))
				throw DBErrorException(query);

		Log::add(QObject::tr("Database has been created @ %1.").arg(databaseFileName));
	}

	Log::add(QObject::tr("Database is open @ %1.").arg(databaseFileName));

	Database::lastFilter.usePlace = Database::lastFilter.useItemType = Database::lastFilter.useActivity = false;
}

void Inventory::close() {
	Log::add(QObject::tr("Database is closed."));
	QSqlDatabase::database(Database::fileName).close();
	QSqlDatabase::removeDatabase(Database::fileName);
}

Inventory* Inventory::instance() {
	if(Database::inventory == NULL)
		Database::inventory = new Inventory();

	return Database::inventory;
}

InventoryViewFilter Inventory::getLastFilter() {
	return Database::lastFilter;
}
