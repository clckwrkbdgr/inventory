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
	beginResetModel();
	list = Inventory::instance()->places();
	endResetModel();
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
		try {
			return list[index.row()].name();
		} catch(IDatabaseObject::DBErrorException e) {
			return tr("### Database error ###");
		}

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
		try {
			list[index.row()].setName(value.toString());
			emit dataChanged(index, index);
			return true;
		} catch(IDatabaseObject::DBErrorException e) {
			return false;
		} catch(INamed::EmptyNameException e) {
			return false;
		}
	}

	return false;
}

bool PlaceModel::insertRows(int row, int count, const QModelIndex&) {
	beginInsertRows(QModelIndex(), row, row + count - 1);

	try {
		for(int i = 0; i < count; i++) {
			list.insert(row, Place::add(tr("New place")));
		}
	} catch(IDatabaseObject::DBErrorException e) {
		endInsertRows();
		return false;
	} catch(INamed::EmptyNameException e) {
		endInsertRows();
		return false;
	}

	endInsertRows();
	return true;
}

bool PlaceModel::removeRows(int row, int count, const QModelIndex&) {
	beginRemoveRows(QModelIndex(), row, row + count - 1);

	for(int i = 0; i < count; i++)
		if(row >= 0 && row < list.size()) {
			try {
				if(!Place::remove(list[row].getId())) {
					endRemoveRows();
					return false;
				}
				list.removeAt(row);
			} catch(IDatabaseObject::DBErrorException e) {
				endRemoveRows();
				return false;
			}
		}

	endRemoveRows();
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
	beginResetModel();
	list = Inventory::instance()->itemTypes();
	endResetModel();
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
		try {
			return list[index.row()].name();
		} catch(IDatabaseObject::DBErrorException e) {
			return tr("### Database error ###");
		}

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
		try {
			list[index.row()].setName(value.toString());
			emit dataChanged(index, index);
			return true;
		} catch(IDatabaseObject::DBErrorException e) {
			return false;
		} catch(INamed::EmptyNameException e) {
			return false;
		}
	}

	return false;
}

bool ItemTypeModel::insertRows(int row, int count, const QModelIndex&) {
	beginInsertRows(QModelIndex(), row, row + count - 1);

	try {
		for(int i = 0; i < count; i++) {
			list.insert(row, ItemType::add(tr("New item type")));
		}
	} catch(IDatabaseObject::DBErrorException e) {
		endInsertRows();
		return false;
	} catch(INamed::EmptyNameException e) {
		endInsertRows();
		return false;
	}

	endInsertRows();
	return true;
}

bool ItemTypeModel::removeRows(int row, int count, const QModelIndex&) {
	beginRemoveRows(QModelIndex(), row, row + count - 1);

	for(int i = 0; i < count; i++)
		if(row >= 0 && row < list.size()) {
			try {
				if(!Place::remove(list[row].getId())) {
					endRemoveRows();
					return false;
				}
				list.removeAt(row);
			} catch(IDatabaseObject::DBErrorException e) {
				endRemoveRows();
				return false;
			}
		}

	endRemoveRows();
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
	beginResetModel();
	list = itemList;
	endResetModel();
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
	
	if(index.column() == ActiveField)
		return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	try {
		if(!list[index.row()].isActive())
			return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	} catch(IDatabaseObject::DBErrorException e) {
		return Qt::ItemIsEnabled;
	}

	return QAbstractItemModel::flags(index) | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant InventoryModel::data(const QModelIndex &index, int role) const {
	if(!index.isValid() || index.row() >= list.size() || index.column() >= FieldCount)
		return QVariant();

	Item item = list[index.row()];
	if(!item.isActive() && role == Qt::ForegroundRole)
		return qApp->palette().color(QPalette::Disabled, QPalette::Text);

	// @todo: Grey backgr for disabled items.
	try {
		switch(index.column()) {
			case ItemTypeField: {
				if(role == Qt::DisplayRole)
					return item.itemType().name();
				if(role == Qt::EditRole)
					return item.itemType().getId();

				break;
			}
			case NameField: {
				if(role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)
					return item.name();

				break;
			}
			case InnField: {
				if(item.innExists()) {
					if(role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)
						return item.inn();
				} else {
					if(role == Qt::EditRole)
						return 0;
				}

				break;
			}
			case PlaceField: {
				if(role == Qt::DisplayRole)
					return item.place().name();
				if(role == Qt::EditRole)
					return item.place().getId();

				break;
			}
			case ActiveField: {
				if(role == Qt::CheckStateRole && item.isActive())
					return Qt::Checked;

				break;
			}
			case NoteField: {
				if(role == Qt::DisplayRole || role == Qt::EditRole || role == Qt::ToolTipRole)
					return item.note();
				// @todo: Icon (for not empty) and input dialog.

				break;
			}
		}
	} catch(IDatabaseObject::DBErrorException e) {
		return tr("### Database error ###");
	}

	return QVariant();
}

QVariant InventoryModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
		switch(section) {
			case ItemTypeField: return tr("Item type");
			case NameField: return tr("Name");
			case InnField: return tr("INN");
			case PlaceField: return tr("Place");
			case ActiveField: return tr("Active");
			case NoteField: return tr("Note");
			default: return QVariant();
		}

	return QVariant();
}

int InventoryModel::rowCount(const QModelIndex&) const {
	return list.size();
}

int InventoryModel::columnCount(const QModelIndex&) const {
	return FieldCount;
}

void InventoryModel::sort(int column, Qt::SortOrder order) {
	switch(column) {
		case ItemTypeField: {
			beginResetModel();

			QMultiMap<QString, Item> map;
			bool success = true;
			try {
				foreach(Item item, list)
					map.insert(item.itemType().name(), item);
			} catch(IDatabaseObject::DBErrorException e) {
				success = false;
			}
			if(success) list = map.values();

			break;
		}
		case NameField: {
			beginResetModel();

			QMultiMap<QString, Item> map;
			bool success = true;
			try {
				foreach(Item item, list)
					map.insert(item.name(), item);
			} catch(IDatabaseObject::DBErrorException e) {
				success = false;
			}
			if(success) list = map.values();

			break;
		}
		case InnField: {
			beginResetModel();

			QMultiMap<int, Item> map;
			bool success = true;
			try {
				foreach(Item item, list) {
					if(item.innExists())
						map.insert(item.inn(), item);
					else
						map.insert(-1, item);
				}
			} catch(IDatabaseObject::DBErrorException e) {
				success = false;
			}
			if(success) list = map.values();

			break;
		}
		case PlaceField: {
			beginResetModel();

			QMultiMap<QString, Item> map;
			bool success = true;
			try {
				foreach(Item item, list)
					map.insert(item.place().name(), item);
			} catch(IDatabaseObject::DBErrorException e) {
				success = false;
			}
			if(success) list = map.values();

			break;
		}
		case ActiveField: {
			beginResetModel();

			QMultiMap<bool, Item> map;
			bool success = true;
			try {
				foreach(Item item, list)
					map.insert(item.isActive(), item);
			} catch(IDatabaseObject::DBErrorException e) {
				success = false;
			}
			if(success) list = map.values();

			break;
		}
		case NoteField: {
			beginResetModel();

			QMultiMap<QString, Item> map;
			bool success = true;
			try {
				foreach(Item item, list)
					map.insert(item.note(), item);
			} catch(IDatabaseObject::DBErrorException e) {
				success = false;
			}
			if(success) list = map.values();

			break;
		}
		default: return;
	}

	if(order == Qt::DescendingOrder) {
		for(int i = 0; i < list.size(); i++)
			list.insert(i, list.takeLast());
	}

	endResetModel();
}

bool InventoryModel::setData(const QModelIndex &index, const QVariant &value, int role) {
	if(!index.isValid() || index.row() >= list.size() || index.column() >= FieldCount)
		return false;

	Item item = list[index.row()];
	//if(!item.isActive())
		//return false;

	if(role == Qt::EditRole) {
		try {
			switch(index.column()) {
				case ItemTypeField : item.setItemType(ItemType(value.toInt())); break; 
				case NameField     : item.setName(value.toString()); break; 
				case InnField      : item.setInn(value.toInt()); break; 
				case PlaceField    : item.setPlace(Place(value.toInt())); break; 
				case ActiveField   : item.deactivate(); break; 
				case NoteField     : item.setNote(value.toString()); break; // @todo: Icon (for not empty) and input dialog.
				default: return false;
			}
		} catch(INamed::EmptyNameException e) {
			return false;
		} catch(IDatabaseObject::DBErrorException e) {
			return false;
		} catch(Item::InactiveException e) {
			return false;
		}
		emit dataChanged(index, index);
		return true;
	}

	return false;
}

bool InventoryModel::insertRows(int row, int count, const QModelIndex&) {
	beginInsertRows(QModelIndex(), row, row + count - 1);

	try {
		for(int i = 0; i < count; i++) {
			list.insert(row, Item::add(tr("New item")));
		}
	} catch(IDatabaseObject::DBErrorException e) {
		endInsertRows();
		return false;
	} catch(INamed::EmptyNameException e) {
		endInsertRows();
		return false;
	}

	endInsertRows();
	return true;
}

bool InventoryModel::removeRows(int row, int count, const QModelIndex&) {
	beginRemoveRows(QModelIndex(), row, row + count - 1);

	try {
		for(int i = 0; i < count; i++)
			if(row >= 0 && row < list.size()) {
				Item::remove(list[row].getId());
				list.removeAt(row);
			}
	} catch(IDatabaseObject::DBErrorException e) {
		endRemoveRows();
		return false;
	}

	endRemoveRows();
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
	try {
		if(index.isValid() && index.column() == InventoryModel::ItemTypeField) {
			QComboBox *comboBox = new QComboBox(parent);
			ItemTypeModel *model = new ItemTypeModel(comboBox);
			comboBox->setModel(model);
			return comboBox;
		}
		
		if(index.isValid() && index.column() == InventoryModel::PlaceField) {
			QComboBox *comboBox = new QComboBox(parent);
			PlaceModel *model = new PlaceModel(comboBox);
			comboBox->setModel(model);
			return comboBox;
		}
	} catch(INamed::EmptyNameException e) {
		return QItemDelegate::createEditor(parent, option, index);
	} catch(IDatabaseObject::DBErrorException e) {
		return QItemDelegate::createEditor(parent, option, index);
	} catch(IIdObject::InvalidIdException e) {
		return QItemDelegate::createEditor(parent, option, index);
	}
	
	return QItemDelegate::createEditor(parent, option, index);
}

void InventoryDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	try {
		if(index.isValid() && index.column() == InventoryModel::ItemTypeField) {
			QComboBox *comboBox = static_cast<QComboBox*>(editor);
			ItemTypeModel *model = static_cast<ItemTypeModel*>(comboBox->model());
			int id = index.model()->data(index, Qt::EditRole).toInt();

			comboBox->setCurrentIndex(model->indexOf(ItemType(id)));
			return;
		}
		
		if(index.isValid() && index.column() == InventoryModel::PlaceField) {
			QComboBox *comboBox = static_cast<QComboBox*>(editor);
			PlaceModel *model = static_cast<PlaceModel*>(comboBox->model());
			int id = index.model()->data(index, Qt::EditRole).toInt();

			comboBox->setCurrentIndex(model->indexOf(Place(id)));
			return;
		}
	} catch(INamed::EmptyNameException e) {
		QItemDelegate::setEditorData(editor, index);
		return;
	} catch(IDatabaseObject::DBErrorException e) {
		QItemDelegate::setEditorData(editor, index);
		return;
	} catch(IIdObject::InvalidIdException e) {
		QItemDelegate::setEditorData(editor, index);
		return;
	}

	QItemDelegate::setEditorData(editor, index);
}

void InventoryDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	try {
		if(index.isValid() && index.column() == InventoryModel::ItemTypeField) {
			QComboBox *comboBox = static_cast<QComboBox*>(editor);
			ItemTypeModel *itemTypeModel = static_cast<ItemTypeModel*>(comboBox->model());
			int id = itemTypeModel->idAt(comboBox->currentIndex());

			model->setData(index, id, Qt::EditRole);
			return;
		}
		
		if(index.isValid() && index.column() == InventoryModel::PlaceField) {
			QComboBox *comboBox = static_cast<QComboBox*>(editor);
			PlaceModel *placeModel = static_cast<PlaceModel*>(comboBox->model());
			int id = placeModel->idAt(comboBox->currentIndex());

			model->setData(index, id, Qt::EditRole);
			return;
		}
	} catch(INamed::EmptyNameException e) {
		QItemDelegate::setModelData(editor, model, index);
		return;
	} catch(IDatabaseObject::DBErrorException e) {
		QItemDelegate::setModelData(editor, model, index);
		return;
	} catch(IIdObject::InvalidIdException e) {
		QItemDelegate::setModelData(editor, model, index);
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
	beginResetModel();
	list = Inventory::instance()->historyOf(item);
	endResetModel();
}

Qt::ItemFlags HistoryModel::flags(const QModelIndex &index) const {
	if(index.isValid())
		return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	return Qt::ItemIsEnabled;
}

QVariant HistoryModel::data(const QModelIndex &index, int role) const {
	if(!index.isValid())
		return QVariant();

	if(index.row() >= list.size() || index.column() >= FieldCount)
		return QVariant();

	if(role == Qt::DisplayRole || role == Qt::ToolTipRole) {
		try {
			switch(index.column()) {
				case TimeField: return list[index.row()].dateTime().toString(); break; 
				case NameField: 
					switch(list[index.row()].changedField())  {
						case History::ItemTypeField: return tr("Item type");
						case History::NameField: return tr("Name");
						case History::NoteField: return tr("Note");
						case History::InnField: return tr("Inn");
						case History::PlaceField: return tr("Place");
						case History::ActiveField: return tr("Activity");
						default: return tr("Unknown field");
					}
				case OldValueField: return list[index.row()].oldValue(); 
				case NewValueField: return list[index.row()].newValue(); 
				default: break;
			}
		} catch(IDatabaseObject::DBErrorException e) {
			return tr("### Database exception ###");
		}
	}

	return QVariant();
}

QVariant HistoryModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if(role == Qt::DisplayRole && orientation == Qt::Horizontal)
		switch(section) {
			case TimeField: return tr("Change time");
			case NameField: return tr("Field");
			case OldValueField: return tr("Old value");
			case NewValueField: return tr("New value");
			default: return QString();
		}

	return QVariant();
}

int HistoryModel::rowCount(const QModelIndex &) const {
	return list.size();
}

int HistoryModel::columnCount(const QModelIndex &) const {
	return FieldCount;
}

QModelIndex HistoryModel::index(int row, int column, const QModelIndex &) const {
	return createIndex(row, column, row);
}

QModelIndex HistoryModel::parent(const QModelIndex &) const {
	return QModelIndex();
}

