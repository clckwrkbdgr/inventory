#include <QtDebug>

#include "models.h"

namespace Inventory {

QString Database::databaseName = "";

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

bool Database::reopen()
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "inventory");
	db.setDatabaseName(databaseName);
	return db.open();
}

void Database::close()
{
	QSqlDatabase db = QSqlDatabase::database("inventory");
	db.close();
	QSqlDatabase::removeDatabase("inventory");
}

QSqlDatabase Database::db()
{
	return QSqlDatabase::database("inventory");
}

QString Database::error(QSqlError e)
{
	static ErrorMap errorTypes = getErrorTypes();
	return QObject::tr("Type: %1\nNumber: %2\nDriver: %3\nDatabase: %4").
		arg(errorTypes[e.type()]).
		arg(e.number()).
		arg(e.driverText()).
		arg(e.databaseText());
}

void Database::setDatabaseName(const QString & newName)
{
	databaseName = newName;
	reopen();
}

bool Database::query(const QString & prepared, const Placeholders & placeholders)
{
	QSqlQuery query(Database::db());
	query.prepare(prepared);
	foreach(QString key, placeholders.keys()) {
		query.bindValue(key, placeholders[key]);
	}
	bool ok = query.exec();
	return ok;
}

QSqlQuery Database::select(const QString & prepared, const Placeholders & placeholders)
{
	QSqlQuery query(Database::db());
	query.prepare(prepared);
	foreach(QString key, placeholders.keys()) {
		query.bindValue(key, placeholders[key]);
	}
	query.exec();
	return query;
}

}

namespace Inventory { // InventoryModel

InventoryModel::InventoryModel(QObject * parent)
	: QAbstractTableModel(parent)
{
	Database::query(" CREATE TABLE IF NOT EXISTS "
			" Inventory ( "
			" id                INTEGER  PRIMARY KEY, "
			" itemType          INTEGER  NOT NULL, "
			" place             INTEGER  NOT NULL, "
			" responsiblePerson INTEGER  NOT NULL, "
			" name              TEXT     NOT NULL DEFAULT '', "
			" inn               INTEGER           DEFAULT NULL, "
			" writtenOff        INTEGER  NOT NULL DEFAULT 0, "
			" underRepair       INTEGER  NOT NULL DEFAULT 0, "
			" checked           INTEGER  NOT NULL DEFAULT 0, "
			" note              TEXT     NOT NULL DEFAULT '', "
			" FOREIGN KEY (itemType)          REFERENCES ItemTypes(id), "
			" FOREIGN KEY (place)             REFERENCES Place    (id), "
			" FOREIGN KEY (responsiblePerson) REFERENCES Persons  (id) "
			" ); "
			);
	update();
}

void InventoryModel::update()
{
	QSqlQuery query = Database::select(
			" SELECT Inventory.id, "
			"   itemType, ItemTypes.name, "
			"   place, Places.name, "
			"   responsiblePerson, Persons.name, "
			"   Inventory.name, "
			"   inn, writtenOff, underRepair, checked, note "
			" FROM Inventory, ItemTypes, Places, Persons "
			" WHERE Inventory.itemType = ItemTypes.id "
			" AND Inventory.place = Places.id "
			" AND Inventory.responsiblePerson = Persons.id "
			" ; "
			);
	items.clear();
	while(query.next()) {
		Item item;
		item.id                  = query.value( 0).toInt();
		item.itemTypeId          = query.value( 1).toInt();
		item.itemType            = query.value( 2).toString();
		item.placeId             = query.value( 3).toInt();
		item.place               = query.value( 4).toString();
		item.responsiblePersonId = query.value( 5).toInt();
		item.responsiblePerson   = query.value( 6).toString();
		item.name                = query.value( 7).toString();
		item.innIsNotNull        = query.value( 8).isValid() && !query.value(8).isNull();
		item.inn                 = query.value( 8).toInt();
		item.writtenOff          = query.value( 9).toBool();
		item.underRepair         = query.value(10).toBool();
		item.checked             = query.value(11).toBool();
		item.note                = query.value(12).toString();
		items << item;
	}
}

enum { ITEM_TYPE, ITEM_PLACE, ITEM_PERSON, ITEM_NAME, ITEM_INN, ITEM_WRITTEN_OFF, ITEM_UNDER_REPAIR, ITEM_CHECKED, ITEM_NOTE, ITEM_FIELD_COUNT };

Qt::ItemFlags InventoryModel::flags(const QModelIndex & index) const
{
	if(index.row() < 0 || rowCount() <= index.row())
		return Qt::NoItemFlags;
	if(index.column() < 0 || ITEM_FIELD_COUNT <= index.column())
		return Qt::NoItemFlags;

	Qt::ItemFlags result = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
	static QList<int> checkableFields = QList<int>() << ITEM_WRITTEN_OFF << ITEM_UNDER_REPAIR << ITEM_CHECKED;
	if(checkableFields.contains(index.column())) {
		result |= Qt::ItemIsUserCheckable;
	}
	return result;
}

QVariant InventoryModel::data(const QModelIndex & index, int role) const
{
	if(index.row() >= rowCount())
		return QVariant();

	const Item & item = items[index.row()];
	if(role == Qt::CheckStateRole) {
		switch(index.column()) {
			case ITEM_WRITTEN_OFF: return item.writtenOff ? Qt::Checked : Qt::Unchecked;
			case ITEM_UNDER_REPAIR: return item.underRepair ? Qt::Checked : Qt::Unchecked;
			case ITEM_CHECKED: return item.checked ? Qt::Checked : Qt::Unchecked;
			default: return QVariant();
		}
	}
	if(role == Qt::EditRole || role == Qt::DisplayRole) {
		switch(index.column()) {
			case ITEM_TYPE: {
				if(role == Qt::EditRole) {
					return item.itemTypeId;
				} else if(role == Qt::DisplayRole) {
					return item.itemType;
				}
				break;
			}
			case ITEM_PLACE: {
				if(role == Qt::EditRole) {
					return item.placeId;
				} else if(role == Qt::DisplayRole) {
					return item.place;
				}
				break;
			}
			case ITEM_PERSON: {
				if(role == Qt::EditRole) {
					return item.responsiblePersonId;
				} else if(role == Qt::DisplayRole) {
					return item.responsiblePerson;
				}
				break;
			}
			case ITEM_NAME: return item.name;
			case ITEM_INN: {
			    if(item.innIsNotNull) {
			 	   return item.inn;
			    } else {
			 	   return "";
			    }
			}
			case ITEM_WRITTEN_OFF: return item.writtenOff ? tr("Written off") : "";
			case ITEM_UNDER_REPAIR: return item.underRepair ? tr("Under repair") : "";
			case ITEM_CHECKED: return item.checked ? tr("Checked") : "";
			case ITEM_NOTE: return item.note;
			default: return QVariant();
		}
	}
	return QVariant();
}

enum { HISTORY_TYPE, HISTORY_PLACE, HISTORY_PERSON, HISTORY_NAME, HISTORY_INN,
	HISTORY_WRITTEN_OFF, HISTORY_UNDER_REPAIR, HISTORY_CHECKED, HISTORY_NOTE, HISTORY_FIELD_COUNT };
template<class T> void updateField(Id id, const QString & fieldName, int fieldIndex, const T & oldValue, const T & newValue)
{
	if(oldValue == newValue)
		return;
	if(fieldIndex >= HISTORY_FIELD_COUNT)
		return;

	Database::Placeholders map;
	map[":old"] = oldValue;
	map[":field"] = fieldIndex;
	map[":new"] = newValue;
	Database::query(
			"INSERT INTO History (fieldName, oldValue, newValue) "
			" VALUES (:field, :old, :new); ", map
			);

	Database::Placeholders values;
	values[":value"] = newValue;
	values[":id"] = id;
	Database::query("UPDATE Inventory SET " + fieldName + " = :value WHERE id = :id; ", values);
}

bool foreignIdExists(const QString & tableName, int id)
{
	Database::Placeholders map;
	map[":id"] = id;
	QSqlQuery q = Database::select("SELECT COUNT(*) FROM " + tableName + " WHERE id = :id;", map);
	if(!q.isActive())
		return false;
	q.next();
	return q.value(0).toInt() > 0;
}

bool InventoryModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if(index.row() >= rowCount())
		return false;
	if(!value.isValid())
		return false;

	if(role == Qt::CheckStateRole) {
		const Item & item = items[index.row()];
		switch(index.column()) {
			case ITEM_WRITTEN_OFF:  updateField(item.id, "writtenOff",  HISTORY_WRITTEN_OFF,  item.writtenOff,  value == Qt::Checked); break;
			case ITEM_UNDER_REPAIR: updateField(item.id, "underRepair", HISTORY_UNDER_REPAIR, item.underRepair, value == Qt::Checked); break;
			case ITEM_CHECKED:      updateField(item.id, "checked",     HISTORY_CHECKED,      item.checked,     value == Qt::Checked); break;
			default: return false;
		}
	} else if(role == Qt::EditRole) {
		const Item & item = items[index.row()];
		switch(index.column()) {
			case ITEM_TYPE:
				if(!foreignIdExists("ItemTypes", value.toInt()))
					return false;
				updateField(item.id, "itemType", HISTORY_TYPE, item.itemTypeId, value.toInt());
				break;
			case ITEM_PLACE:
				if(!foreignIdExists("Places", value.toInt()))
					return false;
				updateField(item.id, "place", HISTORY_PLACE, item.placeId, value.toInt());
				break;
			case ITEM_PERSON:
				if(!foreignIdExists("Persons", value.toInt()))
					return false;
				updateField(item.id, "responsiblePerson", HISTORY_PERSON, item.responsiblePersonId, value.toInt());
				break;
			case ITEM_NAME: updateField(item.id, "name", HISTORY_NAME, item.name, value.toString()); break;
			case ITEM_INN: {
				bool valueIsNull = !value.isValid();
				bool valueIsNum = false;
				int inn = value.toInt(&valueIsNum);
				bool valueIsEmpty = value.toString().isEmpty();

				if(!valueIsNull && (valueIsNum || valueIsEmpty)) {
					updateField(item.id, "inn", HISTORY_INN,
							item.innIsNotNull ? QVariant(item.inn) : QVariant(),
							valueIsEmpty ? QVariant() : inn
							);
				} else {
					return false;
				}
				break;
			}
			case ITEM_NOTE: updateField(item.id, "note", HISTORY_NOTE, item.note, value.toString()); break;
			default: return false;
		}
	} else {
		return false;
	}

	update();
	return true;
}

QVariant InventoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role != Qt::EditRole && role != Qt::DisplayRole)
		return QVariant();
	if(orientation != Qt::Horizontal)
		return QVariant();
	switch(section) {
		case ITEM_TYPE: return tr("Item type");
		case ITEM_PLACE: return tr("Place");
		case ITEM_PERSON: return tr("Responsible person");
		case ITEM_NAME: return tr("Name");
		case ITEM_INN: return tr("INN");
		case ITEM_WRITTEN_OFF: return tr("Written off");
		case ITEM_UNDER_REPAIR: return tr("Under repair");
		case ITEM_CHECKED: return tr("Checked");
		case ITEM_NOTE: return tr("Note");
		default: return QVariant();
	}
	return QVariant();
}

int InventoryModel::rowCount(const QModelIndex & /*parent*/) const
{
	return items.count();
}

int InventoryModel::columnCount(const QModelIndex & /*parent*/) const
{
	return ITEM_FIELD_COUNT;
}

Id getDefaultId(const QString & tableName)
{
	QSqlQuery q = Database::select("SELECT COUNT(*) FROM " + tableName + ";");
	q.next();
	if(q.value(0).toInt() == 0) {
		q = Database::select("INSERT INTO " + tableName + " (name) VALUES ('None');");
		if(q.lastInsertId().isValid()) {
			return q.lastInsertId().toInt();
		}
	}
	q = Database::select("SELECT id FROM " + tableName + " ; ");
	q.next();
	return q.value(0).toInt();
}

bool InventoryModel::insertRows(int row, int count, const QModelIndex & /*parent*/)
{
	if(count <= 0)
		return false;
	if(row < 0 || rowCount() + 1 <= row)
		return false;

	Id itemTypeId = getDefaultId("ItemTypes");
	Id placeId    = getDefaultId("Places");
	Id personId   = getDefaultId("Persons");
	// TODO Если установлен фильтр, использовать его значения.

	while(count--) {
		Database::Placeholders map;
		map[":itemtype"] = itemTypeId;
		map[":place"]    = placeId;
		map[":person"]   = personId;
		Database::query(
				"INSERT INTO Inventory (itemType, place, responsiblePerson) "
				" VALUES (:itemtype, :place, :person); ",
				map
				);
	}
	update();
	return true;
}

bool InventoryModel::removeRows(int row, int count, const QModelIndex & /*parent*/)
{
	if(count <= 0)
		return false;
	if(row < 0 || row >= rowCount())
		return false;
	
	for(int i = 0; i < count; ++i) {
		Database::Placeholders map;
		map[":id"] = idAt(row + i);
		Database::query("DELETE FROM History WHERE item = :id", map);
		Database::query("DELETE FROM Inventory WHERE id = :id", map);
	}
	update();
	return true;
}

Id InventoryModel::idAt(int row) const
{
	if(0 <= row && row < items.count()) {
		return items[row].id;
	}
	return Id();
}

void InventoryModel::setItemTypeFilter(int /*itemType*/)
{
	return;
}

void InventoryModel::switchItemTypeFilter(bool /*on*/)
{
	return;
}

void InventoryModel::setPlaceFilter(int /*place*/)
{
	return;
}

void InventoryModel::switchPlaceFilter(bool /*on*/)
{
	return;
}

void InventoryModel::setWrittenOffFilter(bool /*writtenOff*/)
{
	return;
}

void InventoryModel::switchWrittenOffFilter(bool /*on*/)
{
	return;
}

}

namespace Inventory { // HistoryModel

HistoryModel::HistoryModel(int /*item*/, QObject * /*parent*/)
{
	return;
}

Qt::ItemFlags HistoryModel::flags(const QModelIndex & /*index*/) const
{
	return 0;
}

QVariant HistoryModel::data(const QModelIndex & /*index*/, int /*role*/) const
{
	return QVariant();
}

QVariant HistoryModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int /*role*/) const
{
	return QVariant();
}

int HistoryModel::rowCount(const QModelIndex & /*parent*/) const
{
	return 0;
}

int HistoryModel::columnCount(const QModelIndex & /*parent*/) const
{
	return 0;
}


/*
История:
	Создать: требуется указать предмет.
	Столбцов: 5.
	Строк: сколько загружено.
	Нередактируемо.
	Ячейки:
		Время - текст (не показывать день недели).
		Поле - текст.
		Старое значение - текст.
		Новое значение - текст.
		Для полей-пометок писать словами.
*/

}

namespace Inventory { // PrintableInventoryModel

PrintableInventoryModel::PrintableInventoryModel(QObject * /*parent*/)
{
	return;
}

Qt::ItemFlags PrintableInventoryModel::flags(const QModelIndex & /*index*/) const
{
	return 0;
}

QVariant PrintableInventoryModel::data(const QModelIndex & /*index*/, int /*role*/) const
{
	return QVariant();
}

QVariant PrintableInventoryModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int /*role*/) const
{
	return QVariant();
}

int PrintableInventoryModel::rowCount(const QModelIndex & /*parent*/) const
{
	return 0;
}

int PrintableInventoryModel::columnCount(const QModelIndex & /*parent*/) const
{
	return 0;
}

void PrintableInventoryModel::setItemTypeFilter(int /*itemType*/)
{
	return;
}

void PrintableInventoryModel::switchItemTypeFilter(bool /*on*/)
{
	return;
}

void PrintableInventoryModel::setPlaceFilter(int /*place*/)
{
	return;
}

void PrintableInventoryModel::switchPlaceFilter(bool /*on*/)
{
	return;
}

void PrintableInventoryModel::setWrittenOffFilter(bool /*writtenOff*/)
{
	return;
}

void PrintableInventoryModel::switchWrittenOffFilter(bool /*on*/)
{
	return;
}


/*
Печать:
	Нередактируемо.
	Столбцов: 6
	Строк: сколько загружено.
	Ячейки:
		Тип: слово.
		Наименование: слово.
		Количество: слово.
		Место: слово.
		ИНН: пусто или число.
		Пометка: пусто или "списан".
	Установить фильтр по типу(тип).
	Включить фильтр по типу(вкл|выкл).
	Установить фильтр по месту(место).
	Включить фильтр по месту(вкл|выкл).
	Установить фильтр по списанию(списан|нет).
	Включить фильтр по списанию.
	*/

}

namespace Inventory { // ReferenceModel

RefType getRefType(int type)
{
	RefType result;
	result.type = ReferenceModel::INVALID;
	switch(type) {
		case ReferenceModel::ITEM_TYPES: {
			result.type = type;
			result.table = "ItemTypes";
			result.foreignKey = "itemType";
			break;
		}
		case ReferenceModel::PLACES: {
			result.type = type;
			result.table = "Places";
			result.foreignKey = "place";
			break;
		}
		case ReferenceModel::PERSONS: {
			result.type = type;
			result.table = "Persons";
			result.foreignKey = "responsiblePerson";
			break;
		}
		default: break;
	}
	return result;
}

ReferenceModel::ReferenceModel(int newType, QObject * parent)
	: QAbstractTableModel(parent)
{
	refType = getRefType(newType);
	if(!type())
		return;

	Database::query(" CREATE TABLE IF NOT EXISTS "
			" " + refType.table + " ( "
			" id                INTEGER  PRIMARY KEY AUTOINCREMENT, "
			" name              TEXT NOT NULL DEFAULT '' "
			" ); "
			);
	update();
}

void ReferenceModel::update()
{
	if(!type())
		return;

	QSqlQuery query = Database::select(
			" SELECT id, name "
			" FROM " + refType.table + " "
			" ; "
			);
	records.clear();
	while(query.next()) {
		RefRecord rec;
		rec.id   = query.value(0).toInt();
		rec.name = query.value(1).toString();
		records << rec;
	}
}

Qt::ItemFlags ReferenceModel::flags(const QModelIndex & index) const
{
	if(index.row() < 0 || rowCount() <= index.row())
		return Qt::NoItemFlags;
	if(index.column() < 0 || columnCount() <= index.column())
		return Qt::NoItemFlags;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant ReferenceModel::data(const QModelIndex & index, int role) const
{
	if(index.row() >= rowCount())
		return QVariant();

	const RefRecord & rec = records[index.row()];
	if(role == Qt::EditRole || role == Qt::DisplayRole) {
		if(index.column() == 0) {
			return rec.name;
		}
	}
	return QVariant();
}

bool ReferenceModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if(index.row() >= rowCount())
		return false;

	if(role == Qt::EditRole) {
		if(index.column() == 0) {
			if(records[index.row()].name == value.toString())
				return true;

			Database::Placeholders values;
			values[":value"] = value.toString();
			values[":id"]    = idAt(index.row());
			Database::query("UPDATE " + refType.table + " SET name = :value WHERE id = :id;", values);

			update();
			return true;
		}
	}

	return false;
}

QVariant ReferenceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role != Qt::EditRole && role != Qt::DisplayRole)
		return QVariant();
	if(orientation != Qt::Horizontal)
		return QVariant();

	if(section == 0) {
		return tr("Name");
	}
	return QVariant();
}

int ReferenceModel::rowCount(const QModelIndex & /*parent*/) const
{
	return records.count();
}

int ReferenceModel::columnCount(const QModelIndex & /*parent*/) const
{
	return type() ? 1 : 0;
}

bool ReferenceModel::insertRows(int row, int count, const QModelIndex & /*parent*/)
{
	if(!type())
		return false;

	if(count <= 0)
		return false;
	if(row < 0 || rowCount() + 1 <= row)
		return false;

	while(count--) {
		Database::query("INSERT INTO " + refType.table + " DEFAULT VALUES; ");
	}
	update();
	return true;
}

int ReferenceModel::countOfItemsUsing(Id id)
{
	QString queryText = QString(
			"SELECT COUNT(Inventory.id) FROM Inventory, %1"
			" WHERE Inventory.%2 = %1.id AND %1.id = :id;"
			).
		arg(refType.table).
		arg(refType.foreignKey);

	Database::Placeholders map;
	map[":id"] = id;
	QSqlQuery q = Database::select(queryText, map);
	q.next();
	return q.value(0).toInt();
}

bool ReferenceModel::removeRows(int row, int count, const QModelIndex & /*parent*/)
{
	if(!type())
		return false;

	if(count <= 0)
		return false;
	if(row < 0 || row >= rowCount() + 1)
		return false;
	
	for(int i = 0; i < count; ++i) {
		if(countOfItemsUsing(idAt(row + i)) > 0) {
			return false;
		}

		Database::Placeholders map;
		map[":id"] = idAt(row + i);
		Database::query("DELETE FROM " + refType.table + " WHERE id = :id", map);
	}
	update();
	return true;
}

Id ReferenceModel::idAt(int row) const
{
	if(0 <= row && row < records.count()) {
		return records[row].id;
	}
	return Id();
}

int ReferenceModel::type() const
{
	return refType.type;
}

bool ReferenceModel::addMultiline(const QStringList & lines)
{
	if(!type())
		return false;

	foreach(QString name, lines) {
		Database::Placeholders map;
		map[":name"] = name;
		Database::query("INSERT INTO " + refType.table + " (name) VALUES (:name); ", map);
	}
	update();

	return true;
}

}
