#pragma once

#include <QtCore/QObject>
#include <QtCore/QDateTime>
#include <QtCore/QList>
#include <QtCore/QString>
#include <QtSql/QSqlQuery>

class QAbstractItemModel;

// @todo: - throws
// @todo: active, IIdObject - rename.
class IIdObject {
public:
	class InvalidIdException {};

	virtual int getId() const = 0;
};

class IDatabaseObject {
public:
	class DBErrorException {
	public:
		QSqlQuery query;
		DBErrorException(const QSqlQuery &query) {
			this->query = query;
		}
	};
};

class INamed {
public:
	class EmptyNameException {};

	virtual QString name() const = 0;
	virtual void setName(const QString &value) = 0;
};

// @todo: item type and place are children of the common base class, so their models are, and so the add list function are working with model, not DB
class Place : virtual public IIdObject, virtual public IDatabaseObject, virtual public INamed {
public:
	Place(int id);
	Place(const Place &other);
	const Place& operator=(const Place &other);
	virtual ~Place();

	virtual int getId() const;
	virtual QString name() const;
	virtual void setName(const QString &value);

	virtual bool operator==(const Place &other) const;

	static Place add(const QString &newName);
private:
	int id;
};

class ItemType : virtual public IIdObject, virtual public IDatabaseObject, virtual public INamed {
public:
	ItemType(int id);
	ItemType(const ItemType &other);
	const ItemType& operator=(const ItemType &other);
	virtual ~ItemType();

	virtual int getId() const;
	virtual QString name() const;
	virtual void setName(const QString &value);

	virtual bool operator==(const ItemType &other) const;

	static ItemType add(const QString &newName);
private:
	int id;
};

class Item : virtual public IIdObject, virtual public IDatabaseObject, virtual public INamed {
public:
	class InactiveException {};

	Item(int id);
	Item(const Item &other);
	const Item& operator=(const Item &other);
	virtual ~Item();

	virtual int getId() const;
	virtual ItemType itemType() const;
	virtual QString name() const;
	virtual QString note() const;
	virtual int inn() const;
	virtual bool innExists() const;
	virtual Place place() const;
	virtual bool isActive() const;

	virtual void setItemType(const ItemType &value);
	virtual void setName(const QString &value);
	virtual void setNote(const QString &value);
	virtual void setInn(int value);
	virtual void setPlace(const Place &value);
	virtual void deactivate();

	virtual bool operator==(const Item &other) const;

	static Item add(const ItemType &newItemType, const QString &newName, const Place &newPlace);
	static Item add(const QString &newName);
	static void remove(int id);
private:
	void addHistory(int changedField, const QString &oldValue, const QString &newValue);
	int id;
};


class History : virtual public IIdObject, virtual public IDatabaseObject {
public:
	enum ItemField {InvalidField = 0, ItemTypeField = 1, NameField = 2, NoteField = 3, InnField = 4, PlaceField = 5, ActiveField = 6};

	History(int id);
	History(const History &other);
	const History& operator=(const History &other);
	virtual ~History();

	virtual int getId() const;
	virtual Item item() const;
	virtual QDateTime dateTime() const;
	virtual ItemField changedField() const;
	virtual QString oldValue() const;
	virtual QString newValue() const;
private:
	int id;
};

class Log : virtual public IIdObject, virtual public IDatabaseObject {
public:
	Log(int id);
	Log(const Log &other);
	const Log& operator=(const Log &other);
	virtual ~Log();

	virtual int getId() const;
	virtual QDateTime dateTime() const;
	virtual QString text() const;

	static Log add(const QString &newText);
private:
	int id;
};

// @todo: ItemType and Place instead of id's are returned. Probably, data hiding.
struct InventoryViewFilter {
	bool usePlace;
	int placeId;
	bool useItemType;
	int itemTypeId;
	bool useActivity;
	bool active;
};

class Inventory : virtual public IDatabaseObject {
	Q_DISABLE_COPY(Inventory);

	Inventory() {}
	~Inventory() {}
public:
	class DBOpenErrorException {};
	class DBIsClosedException {};

	// @todo: database object lists as thin clients and models.
	QList<Item> items() const;
	QList<Item> filteredItems(const Place *place, const ItemType *itemType, const bool* activity) const;
	QList<Item> filteredItems(const InventoryViewFilter &filter) const;
	QList<Item> itemsByPlace(const Place &place) const;
	QList<Item> itemsByItemType(const ItemType &itemType) const;
	QList<Item> itemsByActivity(bool active) const;
	QList<Place> places() const;
	QList<ItemType> itemTypes() const;

	QList<Log> logByTime(QDateTime startTime, QDateTime stopTime) const;
	QList<History> historyOf(const Item &item) const;

	QSqlQuery getQuery(int sortingColumn = -1, Qt::SortOrder order = Qt::AscendingOrder);

	static bool isOpen();
	static void open(const QString &databaseFileName);
	static void close();
	
	static Inventory* instance();
	static InventoryViewFilter getLastFilter();
};

