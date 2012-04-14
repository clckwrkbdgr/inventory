#pragma once

#include <QtCore/QAbstractTableModel>
#include <QtCore/QDateTime>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtCore/QStringList>

namespace Inventory {

class Database {
public:
	typedef QMap<QString, QVariant> Placeholders;
	static bool reopen();
	static void close();

	static QSqlDatabase db();
	static QString error(QSqlError e = db().lastError());
	static void setDatabaseName(const QString & newName);
	static const QString & databaseName();
	static bool query(const QString & prepared, const Placeholders & placeholders = Placeholders());
	static QSqlQuery select(const QString & prepared, const Placeholders & placeholders = Placeholders());
private:
	Database() {}
	~Database() {}

	static QString dbName;
};

typedef int Id;

class AbstractUpdatableTableModel : public QAbstractTableModel {
	Q_OBJECT
public:
	AbstractUpdatableTableModel(QObject * parent = 0)
		: QAbstractTableModel(parent) {}
	virtual ~AbstractUpdatableTableModel() {}
	virtual void update() = 0;
};

struct Filter {
	bool useItemTypeFilter;
	Id itemTypeFilter;
	bool usePlaceFilter;
	Id placeFilter;
	bool useWrittenOffFilter;
	bool writtenOffFilter;

	Filter();
};

struct Item {
	Id id;
	Id itemTypeId;
	QString itemType;
	Id placeId;
	QString place;
	Id responsiblePersonId;
	QString responsiblePerson;
	QString name;
	int inn;
	bool writtenOff;
	bool underRepair;
	bool checked;
	QString note;
};

class InventoryModel : public AbstractUpdatableTableModel {
	Q_OBJECT
	Q_DISABLE_COPY(InventoryModel);
public:
	InventoryModel(QObject * parent = 0);
	virtual ~InventoryModel() {}

	virtual Qt::ItemFlags flags(const QModelIndex & index) const;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
	virtual bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
	virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

	virtual Id idAt(int row) const;
	virtual void setItemTypeFilter(Id itemType);
	virtual void switchItemTypeFilter(bool on = true);
	virtual void setPlaceFilter(Id place);
	virtual void switchPlaceFilter(bool on = true);
	virtual void setWrittenOffFilter(bool writtenOff);
	virtual void switchWrittenOffFilter(bool on = true);

	virtual void update();
private:
	Filter filter;
	QList<Item> items;
};

struct HistoryRecord {
	int id;
	QDateTime changeTime;
	int field;
	QString oldValue;
	QString newValue;
};

class HistoryModel : public QAbstractTableModel {
	Q_OBJECT
	Q_DISABLE_COPY(HistoryModel);
public:
	HistoryModel(int item, QObject * parent = 0);
	virtual ~HistoryModel() {}

	virtual Qt::ItemFlags flags(const QModelIndex & index) const;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
private:
	QList<HistoryRecord> records;
};

struct ItemGroup {
	QString itemType;
	int itemTypeId;
	QString name;
	int itemCount;
	QString place;
	int placeId;
	int inn;
	bool writtenOff;
};

class PrintableInventoryModel : public AbstractUpdatableTableModel {
	Q_OBJECT
	Q_DISABLE_COPY(PrintableInventoryModel);
public:
	PrintableInventoryModel(QObject * parent = 0);
	virtual ~PrintableInventoryModel() {}

	virtual Qt::ItemFlags flags(const QModelIndex & index) const;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;

	virtual void setItemTypeFilter(Id itemType);
	virtual void switchItemTypeFilter(bool on = true);
	virtual void setPlaceFilter(Id place);
	virtual void switchPlaceFilter(bool on = true);
	virtual void setWrittenOffFilter(bool writtenOff);
	virtual void switchWrittenOffFilter(bool on = true);

	virtual void update();
private:
	QList<ItemGroup> groups;
	Filter filter;
};

struct RefRecord {
	Id id;
	QString name;
};

struct RefType {
	int type;
	QString table;
	QString foreignKey;
};

class ReferenceModel : public AbstractUpdatableTableModel {
	Q_OBJECT
	Q_DISABLE_COPY(ReferenceModel);
public:
	enum { INVALID = 0, ITEM_TYPES, PLACES, PERSONS, REF_TYPE_COUNT };

	ReferenceModel(int newType, QObject * parent = 0);
	virtual ~ReferenceModel() {}

	virtual Qt::ItemFlags flags(const QModelIndex & index) const;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	virtual bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
	virtual bool insertRows(int row, int count, const QModelIndex & parent = QModelIndex());
	virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

	virtual Id idAt(int row) const;
	int countOfItemsUsing(Id id);
	int type() const;
	virtual bool addMultiline(const QStringList & lines);

	virtual void update();
private:
	QList<RefRecord> records;
	RefType refType;
};

}
