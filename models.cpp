#include <QtDebug>
#include <QtGui/QApplication>
#include <QtGui/QPalette>
#include <QtGui/QComboBox>

#include "models.h"

PlaceModel::PlaceModel(QObject *parent) : QAbstractItemModel(parent) {
	updateList();
}

PlaceModel::~PlaceModel() {

}

void PlaceModel::updateList() {
#if QT_VERSION >= 0x040600
	beginResetModel();
#endif
	list = Inventory::instance()->places();
#if QT_VERSION >= 0x040600
	endResetModel();
#else
	reset();
#endif
}

int PlaceModel::indexOf(const Place &place) const {
	return list.indexOf(place);
}

int PlaceModel::idAt(int row) const {
	if(0 <= row && row < list.size())
		return list[row].getId();
	return -1;
}

Qt::ItemFlags PlaceModel::flags(const QModelIndex &index) const {
	if(index.isValid())
		return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;

	return Qt::ItemIsEnabled;
}

QVariant PlaceModel::data(const QModelIndex &index, int role) const {
	if(!index.isValid())
		return QVariant();

	if(index.row() >= list.size() || index.column() >= 1)
		return QVariant();

	if(role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)
		return list[index.row()].name();

	return QVariant();
}

QVariant PlaceModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if(role == Qt::DisplayRole && orientation == Qt::Horizontal && section == 1)
		return tr("Name");

	return QVariant();
}

int PlaceModel::rowCount(const QModelIndex&) const {
	return list.size();
}

int PlaceModel::columnCount(const QModelIndex&) const {
	return 1;
}

bool PlaceModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if(index.isValid() && index.row() < list.size() && index.column() < 1 &&
					(role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)) {
		list[index.row()].setName(value.toString());
		emit dataChanged(index, index);
		return true;
	}

	return false;
}

bool PlaceModel::insertRows(int row, int count, const QModelIndex&) {
	beginInsertRows(QModelIndex(), row, row + count - 1);

	for(int i = 0; i < count; i++) {
		list.insert(row, Place::add(tr("New place")));
	}

	endInsertRows();
	return true;
}

QModelIndex PlaceModel::index(int row, int column, const QModelIndex &) const {
	return createIndex(row, column, row);
}

QModelIndex PlaceModel::parent(const QModelIndex&) const {
	return QModelIndex();
}

//----------------------------------------------------------------------------------------------------

ItemTypeModel::ItemTypeModel(QObject *parent) : QAbstractItemModel(parent) {
	updateList();
}

ItemTypeModel::~ItemTypeModel() {

}

void ItemTypeModel::updateList() {
	#if QT_VERSION >= 0x040600
		beginResetModel();
	#endif
	list = Inventory::instance()->itemTypes();
#if QT_VERSION >= 0x040600
	endResetModel();
#else
	reset();
#endif
}

int ItemTypeModel::indexOf(const ItemType &itemType) const {
	return list.indexOf(itemType);
}

int ItemTypeModel::idAt(int row) const {
	if(0 <= row && row < list.size())
		return list[row].getId();
	return -1;
}

Qt::ItemFlags ItemTypeModel::flags(const QModelIndex &index) const {
	if(index.isValid())
		return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;

	return Qt::ItemIsEnabled;
}

QVariant ItemTypeModel::data(const QModelIndex &index, int role) const {
	if(!index.isValid())
		return QVariant();

	if(index.row() >= list.size() || index.column() >= 1)
		return QVariant();

	if(role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)
		return list[index.row()].name();

	return QVariant();
}

QVariant ItemTypeModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if(role == Qt::DisplayRole && orientation == Qt::Horizontal && section == 1)
		return tr("Name");

	return QVariant();
}

int ItemTypeModel::rowCount(const QModelIndex&) const {
	return list.size();
}

int ItemTypeModel::columnCount(const QModelIndex&) const {
	return 1;
}

bool ItemTypeModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if(index.isValid() && index.row() < list.size() && index.column() < 1 &&
					(role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)) {
		list[index.row()].setName(value.toString());
		emit dataChanged(index, index);
		return true;
	}

	return false;
}

bool ItemTypeModel::insertRows(int row, int count, const QModelIndex&) {
	beginInsertRows(QModelIndex(), row, row + count - 1);

	for(int i = 0; i < count; i++) {
		list.insert(row, ItemType::add(tr("New item type")));
	}

	endInsertRows();
	return true;
}

QModelIndex ItemTypeModel::index(int row, int column, const QModelIndex &) const {
	return createIndex(row, column, row);
}

QModelIndex ItemTypeModel::parent(const QModelIndex&) const {
	return QModelIndex();
}

//----------------------------------------------------------------------------------------------------

InventoryModel::InventoryModel(QObject *parent) : QAbstractItemModel(parent) {

}

InventoryModel::~InventoryModel() {

}

void InventoryModel::updateList(const QList<Item> &itemList) {
#if QT_VERSION >= 0x040600
	beginResetModel();
#endif
	list = itemList;
#if QT_VERSION >= 0x040600
	endResetModel();
#else
	reset();
#endif
}

int InventoryModel::indexOf(const Item &item) const {
	return list.indexOf(item);
}

int InventoryModel::idAt(int row) const {
	if(0 <= row && row < list.size())
		return list[row].getId();
	return -1;
}

Qt::ItemFlags InventoryModel::flags(const QModelIndex &index) const {
	if(!index.isValid())
		return Qt::ItemIsEnabled;
	
	if(index.column() == 5)
		return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	if(!list[index.row()].isActive())
		return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant InventoryModel::data(const QModelIndex &index, int role) const {
	if(!index.isValid() || index.row() >= list.size() || index.column() >= 6)
		return QVariant();

	Item item = list[index.row()];
	if(!item.isActive() && role == Qt::ForegroundRole)
		return qApp->palette().color(QPalette::Disabled, QPalette::Text);

	// @todo: Grey backgr for disabled items.
	switch(index.column()) {
		case 0: { // Item type.
			if(role == Qt::DisplayRole)
				return item.itemType().name();
			if(role == Qt::EditRole)
				return item.itemType().getId(); // @todo: Combobox.

			break;
		}
		case 1: { // Name.
			if(role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)
				return item.name();

			break;
		}
		case 2: { // Note.
			if(role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)
				return item.note();
			// @todo: Icon (for not empty) and input dialog.

			break;
		}
		case 3: { // INN.
			if(item.innExists()) {
				if(role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)
					return item.inn();
			} else {
				if(role == Qt::EditRole)
					return 0; // @todo: Spinbox.
			}

			break;
		}
		case 4: { // Place.
			if(role == Qt::DisplayRole)
				return item.place().name();
			if(role == Qt::EditRole)
				return item.place().getId(); // @todo: Combobox.

			break;
		}
		case 5: { // Active.
			if(role == Qt::CheckStateRole && item.isActive())
				return Qt::Checked;

			break;
		}
	}

	return QVariant();
}

QVariant InventoryModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
		switch(section) {
			case 0: return tr("Item type");
			case 1: return tr("Name");
			case 2: return tr("Note");
			case 3: return tr("INN");
			case 4: return tr("Place");
			case 5: return tr("Active");
			default: return QVariant();
		}

	return QVariant();
}

int InventoryModel::rowCount(const QModelIndex&) const {
	return list.size();
}

int InventoryModel::columnCount(const QModelIndex&) const {
	return 6;
}

void InventoryModel::sort(int column, Qt::SortOrder order) {
	switch(column) {
		case 0: {
#if QT_VERSION >= 0x040600
				beginResetModel();
#endif

			QMultiMap<QString, Item> map;
			foreach(Item item, list)
				map.insert(item.itemType().name(), item);
			list = map.values();

			break;
		}
		case 1: {
#if QT_VERSION >= 0x040600
				beginResetModel();
#endif

			QMultiMap<QString, Item> map;
			foreach(Item item, list)
				map.insert(item.name(), item);
			list = map.values();

			break;
		}
		case 2: {
#if QT_VERSION >= 0x040600
				beginResetModel();
#endif

			QMultiMap<QString, Item> map;
			foreach(Item item, list)
				map.insert(item.note(), item);
			list = map.values();

			break;
		}
		case 3: {
#if QT_VERSION >= 0x040600
				beginResetModel();
#endif

			QMultiMap<int, Item> map;
			foreach(Item item, list) {
				if(item.innExists())
					map.insert(item.inn(), item);
				else
					map.insert(-1, item);
			}
			list = map.values();

			break;
		}
		case 4: {
#if QT_VERSION >= 0x040600
				beginResetModel();
#endif

			QMultiMap<QString, Item> map;
			foreach(Item item, list)
				map.insert(item.place().name(), item);
			list = map.values();

			break;
		}
		case 5: {
#if QT_VERSION >= 0x040600
				beginResetModel();
#endif

			QMultiMap<bool, Item> map;
			foreach(Item item, list)
				map.insert(item.isActive(), item);
			list = map.values();

			break;
		}
		default: return;
	}

	if(order == Qt::DescendingOrder) {
		for(int i = 0; i < list.size(); i++)
			list.insert(i, list.takeLast());
	}

#if QT_VERSION >= 0x040600
	endResetModel();
#else
	reset();
#endif
}

bool InventoryModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if(!index.isValid() || index.row() >= list.size() || index.column() >= 6)
		return false;

	Item item = list[index.row()];
	if(!item.isActive())
		return false;

	switch(index.column()) {
		case 0: { // Item type.
			if(role == Qt::EditRole) {
				item.setItemType(ItemType(value.toInt()));
				emit dataChanged(index, index);
				return true; // @todo: Combobox.
			}

			break;
		}
		case 1: { // Name.
			if(role == Qt::EditRole) {
				item.setName(value.toString());
				emit dataChanged(index, index);
				return true;
			}

			break;
		}
		case 2: { // Note.
			if(role == Qt::EditRole) {
				item.setNote(value.toString());
				emit dataChanged(index, index);
				return true;
			}
			// @todo: Icon (for not empty) and input dialog.

			break;
		}
		case 3: { // INN.
			if(role == Qt::EditRole) {
				item.setInn(value.toInt());
				emit dataChanged(index, index);
				return true; // @todo: Spinbox.
			}

			break;
		}
		case 4: { // Place.
			if(role == Qt::EditRole) {
				item.setPlace(Place(value.toInt()));
				emit dataChanged(index, index);
				return true; // @todo: Combobox.
			}

			break;
		}
		case 5: { // Active.
			if(role == Qt::EditRole) {
				item.deactivate();
				emit dataChanged(index, index);
				return true;
			}

			break;
		}
		default: return false;
	}

	return false;
}

bool InventoryModel::insertRows(int row, int count, const QModelIndex&) {
	beginInsertRows(QModelIndex(), row, row + count - 1);

	QList<ItemType> types = Inventory::instance()->itemTypes();
	if(types.isEmpty())
		ItemType::add(tr("New item type"));
	ItemType itemType = Inventory::instance()->itemTypes()[0];

	QList<Place> places = Inventory::instance()->places();
	if(places.isEmpty())
		Place::add(tr("New place"));
	Place place = Inventory::instance()->places()[0];

	for(int i = 0; i < count; i++) {
		list.insert(row, Item::add(itemType, tr("New item"), place));
	}

	endInsertRows();
	return true;
}

QModelIndex InventoryModel::index(int row, int column, const QModelIndex&) const {
	return createIndex(row, column, row);
}

QModelIndex InventoryModel::parent(const QModelIndex&) const {
	return QModelIndex();
}

//----------------------------------------------------------------------------------------------------

InventoryDelegate::InventoryDelegate(QObject *parent) : QItemDelegate(parent) {

}

InventoryDelegate::~InventoryDelegate() {

}

QWidget* InventoryDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
	if(index.isValid() && index.column() == 0) {
		QComboBox *comboBox = new QComboBox(parent);
		ItemTypeModel *model = new ItemTypeModel(comboBox);
		comboBox->setModel(model);
		return comboBox;
	}
	
	if(index.isValid() && index.column() == 4) {
		QComboBox *comboBox = new QComboBox(parent);
		PlaceModel *model = new PlaceModel(comboBox);
		comboBox->setModel(model);
		return comboBox;
	}
	
	return QItemDelegate::createEditor(parent, option, index);
}

void InventoryDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	if(index.isValid() && index.column() == 0) {
		QComboBox *comboBox = static_cast<QComboBox*>(editor);
		ItemTypeModel *model = static_cast<ItemTypeModel*>(comboBox->model());
		int id = index.model()->data(index, Qt::EditRole).toInt();

		comboBox->setCurrentIndex(model->indexOf(ItemType(id)));
		return;
	}
	
	if(index.isValid() && index.column() == 4) {
		QComboBox *comboBox = static_cast<QComboBox*>(editor);
		PlaceModel *model = static_cast<PlaceModel*>(comboBox->model());
		int id = index.model()->data(index, Qt::EditRole).toInt();

		comboBox->setCurrentIndex(model->indexOf(Place(id)));
		return;
	}

	QItemDelegate::setEditorData(editor, index);
}

void InventoryDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	if(index.isValid() && index.column() == 0) {
		QComboBox *comboBox = static_cast<QComboBox*>(editor);
		ItemTypeModel *itemTypeModel = static_cast<ItemTypeModel*>(comboBox->model());
		int id = itemTypeModel->idAt(comboBox->currentIndex());

		model->setData(index, id, Qt::EditRole);
		return;
	}
	
	if(index.isValid() && index.column() == 4) {
		QComboBox *comboBox = static_cast<QComboBox*>(editor);
		PlaceModel *placeModel = static_cast<PlaceModel*>(comboBox->model());
		int id = placeModel->idAt(comboBox->currentIndex());

		model->setData(index, id, Qt::EditRole);
		return;
	}

	QItemDelegate::setModelData(editor, model, index);
}

void InventoryDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &) const {
	editor->setGeometry(option.rect);
}

//----------------------------------------------------------------------------------------------------

HistoryModel::HistoryModel(const Item &newItem, QObject *parent) : QAbstractItemModel(parent), item(newItem) {
	updateList();
}

HistoryModel::~HistoryModel() {

}

void HistoryModel::updateList() {
#if QT_VERSION >= 0x040600
	beginResetModel();
#endif
	list = Inventory::instance()->historyOf(item);
#if QT_VERSION >= 0x040600
	endResetModel();
#else
	reset();
#endif
}

Qt::ItemFlags HistoryModel::flags(const QModelIndex &index) const {
	if(index.isValid())
		return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	return Qt::ItemIsEnabled;
}

QVariant HistoryModel::data(const QModelIndex &index, int role) const {
	if(!index.isValid())
		return QVariant();

	if(index.row() >= list.size() || index.column() >= 4)
		return QVariant();

	switch(index.column()) {
		case 0: {
			if(role == Qt::DisplayRole || role == Qt::ToolTipRole)
				return list[index.row()].dateTime().toString();
			break;
		}
		case 1: {
			if(role == Qt::DisplayRole || role == Qt::ToolTipRole)
				switch(list[index.row()].changedField()) {
					case History::ItemTypeField: return tr("Item type");
					case History::NameField: return tr("Name");
					case History::NoteField: return tr("Note");
					case History::InnField: return tr("Inn");
					case History::PlaceField: return tr("Place");
					case History::ActiveField: return tr("Activity");
					default: return tr("Unknown field");
				}
		}
		case 2: {
			if(role == Qt::DisplayRole || role == Qt::ToolTipRole)
				return list[index.row()].oldValue();
		}
		case 3: {
			if(role == Qt::DisplayRole || role == Qt::ToolTipRole)
				return list[index.row()].newValue();
		}
		default: break;
	}

	return QVariant();
}

QVariant HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
		switch(section) {
			case 0: return tr("Change time");
			case 1: return tr("Field");
			case 2: return tr("Old value");
			case 3: return tr("New value");
			default: return QString();
		}

	return QVariant();
}

int HistoryModel::rowCount(const QModelIndex &) const {
	return list.size();
}

int HistoryModel::columnCount(const QModelIndex &) const {
	return 4;
}

QModelIndex HistoryModel::index(int row, int column, const QModelIndex &) const {
	return createIndex(row, column, row);
}

QModelIndex HistoryModel::parent(const QModelIndex &) const {
	return QModelIndex();
}

