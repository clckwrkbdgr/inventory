#pragma once

#include <QtCore/QAbstractItemModel>
#include <QtGui/QItemDelegate>

#include "inventory.h"

// @todo: - error handling ,exception catching
// @todo: - all number and text constants are contansts
// @todo: show note icon in name field, and tooltip, and edit note in inputdialog via context menu.
class PlaceModel : public QAbstractItemModel {
	Q_OBJECT
public:
	PlaceModel(QObject *parent = 0); // Update list.
	virtual ~PlaceModel();

	void updateList();
	int indexOf(const Place &place) const;
	int idAt(int row) const;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const; // Tool tip.
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &index) const;
private:
	QList<Place> list;

};

class ItemTypeModel : public QAbstractItemModel {
	Q_OBJECT
public:
	ItemTypeModel(QObject *parent = 0); // Update list.
	virtual ~ItemTypeModel();

	void updateList();
	int indexOf(const ItemType &itemType) const;
	int idAt(int row) const;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const; // Tool tip.
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &index) const;
private:
	QList<ItemType> list;

};


class InventoryModel : public QAbstractItemModel {
	Q_OBJECT
public:
	InventoryModel(QObject *parent = 0);
	virtual ~InventoryModel();

	void updateList(const QList<Item> &itemList);
	int indexOf(const Item &item) const;
	int idAt(int row) const;

	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	// @todo - reformat item view, for name at last, active at first etc.
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const; // Tool tip.
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

	virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
	virtual bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &index) const;
private:
	QList<Item> list;

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


class HistoryModel : public QAbstractItemModel {
	Q_OBJECT
public:
	HistoryModel(const Item &item, QObject *parent = 0); // Update list.
	virtual ~HistoryModel();

	void updateList();

	virtual Qt::ItemFlags flags(const QModelIndex &index) const; // Read-only.
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const; // Tool tip.
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;

	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &index) const;
private:
	Item item;
	QList<History> list;
};

