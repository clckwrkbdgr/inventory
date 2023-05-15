#pragma once

#include <QtCore/QAbstractTableModel>
#include <QtCore/QDateTime>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtCore/QStringList>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QItemDelegate>

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

class InventoryDelegate : public QItemDelegate {
	Q_OBJECT
	Q_DISABLE_COPY(InventoryDelegate)
public:
	InventoryDelegate(QObject *parent = 0);
	virtual ~InventoryDelegate();

	virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
	virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
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
	virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

	virtual Id idAt(int row) const;
	virtual void setItemTypeFilter(Id itemType);
	virtual void switchItemTypeFilter(bool on = true);
	virtual void setPlaceFilter(Id place);
	virtual void switchPlaceFilter(bool on = true);
	virtual void setWrittenOffFilter(bool writtenOff);
	virtual void switchWrittenOffFilter(bool on = true);

	void switchAllWrittenOff(bool on = true);
	void switchAllRepair(bool on = true);
	void switchAllChecked(bool on = true);

	virtual void update();
signals:
	void switchingProcessUpdated(int i);
	void preparedToUpdate();
private:
	Filter filter;
	QList<Item> items;

	bool sorted;
	int sortColumn;
	Qt::SortOrder sortOrder;
};

struct HistoryRecord {
	int id;
	QDateTime changeTime;
	int field;
	QVariant rawOldValue, rawNewValue;
	QString oldValue;
	QString newValue;
};

class HistoryModel : public QAbstractTableModel {
	Q_OBJECT
	Q_DISABLE_COPY(HistoryModel);
public:
	HistoryModel(Id item, QObject * parent = 0);
	virtual ~HistoryModel() {}

	virtual Qt::ItemFlags flags(const QModelIndex & index) const;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
	virtual bool removeRows(int row, int count, const QModelIndex & parent = QModelIndex());

	virtual Id idAt(int row) const;
private:
	Id itemId;
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

	virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

	virtual void update();
private:
	QList<ItemGroup> groups;
	Filter filter;

	bool sorted;
	int sortColumn;
	Qt::SortOrder sortOrder;
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
	virtual int rowOf(Id id) const;
	int countOfItemsUsing(Id id);
	int type() const;
	virtual bool addMultiline(const QStringList & lines);

	virtual void update();
private:
	QList<RefRecord> records;
	RefType refType;
};

}
