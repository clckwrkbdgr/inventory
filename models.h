#pragma once

#include <QtCore/QAbstractTableModel>
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
	static QString lastError();
	static void setDatabaseName(const QString & newName);
	static bool query(const QString & text);
	static bool query(const QString & prepared, const Placeholders & placeholders);
	static QSqlQuery select(const QString & text);
private:
	Database() {}
	~Database() {}

	static QString databaseName;
};

typedef int Id;

struct Item {
	Id id;
	Id itemTypeId;
	QString itemType;
	Id placeId;
	QString place;
	Id responsiblePersonId;
	QString responsiblePerson;
	QString name;
	bool innIsNotNull;
	int inn;
	bool writtenOff;
	bool underRepair;
	bool checked;
	QString note;
};

class InventoryModel : public QAbstractTableModel {
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
private:
	QList<Item> items;
	void update();
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
};

class PrintableInventoryModel : public QAbstractTableModel {
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
};

struct RefRecord {
	Id id;
	QString name;
};

struct RefType {
	int type;
	QString table;
};

class ReferenceModel : public QAbstractTableModel {
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
	int type() const;
	virtual bool addMultiline(const QStringList & lines);
private:
	QList<RefRecord> records;
	RefType refType;
	void update();
};

}
