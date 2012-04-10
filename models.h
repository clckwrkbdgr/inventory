#pragma once

#include <QtCore/QAbstractTableModel>

namespace Inventory {

typedef int Id;

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

class ReferenceModel : public QAbstractTableModel {
	Q_OBJECT
	Q_DISABLE_COPY(ReferenceModel);
public:
	enum { INVALID = 0, ITEM_TYPES, PLACES, PERSONS, COUNT };

	ReferenceModel(int type, QObject * parent = 0);
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
};

}
