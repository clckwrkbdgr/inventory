#include "models.h"

namespace Inventory { // InventoryModel

InventoryModel::InventoryModel(QObject * /*parent*/)
{
	return;
}

Qt::ItemFlags InventoryModel::flags(const QModelIndex & /*index*/) const
{
	return 0;
}

QVariant InventoryModel::data(const QModelIndex & /*index*/, int /*role*/) const
{
	return QVariant();
}

bool InventoryModel::setData(const QModelIndex & /*index*/, const QVariant & /*value*/, int /*role*/)
{
	return false;
}

QVariant InventoryModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int /*role*/) const
{
	return QVariant();
}

int InventoryModel::rowCount(const QModelIndex & /*parent*/) const
{
	return 0;
}

int InventoryModel::columnCount(const QModelIndex & /*parent*/) const
{
	return 0;
}

bool InventoryModel::insertRows(int /*row*/, int /*count*/, const QModelIndex & /*parent*/)
{
	return false;
}

bool InventoryModel::removeRows(int /*row*/, int /*count*/, const QModelIndex & /*parent*/)
{
	return false;
}

Id InventoryModel::idAt(int /*row*/) const
{
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


/*
Предметы:
	Столбцов: 7
	Строк: сколько будет заргружено.
	Ячейки:
		Тип: отображение названия для чтения и список для редактирования.
		Место: то же самое.
		Лицо: то же самое.
		Наименование: текст.
		ИНН: Пусто, если нет, иначе сам ИНН.
			При правке надо следить, чтобы был уникальным или пустым
			Если неуникальным, показать сообщение об этом.
			Если пустым, сделать НУЛЛ.
		Пометка о списании: слово, если списано. Иначе - пусто.
		Пометка о ремонте - то же самое.
		Пометка о проверке - то же самое.
		Примечание - текст.
	Установить ячейки:
		При изменении поля: записать историю.
		Обновить таблицу.
	Добавить строки:
		Добавить строку вниз со значениями по умолчанию и обновить.
		Если установлен фильтр, использовать его значения.
		Если списки пусты, добавить в пустые списки дефолтные значения и использовать их.
		И обновить.
	Удалить строки:
		Удалить также всю связанную историю.
	Установить фильтр по типу(тип).
	Включить фильтр по типу(вкл|выкл).
	Установить фильтр по месту(место).
	Включить фильтр по месту(вкл|выкл).
	Установить фильтр по списанию(списан|нет).
	Включить фильтр по списанию.
*/

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

ReferenceModel::ReferenceModel(int /*type*/, QObject * /*parent*/)
{
	return;
}

Qt::ItemFlags ReferenceModel::flags(const QModelIndex & /*index*/) const
{
	return 0;
}

QVariant ReferenceModel::data(const QModelIndex & /*index*/, int /*role*/) const
{
	return QVariant();
}

bool ReferenceModel::setData(const QModelIndex & /*index*/, const QVariant & /*value*/, int /*role*/)
{
	return false;
}

QVariant ReferenceModel::headerData(int /*section*/, Qt::Orientation /*orientation*/, int /*role*/) const
{
	return QVariant();
}

int ReferenceModel::rowCount(const QModelIndex & /*parent*/) const
{
	return 0;
}

int ReferenceModel::columnCount(const QModelIndex & /*parent*/) const
{
	return 0;
}

bool ReferenceModel::insertRows(int /*row*/, int /*count*/, const QModelIndex & /*parent*/)
{
	return false;
}

bool ReferenceModel::removeRows(int /*row*/, int /*count*/, const QModelIndex & /*parent*/)
{
	return false;
}

Id ReferenceModel::idAt(int /*row*/) const
{
	return Id();
}

int ReferenceModel::type() const
{
	return 0;
}

bool ReferenceModel::addMultiline(const QStringList & /*lines*/)
{
	return false;
}


/*
Справочник:
	При создании указывается тип справочника.
	Столбцов: 1
	Строк: сколько загружено.
	Ячейки:
		Название: строка.
	Установить ячейки:
		Обновить список.
	Добавить строку:
		Добавить пустую строку и обновить.
	Удалить строку:
		Бросить исключение, если есть ещё предметы, используещие указанные значения.
	Добавить скопом(список):
		Добавить скопом, установив каждое наименование соответственно.
		*/

}
