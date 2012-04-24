#include <QtDebug>

#include "models.h"

namespace Inventory {

QString Database::dbName = "";

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
	db.setDatabaseName(dbName);
	bool ok = db.open();
	if(!ok)
		return false;

	Database::query(" CREATE TABLE IF NOT EXISTS "
			" Places ( "
			" id                INTEGER  PRIMARY KEY AUTOINCREMENT, "
			" name              TEXT NOT NULL DEFAULT '' "
			" ); "
			);
	Database::query(" CREATE TABLE IF NOT EXISTS "
			" Persons ( "
			" id                INTEGER  PRIMARY KEY AUTOINCREMENT, "
			" name              TEXT NOT NULL DEFAULT '' "
			" ); "
			);
	Database::query(" CREATE TABLE IF NOT EXISTS "
			" ItemTypes ( "
			" id                INTEGER  PRIMARY KEY AUTOINCREMENT, "
			" name              TEXT NOT NULL DEFAULT '' "
			" ); "
			);
	Database::query(" CREATE TABLE IF NOT EXISTS "
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
			);
	Database::query(" CREATE TABLE IF NOT EXISTS "
			" History ( "
			"   id                INTEGER  PRIMARY KEY AUTOINCREMENT, "
			"   item              INTEGER  NOT NULL, "
			"   changeTime        TEXT     NOT NULL DEFAULT '', "
			"   field             INTEGER  NOT NULL DEFAULT 10, "
			"   oldValue          TEXT     NOT NULL DEFAULT '', "
			"   newValue          TEXT     NOT NULL DEFAULT '', "
			"   FOREIGN KEY (item) REFERENCES Inventory(id) "
			" ); "
			);
	Database::query(" CREATE VIEW IF NOT EXISTS "
			" PrintableInventory AS "
			" SELECT "
			"   ItemTypes.name        AS itemType, "
			"   ItemTypes.id          AS itemTypeId, "
			"   Inventory.name        AS name, "
			"   COUNT(Inventory.name) AS itemCount, "
			"   Places.name           AS place, "
			"   Places.id             AS placeId, "
			"                            inn, "
			"                            writtenOff "
			" FROM Inventory, ItemTypes, Places "
			" WHERE "
			"   Inventory.itemType = ItemTypes.id AND "
			"   Inventory.place    = Places.id "
			" GROUP BY itemType, itemTypeId, name, place, placeId, inn, writtenOff "
			" ; "
			);

	return ok;
}

void Database::close()
{
	{
		QSqlDatabase db = QSqlDatabase::database("inventory");
		db.close();
	}
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

const QString & Database::databaseName()
{
	return dbName;
}

void Database::setDatabaseName(const QString & newName)
{
	dbName = newName;
}

bool Database::query(const QString & prepared, const Placeholders & placeholders)
{
	QSqlQuery q(Database::db());
	q.prepare(prepared);
	foreach(QString key, placeholders.keys()) {
		q.bindValue(key, placeholders[key]);
	}
	bool ok = q.exec();
	if(!ok) {
		qDebug() << q.executedQuery() << error(q.lastError());
	}
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

namespace Inventory { // Filter

Filter::Filter()
	: useItemTypeFilter(false), itemTypeFilter(0),
	usePlaceFilter(false), placeFilter(0),
	useWrittenOffFilter(false), writtenOffFilter(false) { }

}

namespace Inventory { // InventoryModel

enum { ITEM_TYPE, ITEM_PLACE, ITEM_PERSON, ITEM_NAME, ITEM_INN, ITEM_WRITTEN_OFF, ITEM_UNDER_REPAIR, ITEM_CHECKED, ITEM_NOTE, ITEM_FIELD_COUNT };

InventoryModel::InventoryModel(QObject * parent)
	: AbstractUpdatableTableModel(parent),
	sorted(false), sortColumn(0), sortOrder(Qt::AscendingOrder)
{
	update();
}

void InventoryModel::update()
{
	Database::Placeholders map;
	QStringList constraints;
	if(filter.useItemTypeFilter) {
		constraints << "itemType = :itemType";
		map[":itemType"] = filter.itemTypeFilter;
	}
	if(filter.usePlaceFilter) {
		constraints << "place = :place";
		map[":place"] = filter.placeFilter;
	}
	if(filter.useWrittenOffFilter) {
		constraints << "writtenOff = :writtenOff";
		map[":writtenOff"] = int(filter.writtenOffFilter);
	}
	constraints << "Inventory.itemType = ItemTypes.id";
	constraints << "Inventory.place = Places.id";
	constraints << "Inventory.responsiblePerson = Persons.id";

	QString whereClause = constraints.join(" AND ");
	if(!whereClause.isEmpty()) {
		whereClause = " WHERE " + whereClause;
	}

	QMap<int, QString> fields; {
		fields[ITEM_TYPE]         = "ItemTypes.name";
		fields[ITEM_PLACE]        = "Places.name";
		fields[ITEM_PERSON]       = "Persons.name";
		fields[ITEM_NAME]         = "Inventory.name";
		fields[ITEM_INN]          = "inn";
		fields[ITEM_WRITTEN_OFF]  = "writtenOff";
		fields[ITEM_UNDER_REPAIR] = "underRepair";
		fields[ITEM_CHECKED]      = "checked";
		fields[ITEM_NOTE]         = "note";
	}
	QString sortClause = sorted ? QString(" ORDER BY %1 %2 ").arg(fields.value(sortColumn)).arg(sortOrder == Qt::AscendingOrder ? "ASC" : "DESC") : 0;

	QSqlQuery query = Database::select(
			" SELECT Inventory.id, "
			"   itemType, ItemTypes.name, "
			"   place, Places.name, "
			"   responsiblePerson, Persons.name, "
			"   Inventory.name, "
			"   inn, writtenOff, underRepair, checked, note "
			" FROM Inventory, ItemTypes, Places, Persons "
			+ whereClause 
			+ sortClause + 
			" ; ",
			map
			);

	beginResetModel();
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
		item.inn                 = query.value( 8).toInt();
		item.writtenOff          = query.value( 9).toInt();
		item.underRepair         = query.value(10).toInt();
		item.checked             = query.value(11).toInt();
		item.note                = query.value(12).toString();
		items << item;
	}
	endResetModel();
}

QList<int> InventoryModel::columnsToHide() const
{
	return QList<int>() << ITEM_PERSON << ITEM_UNDER_REPAIR << ITEM_CHECKED;
}

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
			    if(item.inn) {
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

enum { HISTORY_TYPE, HISTORY_PLACE, HISTORY_PERSON, HISTORY_NAME, HISTORY_INN,
	HISTORY_WRITTEN_OFF, HISTORY_UNDER_REPAIR, HISTORY_CHECKED, HISTORY_NOTE, HISTORY_FIELD_COUNT };
void updateField(Id id, const QString & fieldName, int fieldIndex, const QString & oldValue, const QString & newValue)
{
	if(oldValue == newValue)
		return;
	if(fieldIndex >= HISTORY_FIELD_COUNT)
		return;

	Database::Placeholders map;
	map[":id"]    = id;
	map[":field"] = fieldIndex;
	map[":old"]   = oldValue;
	map[":new"]   = newValue;
	Database::query(
			"INSERT INTO History (item, changeTime, field, oldValue, newValue) "
			" VALUES (:id, DATETIME('now'), :field, :old, :new); ", map
			);

	Database::Placeholders values;
	values[":value"] = newValue;
	values[":id"]    = id;
	Database::query("UPDATE Inventory SET " + fieldName + " = :value WHERE id = :id; ", values);
}

void updateField(Id id, const QString & fieldName, int fieldIndex, int oldValue, int newValue)
{
	updateField(id, fieldName, fieldIndex, QString::number(oldValue), QString::number(newValue));
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
					updateField(item.id, "inn", HISTORY_INN, item.inn, inn);
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

	Id itemTypeId = filter.useItemTypeFilter ? filter.itemTypeFilter : getDefaultId("ItemTypes");
	Id placeId    = filter.usePlaceFilter    ? filter.placeFilter    : getDefaultId("Places");
	Id personId   = getDefaultId("Persons");


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
	if(count < 0)
		return false;
	if(count == 0)
		return true;
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

void InventoryModel::sort(int column, Qt::SortOrder order)
{
	if(column >= ITEM_FIELD_COUNT)
		return;

	sorted = true;
	sortColumn = column;
	sortOrder = order;
	update();
}

Id InventoryModel::idAt(int row) const
{
	if(0 <= row && row < items.count()) {
		return items[row].id;
	}
	return Id();
}

void InventoryModel::setItemTypeFilter(int itemType)
{
	filter.itemTypeFilter = itemType;
	update();
}

void InventoryModel::switchItemTypeFilter(bool on)
{
	filter.useItemTypeFilter = on;
	update();
}

void InventoryModel::setPlaceFilter(int place)
{
	filter.placeFilter = place;
	update();
}

void InventoryModel::switchPlaceFilter(bool on)
{
	filter.usePlaceFilter = on;
	update();
}

void InventoryModel::setWrittenOffFilter(bool writtenOff)
{
	filter.writtenOffFilter = writtenOff;
	update();
}

void InventoryModel::switchWrittenOffFilter(bool on)
{
	filter.useWrittenOffFilter = on;
	update();
}

}

namespace Inventory { // HistoryModel

QString unfoldHistoryValue(int field, const QString & value)
{
	switch(field) {
		case HISTORY_TYPE: {
			Database::Placeholders map;
			map[":id"] = value;
			QSqlQuery q = Database::select("SELECT name FROM ItemTypes WHERE id = :id", map);
			if(!q.next())
				return "";
			return q.value(0).toString();
		}
		case HISTORY_PLACE: {
			Database::Placeholders map;
			map[":id"] = value;
			QSqlQuery q = Database::select("SELECT name FROM Places WHERE id = :id", map);
			if(!q.next())
				return "";
			return q.value(0).toString();
		}
		case HISTORY_PERSON: {
			Database::Placeholders map;
			map[":id"] = value;
			QSqlQuery q = Database::select("SELECT name FROM Persons WHERE id = :id", map);
			if(!q.next())
				return "";
			return q.value(0).toString();
		}
		case HISTORY_NAME:         return value;
		case HISTORY_INN:          return (value == "0") ? "" : value;
		case HISTORY_WRITTEN_OFF:  return (value == "1") ? HistoryModel::tr("Written off")  : HistoryModel::tr("Present");
		case HISTORY_UNDER_REPAIR: return (value == "1") ? HistoryModel::tr("Under repair") : HistoryModel::tr("Working");
		case HISTORY_CHECKED:      return (value == "1") ? HistoryModel::tr("Checked")      : HistoryModel::tr("Not checked");
		case HISTORY_NOTE:         return value;
		default: return value;
	};
	return value;
}

HistoryModel::HistoryModel(int item, QObject * parent)
	: QAbstractTableModel(parent)
{
	Database::Placeholders map;
	map[":item"] = item;
	QSqlQuery query = Database::select(
			" SELECT id, changeTime, field, oldValue, newValue "
			" FROM History "
			" WHERE item = :item; ", map
			);
	records.clear();
	while(query.next()) {
		HistoryRecord rec;
		rec.id         = query.value(0).toInt();
		rec.changeTime = QDateTime::fromString(query.value(1).toString(), Qt::ISODate);
		rec.field      = query.value(2).toInt();
		rec.oldValue   = unfoldHistoryValue(rec.field, query.value(3).toString());
		rec.newValue   = unfoldHistoryValue(rec.field, query.value(4).toString());
		records << rec;
	}
}

Qt::ItemFlags HistoryModel::flags(const QModelIndex & index) const
{
	if(index.row() < 0 || rowCount() <= index.row())
		return Qt::NoItemFlags;
	if(index.column() < 0 || columnCount() <= index.column())
		return Qt::NoItemFlags;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant HistoryModel::data(const QModelIndex & index, int role) const
{
	if(index.row() >= rowCount())
		return QVariant();

	const HistoryRecord & rec = records[index.row()];
	if(role == Qt::DisplayRole) {
		switch(index.column()) {
			case 0: return rec.changeTime.toString();
			case 1:
				switch(rec.field) {
					case HISTORY_TYPE:         return tr("Item type");
					case HISTORY_PLACE:        return tr("Place");
					case HISTORY_PERSON:       return tr("Responsible person");
					case HISTORY_NAME:         return tr("Name");
					case HISTORY_INN:          return tr("INN");
					case HISTORY_WRITTEN_OFF:  return tr("Written off");
					case HISTORY_UNDER_REPAIR: return tr("Under repair");
					case HISTORY_CHECKED:      return tr("Checked");
					case HISTORY_NOTE:         return tr("Note");
					default: return tr("Unknown");
				}
				break;
			case 2: return rec.oldValue;
			case 3: return rec.newValue;
			default: return QVariant();
		}
	}
	return QVariant();
}

QVariant HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role != Qt::EditRole && role != Qt::DisplayRole)
		return QVariant();
	if(orientation != Qt::Horizontal)
		return QVariant();

	switch(section) {
		case 0: return tr("Change time");
		case 1: return tr("Field name");
		case 2: return tr("Old value");
		case 3: return tr("New value");
		default: return QVariant();
	}
	return QVariant();
}

int HistoryModel::rowCount(const QModelIndex & /*parent*/) const
{
	return records.count();
}

int HistoryModel::columnCount(const QModelIndex & /*parent*/) const
{
	return 4;
}

Id HistoryModel::idAt(int row) const
{
	if(0 <= row && row < records.count()) {
		return records[row].id;
	}
	return Id();
}

bool HistoryModel::removeRows(int row, int count, const QModelIndex & /*parent*/)
{
	if(count < 0)
		return false;
	if(count == 0)
		return true;
	if(count != 1)
		return false;
	if(row != rowCount() - 1)
		return false;
	
	Database::Placeholders map;
	map[":id"] = idAt(row);
	Database::query("DELETE FROM History WHERE id = :id", map);

	beginResetModel();
	records.takeLast();
	endResetModel();
	return true;
}

}

namespace Inventory { // PrintableInventoryModel

PrintableInventoryModel::PrintableInventoryModel(QObject * parent)
	: AbstractUpdatableTableModel(parent),
	sorted(false), sortColumn(0), sortOrder(Qt::AscendingOrder)
{
	update();
}

void PrintableInventoryModel::update()
{
	Database::Placeholders map;
	QStringList constraints;
	if(filter.useItemTypeFilter) {
		constraints << "itemTypeId = :itemType";
		map[":itemType"] = filter.itemTypeFilter;
	}
	if(filter.usePlaceFilter) {
		constraints << "placeId = :place";
		map[":place"] = filter.placeFilter;
	}
	if(filter.useWrittenOffFilter) {
		constraints << "writtenOff = :writtenOff";
		map[":writtenOff"] = int(filter.writtenOffFilter);
	}
	QString whereClause = constraints.join(" AND ");
	if(!whereClause.isEmpty()) {
		whereClause = " WHERE " + whereClause;
	}

	QMap<int, QString> fields; {
		fields[0] = "itemType";
		fields[1] = "name";
		fields[2] = "itemCount";
		fields[3] = "place";
		fields[4] = "inn";
		fields[5] = "writtenOff";
	}
	QString sortClause = sorted ? QString(" ORDER BY %1 %2 ").arg(fields.value(sortColumn)).arg(sortOrder == Qt::AscendingOrder ? "ASC" : "DESC") : 0;

	QSqlQuery query = Database::select(
			" SELECT "
			"   itemType, "
			"   itemTypeId, "
			"   name, "
			"   itemCount, "
			"   place, "
			"   placeId, "
			"   inn, "
			"   writtenOff "
			" FROM PrintableInventory "
			+ whereClause
			+ sortClause + 
			" ; ",
			map
			);

	beginResetModel();
	groups.clear();
	while(query.next()) {
		ItemGroup group;
		group.itemType   = query.value(0).toString();
		group.itemTypeId = query.value(1).toInt();
		group.name       = query.value(2).toString();
		group.itemCount  = query.value(3).toInt();
		group.place      = query.value(4).toString();
		group.placeId    = query.value(5).toInt();
		group.inn        = query.value(6).toInt();
		group.writtenOff = query.value(7).toInt();
		groups << group;
	}
	endResetModel();
}

Qt::ItemFlags PrintableInventoryModel::flags(const QModelIndex & index) const
{
	if(index.row() < 0 || rowCount() <= index.row())
		return Qt::NoItemFlags;
	if(index.column() < 0 || columnCount() <= index.column())
		return Qt::NoItemFlags;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant PrintableInventoryModel::data(const QModelIndex & index, int role) const
{
	if(index.row() >= rowCount())
		return QVariant();

	const ItemGroup & group = groups[index.row()];
	if(role == Qt::EditRole) {
		switch(index.column()) {
			case 0: return group.itemTypeId;
			case 3: return group.placeId;
		}
	} else if(role == Qt::DisplayRole) {
		switch(index.column()) {
			case 0: return group.itemType;
			case 1: return group.name;
			case 2: return group.itemCount;
			case 3: return group.place;
			case 4: return group.inn ? QVariant(group.inn) : QVariant("");
			case 5: return group.writtenOff ? tr("Written off") : "";
			default: return QVariant();
		}
	}
	return QVariant();
}

QVariant PrintableInventoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role != Qt::EditRole && role != Qt::DisplayRole)
		return QVariant();
	if(orientation != Qt::Horizontal)
		return QVariant();

	switch(section) {
		case 0: return tr("Item type");
		case 1: return tr("Name");
		case 2: return tr("Count");
		case 3: return tr("Place");
		case 4: return tr("INN");
		case 5: return tr("Written off");
		default: return QVariant();
	}
	return QVariant();
}

int PrintableInventoryModel::rowCount(const QModelIndex & /*parent*/) const
{
	return groups.count();
}

int PrintableInventoryModel::columnCount(const QModelIndex & /*parent*/) const
{
	return 6;
}

void PrintableInventoryModel::sort(int column, Qt::SortOrder order)
{
	if(column >= ITEM_FIELD_COUNT)
		return;

	sorted = true;
	sortColumn = column;
	sortOrder = order;
	update();
}

void PrintableInventoryModel::setItemTypeFilter(int itemType)
{
	filter.itemTypeFilter = itemType;
	update();
}

void PrintableInventoryModel::switchItemTypeFilter(bool on)
{
	filter.useItemTypeFilter = on;
	update();
}

void PrintableInventoryModel::setPlaceFilter(int place)
{
	filter.placeFilter = place;
	update();
}

void PrintableInventoryModel::switchPlaceFilter(bool on)
{
	filter.usePlaceFilter = on;
	update();
}

void PrintableInventoryModel::setWrittenOffFilter(bool writtenOff)
{
	filter.writtenOffFilter = writtenOff;
	update();
}

void PrintableInventoryModel::switchWrittenOffFilter(bool on)
{
	filter.useWrittenOffFilter = on;
	update();
}

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
	: AbstractUpdatableTableModel(parent)
{
	refType = getRefType(newType);
	if(!type())
		return;

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

	beginResetModel();
	records.clear();
	while(query.next()) {
		RefRecord rec;
		rec.id   = query.value(0).toInt();
		rec.name = query.value(1).toString();
		records << rec;
	}
	endResetModel();
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

int ReferenceModel::rowOf(Id id) const
{
	for(int row = 0; row < rowCount(); ++row) {
		if(records[row].id == id)
			return row;
	}
	return -1;
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
		if(name.isEmpty())
			continue;
		Database::Placeholders map;
		map[":name"] = name;
		Database::query("INSERT INTO " + refType.table + " (name) VALUES (:name); ", map);
	}
	update();

	return true;
}

}

namespace Inventory { // InventoryDelegate
	
InventoryDelegate::InventoryDelegate(QObject * parent)
	: QItemDelegate(parent)
{
}

InventoryDelegate::~InventoryDelegate() {
}

QWidget* InventoryDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if(!index.isValid())
		return QItemDelegate::createEditor(parent, option, index);

	int refType = ReferenceModel::INVALID;
	switch(index.column()) {
		case ITEM_TYPE  : refType = ReferenceModel::ITEM_TYPES; break;
		case ITEM_PLACE : refType = ReferenceModel::PLACES; break;
		case ITEM_PERSON: refType = ReferenceModel::PERSONS; break;
		default: return QItemDelegate::createEditor(parent, option, index);
	}

	QComboBox * comboBox = new QComboBox(parent);
	ReferenceModel * model = new ReferenceModel(refType, comboBox);
	comboBox->setModel(model);
	return comboBox;
}

void InventoryDelegate::setEditorData(QWidget * editor, const QModelIndex &index) const
{
	if(!index.isValid()) {
		QItemDelegate::setEditorData(editor, index);
		return;
	}
	int col = index.column();
	if(col != ITEM_TYPE && col != ITEM_PLACE && col != ITEM_PERSON) {
		QItemDelegate::setEditorData(editor, index);
		return;
	}
	
	QComboBox * comboBox = static_cast<QComboBox *>(editor);
	if(!comboBox) {
		QItemDelegate::setEditorData(editor, index);
		return;
	}
	ReferenceModel * model = static_cast<ReferenceModel *>(comboBox->model());

	Id id = index.model()->data(index, Qt::EditRole).toInt();
	comboBox->setCurrentIndex(model->rowOf(id));
}

void InventoryDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	if(!index.isValid()) {
		QItemDelegate::setModelData(editor, model, index);
		return;
	}
	int col = index.column();
	if(col != ITEM_TYPE && col != ITEM_PLACE && col != ITEM_PERSON) {
		QItemDelegate::setModelData(editor, model, index);
		return;
	}

	QComboBox *comboBox = static_cast<QComboBox*>(editor);
	ReferenceModel * refModel = static_cast<ReferenceModel*>(comboBox->model());

	Id id = refModel->idAt(comboBox->currentIndex());
	model->setData(index, id, Qt::EditRole);
}

void InventoryDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const
{
	editor->setGeometry(option.rect);
}

}
