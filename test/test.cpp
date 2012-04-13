#include <QtTest/QtTest>
#include <QtDebug>
#include "models.h"

using namespace Inventory;

class InventoryTest: public QObject {
	Q_OBJECT
	enum { ITEM_TYPE = 0, PLACE, PERSON, NAME, INN, WRITTEN_OFF, REPAIR, CHECKED, NOTES };
private slots:
	void initTestCase() {
		Database::setDatabaseName("inventory.sqlite");
		QVERIFY(Database::db().isOpen());

		QScopedPointer<ReferenceModel> places(new ReferenceModel(ReferenceModel::PLACES));
		QVERIFY(places);
		QScopedPointer<ReferenceModel> persons(new ReferenceModel(ReferenceModel::PERSONS));
		QVERIFY(persons);
		QScopedPointer<ReferenceModel> types(new ReferenceModel(ReferenceModel::ITEM_TYPES));
		QVERIFY(types);
		QScopedPointer<InventoryModel> items(new InventoryModel());
		QVERIFY(items);
		QScopedPointer<HistoryModel> history(new HistoryModel(0));
		QVERIFY(history);
	}
	void cleanupTestCase() {
		Database::close();
		QVERIFY(!Database::db().isOpen());
	}
	void referenceTypes_data() {
		QTest::addColumn<int>("original");
		QTest::addColumn<int>("result");

		QTest::newRow("item types") << int(ReferenceModel::ITEM_TYPES) << int(ReferenceModel::ITEM_TYPES);
		QTest::newRow("places")     << int(ReferenceModel::PLACES)     << int(ReferenceModel::PLACES);
		QTest::newRow("persons")    << int(ReferenceModel::PERSONS)    << int(ReferenceModel::PERSONS);
		QTest::newRow("invalid")    << int(ReferenceModel::INVALID)    << int(ReferenceModel::INVALID);
		QTest::newRow("too big")    << int(ReferenceModel::REF_TYPE_COUNT)      << int(ReferenceModel::INVALID);
	}
	void referenceTypes() {
		QFETCH(int, original);
		QFETCH(int, result);
		bool ok = false;

		QScopedPointer<ReferenceModel> model(new ReferenceModel(original));
		QVERIFY(model);
		QCOMPARE(model->type(), result);
		if(model->type() == ReferenceModel::INVALID) {
			QCOMPARE(0, model->rowCount());
			QCOMPARE(0, model->columnCount());
			ok = model->insertRow(0); QVERIFY(!ok); QCOMPARE(0, model->rowCount());
			return;
		}
		QCOMPARE(model->rowCount(), 0);
		QCOMPARE(model->columnCount(), 1);
		ok = model->insertColumn(0);
		QVERIFY(!ok);
		QCOMPARE(model->columnCount(), 1);
	}
	void referenceHeaderData_data() {
		QTest::addColumn<QVariant>("value");
		QTest::addColumn<int>("section");
		QTest::addColumn<int>("orientation");

		QTest::newRow("name")          << QVariant("Name") << 0 << int(Qt::Horizontal);
		QTest::newRow("too far right") << QVariant()       << 1 << int(Qt::Horizontal);
		QTest::newRow("vertical")      << QVariant()       << 0 << int(Qt::Vertical);
	}
	void referenceHeaderData() {
		QFETCH(QVariant, value);
		QFETCH(int, section);
		QFETCH(int, orientation);

		QScopedPointer<ReferenceModel> model(new ReferenceModel(ReferenceModel::PLACES));
		QVERIFY(model);
		QCOMPARE(model->headerData(section, Qt::Orientation(orientation), Qt::DisplayRole), value);
	}
	void referenceRowManipulation_data() {
		QTest::addColumn<bool>("insert");
		QTest::addColumn<int>("pos");
		QTest::addColumn<int>("count");
		QTest::addColumn<bool>("success");
		QTest::addColumn<int>("rowcount");

		QTest::newRow("insert at top") << true  <<  0 << 1 << true  << 1;
		QTest::newRow("insert at bottom") << true  <<  1 << 2 << true  << 3;
		QTest::newRow("insert invalid") << true  << 10 << 1 << false << 3;
		QTest::newRow("remove at top") << false <<  0 << 1 << true  << 2;
		QTest::newRow("remove invalid") << false << 10 << 1 << false << 2;
		QTest::newRow("insert no rows") << true  <<  1 << 0 << false << 2;
		QTest::newRow("remove at bottom") << false <<  1 << 1 << true  << 1;
	}
	void referenceRowManipulation() {
		QFETCH(bool, insert);
		QFETCH(int, pos);
		QFETCH(int, count);
		QFETCH(bool, success);
		QFETCH(int, rowcount);

		QScopedPointer<ReferenceModel> model(new ReferenceModel(ReferenceModel::PLACES));
		QVERIFY(model);

		bool ok = false;
		ok = insert ? model->insertRows(pos, count) : model->removeRows(pos, count);
		QCOMPARE(ok, success);
		QCOMPARE(model->rowCount(), rowcount);
	}
	void referenceCellData_data() {
		QTest::addColumn<int>("row");
		QTest::addColumn<int>("column");
		QTest::addColumn<QVariant>("value");
		QTest::addColumn<bool>("success");
		QTest::addColumn<int>("flags");

		QTest::newRow("test")        << 0 << 0 << QVariant("Test")  << true  << int(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
		QTest::newRow("test1")       << 1 << 0 << QVariant("Test1") << true  << int(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
		QTest::newRow("test2")       << 2 << 0 << QVariant("Test")  << true  << int(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
		QTest::newRow("invalid row") << 3 << 0 << QVariant()        << false << int(Qt::NoItemFlags);
		QTest::newRow("invalid col") << 0 << 1 << QVariant("Test")  << false << int(Qt::NoItemFlags);

		QScopedPointer<ReferenceModel> model(new ReferenceModel(ReferenceModel::PLACES));
		QVERIFY(model);

		bool ok = model->removeRows(0, model->rowCount());
		QVERIFY(ok);
		QCOMPARE(model->rowCount(), 0);

		ok = model->insertRows(0, 3);
		QVERIFY(ok);
		QCOMPARE(model->rowCount(), 3);

	}
	void referenceCellData() {
		QFETCH(int, row);
		QFETCH(int, column);
		QFETCH(QVariant, value);
		QFETCH(bool, success);
		QFETCH(int, flags);

		QScopedPointer<ReferenceModel> model(new ReferenceModel(ReferenceModel::PLACES));
		QVERIFY(model);

		QModelIndex index = model->index(row, column);
		QCOMPARE(model->flags(index), Qt::ItemFlags(flags));
		bool ok = model->setData(index, value);
		QCOMPARE(ok, success);
		if(ok) {
			QCOMPARE(model->data(index), value);
		}
	}
	void referenceMultiline() {
		QScopedPointer<ReferenceModel> model(new ReferenceModel(ReferenceModel::PLACES));
		QVERIFY(model);

		bool ok = model->removeRows(0, model->rowCount());
		QVERIFY(ok);
		QCOMPARE(model->rowCount(), 0);

		ok = model->insertRow(0);
		QVERIFY(ok);
		QCOMPARE(model->rowCount(), 1);

		ok = model->addMultiline(QStringList() << "First line" << "Second line");
		QVERIFY(ok);
		QCOMPARE(model->rowCount(), 3);

		QCOMPARE(           model->data(model->index(0, 0)), QVariant(""));
		QCOMPARE( model->data(model->index(1, 0)), QVariant("First line"));
		QCOMPARE(model->data(model->index(2, 0)), QVariant("Second line"));
	}

	void inventoryHeaderData_data() {
		QTest::addColumn<QVariant>("value");
		QTest::addColumn<int>("section");
		QTest::addColumn<int>("orientation");

		QTest::newRow("item type")     << QVariant("Item type")    << 0 << int(Qt::Horizontal);
		QTest::newRow("place")         << QVariant("Place")        << 1 << int(Qt::Horizontal);
		QTest::newRow("person")        << QVariant("Responsible person") << 2 << int(Qt::Horizontal);
		QTest::newRow("name")          << QVariant("Name")         << 3 << int(Qt::Horizontal);
		QTest::newRow("inn")           << QVariant("INN")          << 4 << int(Qt::Horizontal);
		QTest::newRow("writtenoff")    << QVariant("Written off")  << 5 << int(Qt::Horizontal);
		QTest::newRow("repair")        << QVariant("Under repair") << 6 << int(Qt::Horizontal);
		QTest::newRow("checked")       << QVariant("Checked")      << 7 << int(Qt::Horizontal);
		QTest::newRow("note")          << QVariant("Note")         << 8 << int(Qt::Horizontal);
		QTest::newRow("too far right") << QVariant()               << 9 << int(Qt::Horizontal);
		QTest::newRow("vertical")      << QVariant()               << 0 << int(Qt::Vertical);
	}
	void inventoryHeaderData() {
		QFETCH(QVariant, value);
		QFETCH(int, section);
		QFETCH(int, orientation);

		QScopedPointer<InventoryModel> model(new InventoryModel());
		QVERIFY(model);
		QCOMPARE(model->columnCount(), 9);
		QCOMPARE(model->headerData(section, Qt::Orientation(orientation), Qt::DisplayRole), value);
	}
	void inventoryRowManipulation_data() {
		QTest::addColumn<bool>("insert");
		QTest::addColumn<int>("pos");
		QTest::addColumn<int>("count");
		QTest::addColumn<bool>("success");
		QTest::addColumn<int>("rowcount");

		QTest::newRow("insert at top")    << true  <<  0 << 1 << true  << 1;
		QTest::newRow("insert at bottom") << true  <<  1 << 2 << true  << 3;
		QTest::newRow("insert invalid")   << true  << 10 << 1 << false << 3;
		QTest::newRow("remove at top")    << false <<  0 << 1 << true  << 2;
		QTest::newRow("remove invalid")   << false << 10 << 1 << false << 2;
		QTest::newRow("insert no rows")   << true  <<  1 << 0 << false << 2;
		QTest::newRow("remove at bottom") << false <<  1 << 1 << true  << 1;
	}
	void inventoryRowManipulation() {
		QFETCH(bool, insert);
		QFETCH(int, pos);
		QFETCH(int, count);
		QFETCH(bool, success);
		QFETCH(int, rowcount);

		QScopedPointer<InventoryModel> model(new InventoryModel());
		QVERIFY(model);

		bool ok = false;
		ok = insert ? model->insertRows(pos, count) : model->removeRows(pos, count);
		QCOMPARE(ok, success);
		QCOMPARE(model->rowCount(), rowcount);
	}
	void inventoryTextCells_data() {
		QTest::addColumn<int>("row");
		QTest::addColumn<int>("column");
		QTest::addColumn<QVariant>("editvalue");
		QTest::addColumn<QVariant>("displayvalue");
		QTest::addColumn<bool>("success");
		QTest::addColumn<int>("flags");

		const int R = 3;
		const int INVALID    = Qt::NoItemFlags;
		QTest::newRow("invalid row") << R << 0 << QVariant()       << QVariant() << false << INVALID;

		QScopedPointer<InventoryModel> model(new InventoryModel());
		QVERIFY(model);

		bool ok = false;
		const int C = model->columnCount();
		const int PLAIN_EDIT = Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;

		ok = model->removeRows(0, model->rowCount()); QVERIFY(ok); QCOMPARE(model->rowCount(), 0);
		ok = model->insertRows(0, R);                     QVERIFY(ok); QCOMPARE(model->rowCount(), R);

		QTest::newRow("invalid col") << 0 << C << QVariant("Test") << QVariant() << false << INVALID;

		QTest::newRow("NAME 0")     << 0 << int(NAME) << QVariant("Test")   << QVariant("Test")   << true << PLAIN_EDIT;
		QTest::newRow("NAME 1")     << 1 << int(NAME) << QVariant("Test 1") << QVariant("Test 1") << true << PLAIN_EDIT;
		QTest::newRow("NAME empty") << 1 << int(NAME) << QVariant("      ") << QVariant("      ") << true << PLAIN_EDIT;

		QTest::newRow("NOTE 0")     << 0 << int(NOTES) << QVariant("Test")   << QVariant("Test")   << true << PLAIN_EDIT;
		QTest::newRow("NOTE 0")     << 1 << int(NOTES) << QVariant("Test 1") << QVariant("Test 1") << true << PLAIN_EDIT;
		QTest::newRow("NOTE empty") << 1 << int(NOTES) << QVariant("      ") << QVariant("      ") << true << PLAIN_EDIT;

		QTest::newRow("INN")        << 0 << int(INN) << QVariant("1") << QVariant("1") << true  << PLAIN_EDIT;
		QTest::newRow("INN empty")  << 0 << int(INN) << QVariant("")  << QVariant("")  << true  << PLAIN_EDIT;
		QTest::newRow("INN null")   << 0 << int(INN) << QVariant()    << QVariant("")  << false << PLAIN_EDIT;
		QTest::newRow("INN alpha")  << 2 << int(INN) << QVariant("a") << QVariant("")  << false << PLAIN_EDIT;
		QTest::newRow("INN space")  << 2 << int(INN) << QVariant(" ") << QVariant("")  << false << PLAIN_EDIT;
		QTest::newRow("INN uniq 1") << 2 << int(INN) << QVariant("1") << QVariant("1") << true  << PLAIN_EDIT;
		QTest::newRow("INN 2")      << 1 << int(INN) << QVariant("2") << QVariant("2") << true  << PLAIN_EDIT;
		QTest::newRow("INN uniq 2") << 1 << int(INN) << QVariant("1") << QVariant("1") << true  << PLAIN_EDIT;
	}
	void inventoryTextCells() {
		QFETCH(int, row);
		QFETCH(int, column);
		QFETCH(QVariant, editvalue);
		QFETCH(QVariant, displayvalue);
		QFETCH(bool, success);
		QFETCH(int, flags);

		QScopedPointer<InventoryModel> model(new InventoryModel());
		QVERIFY(model);

		QModelIndex index = model->index(row, column);
		QCOMPARE(model->flags(index), Qt::ItemFlags(flags));
		bool ok = model->setData(index, editvalue, Qt::EditRole);
		QCOMPARE(ok, success);
		if(ok) {
			QCOMPARE(model->data(index, Qt::DisplayRole), displayvalue);
			QCOMPARE(model->data(index, Qt::EditRole), editvalue);
		}
	}
	void inventorySwitchCells_data() {
		QTest::addColumn<int>("row");
		QTest::addColumn<int>("column");
		QTest::addColumn<QVariant>("checkvalue");
		QTest::addColumn<QVariant>("displayvalue");
		QTest::addColumn<bool>("success");
		QTest::addColumn<int>("flags");

		QScopedPointer<InventoryModel> model(new InventoryModel());
		QVERIFY(model);

		bool ok = false;
		const int R = 3, C = model->columnCount();
		const int INVALID    = Qt::NoItemFlags;
		const int SWITCH_EDIT = Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
		const QVariant checked = QVariant(Qt::Checked);
		const QVariant unchecked = QVariant(Qt::Unchecked);
		const QVariant STR_WRITTEN_OFF = "Written off", STR_REPAIR = "Under repair", STR_CHECKED = "Checked", UNCHECKED = "";

		QTest::newRow("invalid row") << R << 0 << QVariant()        << QVariant() << false << INVALID;
		QTest::newRow("invalid col") << 0 << C << QVariant("Test")  << QVariant() << false << INVALID;

		ok = model->removeRows(0, model->rowCount()); QVERIFY(ok); QCOMPARE(model->rowCount(), 0);
		ok = model->insertRows(0, R);                     QVERIFY(ok); QCOMPARE(model->rowCount(), R);

		QTest::newRow("written off checked")   << 0 << int(WRITTEN_OFF) << checked   << STR_WRITTEN_OFF << true << SWITCH_EDIT;
		QTest::newRow("written off unchecked") << 0 << int(WRITTEN_OFF) << unchecked << UNCHECKED       << true << SWITCH_EDIT;
		QTest::newRow("repair checked")        << 1 << int(REPAIR)      << checked   << STR_REPAIR      << true << SWITCH_EDIT;
		QTest::newRow("repair unchecked")      << 2 << int(REPAIR)      << unchecked << UNCHECKED       << true << SWITCH_EDIT;
		QTest::newRow("checked checked")       << 2 << int(CHECKED)     << checked   << STR_CHECKED     << true << SWITCH_EDIT;
		QTest::newRow("checked unchecked")     << 2 << int(CHECKED)     << unchecked << UNCHECKED       << true << SWITCH_EDIT;
	}
	void inventorySwitchCells() {
		QFETCH(int, row);
		QFETCH(int, column);
		QFETCH(QVariant, checkvalue);
		QFETCH(QVariant, displayvalue);
		QFETCH(bool, success);
		QFETCH(int, flags);

		QScopedPointer<InventoryModel> model(new InventoryModel());
		QVERIFY(model);

		QModelIndex index = model->index(row, column);
		QCOMPARE(int(model->flags(index)), int(Qt::ItemFlags(flags)));
		bool ok = model->setData(index, displayvalue, Qt::EditRole);
		QVERIFY(!ok);
		ok = model->setData(index, checkvalue, Qt::CheckStateRole);
		QCOMPARE(ok, success);
		if(ok) {
			QCOMPARE(model->data(index, Qt::DisplayRole),    displayvalue);
			QCOMPARE(model->data(index, Qt::CheckStateRole), checkvalue);
		}
	}
	void inventoryRefCells_data() {
		QTest::addColumn<int>("row");
		QTest::addColumn<int>("column");
		QTest::addColumn<QVariant>("editvalue");
		QTest::addColumn<QVariant>("displayvalue");
		QTest::addColumn<bool>("success");
		QTest::addColumn<int>("flags");

		QScopedPointer<InventoryModel> model(new InventoryModel());
		QVERIFY(model);

		bool ok = false;
		const int R = 3;
		const int INVALID    = Qt::NoItemFlags;
		const int PLAIN_EDIT = Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
		const int C = model->columnCount();

		QTest::newRow("invalid row") << R << 0 << QVariant()       << QVariant() << false << INVALID;
		QTest::newRow("invalid col") << 0 << C << QVariant("Test") << QVariant() << false << INVALID;

		ok = model->removeRows(0, model->rowCount()); QVERIFY(ok); QCOMPARE(model->rowCount(), 0);

		QScopedPointer<ReferenceModel> itemTypes(new ReferenceModel(ReferenceModel::ITEM_TYPES));
			QVERIFY(itemTypes);
			itemTypes->removeRows(0, itemTypes->rowCount());
			itemTypes->insertRows(0, 3);
			itemTypes->setData(itemTypes->index(0, 0), "Type 1");
			itemTypes->setData(itemTypes->index(1, 0), "Type 2");
			itemTypes->setData(itemTypes->index(2, 0), "Type 3");
			QCOMPARE(itemTypes->rowCount(), 3);
		QScopedPointer<ReferenceModel> places   (new ReferenceModel(ReferenceModel::PLACES));
			QVERIFY(places);
			places->removeRows(0, places->rowCount());
			places->insertRows(0, 3);
			places->setData(places->index(0, 0), "Place 1");
			places->setData(places->index(1, 0), "Place 2");
			places->setData(places->index(2, 0), "Place 3");
			QCOMPARE(places->rowCount(), 3);
		QScopedPointer<ReferenceModel> persons  (new ReferenceModel(ReferenceModel::PERSONS));
			QVERIFY(persons);
			persons->removeRows(0, persons->rowCount());
			persons->insertRows(0, 3);
			persons->setData(persons->index(0, 0), "Person 1");
			persons->setData(persons->index(1, 0), "Person 2");
			persons->setData(persons->index(2, 0), "Person 3");
			QCOMPARE(persons->rowCount(), 3);

		ok = model->insertRows(0, R); QVERIFY(ok); QCOMPARE(model->rowCount(), R);

		QTest::newRow("type 1")     << 0 << int(ITEM_TYPE) << QVariant(itemTypes->idAt(1)) << QVariant("Type 2") << true  << PLAIN_EDIT;
		QTest::newRow("type 0")     << 1 << int(ITEM_TYPE) << QVariant(itemTypes->idAt(0)) << QVariant("Type 1") << true  << PLAIN_EDIT;
		QTest::newRow("type at 5")  << 1 << int(ITEM_TYPE) << QVariant(itemTypes->idAt(5)) << QVariant("Type 1") << false << PLAIN_EDIT;
		QTest::newRow("type at -1") << 1 << int(ITEM_TYPE) << QVariant(-1)                 << QVariant("Type 1") << false << PLAIN_EDIT;

		QTest::newRow("place 1")     << 0 << int(PLACE) << QVariant(places->idAt(1)) << QVariant("Place 2") << true  << PLAIN_EDIT;
		QTest::newRow("place 0")     << 1 << int(PLACE) << QVariant(places->idAt(0)) << QVariant("Place 1") << true  << PLAIN_EDIT;
		QTest::newRow("place at 5")  << 1 << int(PLACE) << QVariant(places->idAt(5)) << QVariant("Place 1") << false << PLAIN_EDIT;
		QTest::newRow("place at -1") << 1 << int(PLACE) << QVariant(-1)              << QVariant("Place 1") << false << PLAIN_EDIT;

		QTest::newRow("person 1")     << 0 << int(PERSON) << QVariant(persons->idAt(1)) << QVariant("Person 2") << true  << PLAIN_EDIT;
		QTest::newRow("person 0")     << 1 << int(PERSON) << QVariant(persons->idAt(0)) << QVariant("Person 1") << true  << PLAIN_EDIT;
		QTest::newRow("person at 5")  << 1 << int(PERSON) << QVariant(persons->idAt(5)) << QVariant("Person 1") << false << PLAIN_EDIT;
		QTest::newRow("person at -1") << 1 << int(PERSON) << QVariant(-1)               << QVariant("Person 1") << false << PLAIN_EDIT;
	}
	void inventoryRefCells() {
		QFETCH(int, row);
		QFETCH(int, column);
		QFETCH(QVariant, editvalue);
		QFETCH(QVariant, displayvalue);
		QFETCH(bool, success);
		QFETCH(int, flags);

		QScopedPointer<InventoryModel> model(new InventoryModel());
		QVERIFY(model);

		QModelIndex index = model->index(row, column);
		QCOMPARE(int(model->flags(index)), int(Qt::ItemFlags(flags)));
		bool ok = model->setData(index, editvalue, Qt::EditRole);
		QCOMPARE(ok, success);
		if(ok) {
			QCOMPARE(model->data(index, Qt::DisplayRole), displayvalue);
			QCOMPARE(model->data(index, Qt::EditRole), editvalue);
		}
	}
	void inventoryFilter_data() {
		QTest::addColumn<int>("count");
		QTest::addColumn<bool>("itemtypefilter");
		QTest::addColumn<QVariant>("itemtype");
		QTest::addColumn<bool>("placefilter");
		QTest::addColumn<QVariant>("place");
		QTest::addColumn<bool>("writtenofffilter");
		QTest::addColumn<QVariant>("writtenoff");

		QTest::newRow("invalid") << 8 << false << QVariant(-1) << false << QVariant(-1) << false << QVariant(0);

		bool ok = false;

		QScopedPointer<InventoryModel> model(new InventoryModel());
		QVERIFY(model);
		ok = model->removeRows(0, model->rowCount()); QVERIFY(ok); QCOMPARE(model->rowCount(), 0);
		QScopedPointer<ReferenceModel> itemTypes(new ReferenceModel(ReferenceModel::ITEM_TYPES));
			QVERIFY(itemTypes);
			itemTypes->removeRows(0, itemTypes->rowCount());
			itemTypes->insertRows(0, 3);
			itemTypes->setData(itemTypes->index(0, 0), "Type 1");
			itemTypes->setData(itemTypes->index(1, 0), "Type 2");
			itemTypes->setData(itemTypes->index(2, 0), "Type 3");
		QScopedPointer<ReferenceModel> places   (new ReferenceModel(ReferenceModel::PLACES));
			QVERIFY(places);
			places->removeRows(0, places->rowCount());
			places->insertRows(0, 3);
			places->setData(places->index(0, 0), "Place 1");
			places->setData(places->index(1, 0), "Place 2");
			places->setData(places->index(2, 0), "Place 3");
		ok = model->insertRows(0, 8); QVERIFY(ok); QCOMPARE(model->rowCount(), 8);

		model->setData(model->index(0, ITEM_TYPE),   itemTypes->idAt(0));
		model->setData(model->index(0, PLACE),       places->idAt(0));
		model->setData(model->index(0, WRITTEN_OFF), Qt::Checked);

		model->setData(model->index(1, ITEM_TYPE),   itemTypes->idAt(0));
		model->setData(model->index(1, PLACE),       places->idAt(0));
		model->setData(model->index(1, WRITTEN_OFF), Qt::Unchecked);

		model->setData(model->index(2, ITEM_TYPE),   itemTypes->idAt(1));
		model->setData(model->index(2, PLACE),       places->idAt(0));
		model->setData(model->index(2, WRITTEN_OFF), Qt::Checked);

		model->setData(model->index(3, ITEM_TYPE),   itemTypes->idAt(1));
		model->setData(model->index(3, PLACE),       places->idAt(0));
		model->setData(model->index(3, WRITTEN_OFF), Qt::Unchecked);

		model->setData(model->index(4, ITEM_TYPE),   itemTypes->idAt(0));
		model->setData(model->index(4, PLACE),       places->idAt(1));
		model->setData(model->index(4, WRITTEN_OFF), Qt::Checked);

		model->setData(model->index(5, ITEM_TYPE),   itemTypes->idAt(0));
		model->setData(model->index(5, PLACE),       places->idAt(1));
		model->setData(model->index(5, WRITTEN_OFF), Qt::Unchecked);

		model->setData(model->index(6, ITEM_TYPE),   itemTypes->idAt(1));
		model->setData(model->index(6, PLACE),       places->idAt(1));
		model->setData(model->index(6, WRITTEN_OFF), Qt::Checked);

		model->setData(model->index(7, ITEM_TYPE),   itemTypes->idAt(1));
		model->setData(model->index(7, PLACE),       places->idAt(1));
		model->setData(model->index(7, WRITTEN_OFF), Qt::Unchecked);

		QTest::newRow("000")   << 8 << false << QVariant(itemTypes->idAt(0)) << false << QVariant(places->idAt(0)) << false << QVariant(Qt::Unchecked);
		QTest::newRow("100")   << 4 << true  << QVariant(itemTypes->idAt(1)) << false << QVariant(places->idAt(0)) << false << QVariant(Qt::Unchecked);
		QTest::newRow("010")   << 4 << false << QVariant(itemTypes->idAt(1)) << true  << QVariant(places->idAt(0)) << false << QVariant(Qt::Unchecked);
		QTest::newRow("110")   << 2 << true  << QVariant(itemTypes->idAt(0)) << true  << QVariant(places->idAt(1)) << false << QVariant(Qt::Checked);
		QTest::newRow("001")   << 4 << false << QVariant(itemTypes->idAt(0)) << false << QVariant(places->idAt(1)) << true  << QVariant(Qt::Checked);
		QTest::newRow("101")   << 2 << true  << QVariant(itemTypes->idAt(1)) << false << QVariant(places->idAt(1)) << true  << QVariant(Qt::Checked);
		QTest::newRow("011")   << 2 << false << QVariant(itemTypes->idAt(1)) << true  << QVariant(places->idAt(1)) << true  << QVariant(Qt::Unchecked);
		QTest::newRow("111 1") << 1 << true  << QVariant(itemTypes->idAt(1)) << true  << QVariant(places->idAt(1)) << true  << QVariant(Qt::Checked);
		QTest::newRow("111 2") << 1 << true  << QVariant(itemTypes->idAt(0)) << true  << QVariant(places->idAt(0)) << true  << QVariant(Qt::Unchecked);
		QTest::newRow("111 3") << 1 << true  << QVariant(itemTypes->idAt(1)) << true  << QVariant(places->idAt(0)) << true  << QVariant(Qt::Checked);
	}
	void inventoryFilter() {
		QFETCH(int, count);
		QFETCH(bool, itemtypefilter);
		QFETCH(QVariant, itemtype);
		QFETCH(bool, placefilter);
		QFETCH(QVariant, place);
		QFETCH(bool, writtenofffilter);
		QFETCH(QVariant, writtenoff);

		QScopedPointer<InventoryModel> model(new InventoryModel());
		QVERIFY(model);
		model->switchItemTypeFilter(itemtypefilter);
		model->setItemTypeFilter(itemtype.toInt());
		model->switchPlaceFilter(placefilter);
		model->setPlaceFilter(place.toInt());
		model->switchWrittenOffFilter(writtenofffilter);
		model->setWrittenOffFilter(writtenoff.toBool());

		QCOMPARE(model->rowCount(), count);

		if(itemtypefilter) {
			for(int row = 0; row < model->rowCount(); ++row) {
				QCOMPARE(model->data(model->index(row, ITEM_TYPE), Qt::EditRole), itemtype);
			}
		}

		if(placefilter) {
			for(int row = 0; row < model->rowCount(); ++row) {
				QCOMPARE(model->data(model->index(row, PLACE), Qt::EditRole), place);
			}
		}

		if(writtenofffilter) {
			for(int row = 0; row < model->rowCount(); ++row) {
				QCOMPARE(model->data(model->index(row, WRITTEN_OFF), Qt::EditRole), writtenoff);
			}
		}
	}

	void historyHeaderData_data() {
		QTest::addColumn<QVariant>("value");
		QTest::addColumn<int>("section");
		QTest::addColumn<int>("orientation");

		QTest::newRow("time")  << QVariant("Change time") << 0 << int(Qt::Horizontal);
		QTest::newRow("field") << QVariant("Field name")  << 1 << int(Qt::Horizontal);
		QTest::newRow("old")   << QVariant("Old value")   << 2 << int(Qt::Horizontal);
		QTest::newRow("new")   << QVariant("New value")   << 3 << int(Qt::Horizontal);
	}
	void historyHeaderData() {
		QFETCH(QVariant, value);
		QFETCH(int, section);
		QFETCH(int, orientation);

		QScopedPointer<HistoryModel> model(new HistoryModel(0));
		QVERIFY(model);
		QCOMPARE(model->columnCount(), 4);
		QCOMPARE(model->rowCount(), 0);
		QCOMPARE(model->headerData(section, Qt::Orientation(orientation), Qt::DisplayRole), value);
	}
	void historyRecords_data() {
		QTest::addColumn<int>("row");
		QTest::addColumn<int>("column");
		QTest::addColumn<QVariant>("value");
		QTest::addColumn<int>("role");
		QTest::addColumn<QString>("field");
		QTest::addColumn<QString>("oldvalue");
		QTest::addColumn<QString>("newvalue");

		const int EDIT = Qt::EditRole, CHECK = Qt::CheckStateRole;
		const QVariant checked = QVariant(Qt::Checked);
		const QVariant unchecked = QVariant(Qt::Unchecked);
		bool ok = false;

		QTest::newRow("Name") << 0 << int(NAME)        << QVariant("Test item")        << EDIT  << "Name"               << ""         << "Test item";

		QScopedPointer<InventoryModel> model(new InventoryModel());
		QVERIFY(model);
		ok = model->removeRows(0, model->rowCount()); QVERIFY(ok); QCOMPARE(model->rowCount(), 0);
		QScopedPointer<ReferenceModel> itemTypes(new ReferenceModel(ReferenceModel::ITEM_TYPES));
			QVERIFY(itemTypes);
			itemTypes->removeRows(0, itemTypes->rowCount());
			itemTypes->insertRows(0, 3);
			itemTypes->setData(itemTypes->index(0, 0), "Type 1");
			itemTypes->setData(itemTypes->index(1, 0), "Type 2");
			itemTypes->setData(itemTypes->index(2, 0), "Type 3");
		QScopedPointer<ReferenceModel> places   (new ReferenceModel(ReferenceModel::PLACES));
			QVERIFY(places);
			places->removeRows(0, places->rowCount());
			places->insertRows(0, 3);
			places->setData(places->index(0, 0), "Place 1");
			places->setData(places->index(1, 0), "Place 2");
			places->setData(places->index(2, 0), "Place 3");
		QScopedPointer<ReferenceModel> persons   (new ReferenceModel(ReferenceModel::PERSONS));
			QVERIFY(persons);
			persons->removeRows(0, persons->rowCount());
			persons->insertRows(0, 3);
			persons->setData(persons->index(0, 0), "Person 1");
			persons->setData(persons->index(1, 0), "Person 2");
			persons->setData(persons->index(2, 0), "Person 3");
		ok = model->insertRow(0); QVERIFY(ok); QCOMPARE(model->rowCount(), 1);
		model->setData(model->index(0, ITEM_TYPE),   itemTypes->idAt(0));
		model->setData(model->index(0, PLACE),       places->idAt(0));
		model->setData(model->index(0, PERSON),      persons->idAt(0));

		QTest::newRow("type")    << 0 << int(ITEM_TYPE)   << QVariant(itemTypes->idAt(1)) << EDIT  << "Item type"          << "Type 1"   << "Type 2";
		QTest::newRow("place")   << 0 << int(PLACE)       << QVariant(places->idAt(1)   ) << EDIT  << "Place"              << "Place 1"  << "Place 2";
		QTest::newRow("person")  << 0 << int(PERSON)      << QVariant(persons->idAt(1)  ) << EDIT  << "Responsible person" << "Person 1" << "Person 2";
		QTest::newRow("inn 1")   << 0 << int(INN)         << QVariant(1                 ) << EDIT  << "INN"                << ""         << "1";
		QTest::newRow("inn 2")   << 0 << int(INN)         << QVariant(""                ) << EDIT  << "INN"                << "1"        << "";
		QTest::newRow("wr off")  << 0 << int(WRITTEN_OFF) << QVariant(checked           ) << CHECK << "Written off"        << "Present"  << "Written off";
		QTest::newRow("repair")  << 0 << int(REPAIR)      << QVariant(checked           ) << CHECK << "Under repair"       << "Working"  << "Under repair";
		QTest::newRow("checked") << 0 << int(CHECKED)     << QVariant(checked           ) << CHECK << "Checked"            << "Not checked" << "Checked";
		QTest::newRow("notes")   << 0 << int(NOTES)       << QVariant("Simple note"     ) << EDIT  << "Note"               << ""         << "Simple note";
	}
	void historyRecords() {
		QFETCH(int, row);
		QFETCH(int, column);
		QFETCH(QVariant, value);
		QFETCH(int, role);

		QScopedPointer<InventoryModel> inventory(new InventoryModel());
		bool ok = inventory->setData(inventory->index(row, column), value, role);
		QVERIFY(ok);
		QCOMPARE(inventory->data(inventory->index(row, column), role), value);

		QFETCH(QString, field);
		QFETCH(QString, oldvalue);
		QFETCH(QString, newvalue);

		QScopedPointer<HistoryModel> model(new HistoryModel(inventory->idAt(row)));
		QVERIFY(model);
		QVERIFY(model->rowCount() > 0);
		QCOMPARE(   model->data(model->index(model->rowCount() - 1, 1)), QVariant(field));
		/*
		qDebug() << 
			model->data(model->index(model->rowCount() - 1, 0)) <<
			model->data(model->index(model->rowCount() - 1, 1)) <<
			model->data(model->index(model->rowCount() - 1, 2)) <<
			model->data(model->index(model->rowCount() - 1, 3));
			*/
		QCOMPARE(model->data(model->index(model->rowCount() - 1, 2)), QVariant(oldvalue));
		QCOMPARE(model->data(model->index(model->rowCount() - 1, 3)), QVariant(newvalue));

		QCOMPARE(int(model->flags(model->index(model->rowCount() - 1, 0))), int(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
		QCOMPARE(int(model->flags(model->index(model->rowCount() - 1, 1))), int(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
		QCOMPARE(int(model->flags(model->index(model->rowCount() - 1, 2))), int(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
		QCOMPARE(int(model->flags(model->index(model->rowCount() - 1, 3))), int(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
		QCOMPARE(                         int(model->flags(model->index(model->rowCount() - 1, 4))), int(Qt::NoItemFlags));
	}
	void historyClear() {
		QScopedPointer<InventoryModel> inventory(new InventoryModel());
		QVERIFY(inventory);
		Id id = inventory->idAt(0);
		QVERIFY(id);
		bool ok = inventory->removeRow(0);
		QVERIFY(ok);

		QScopedPointer<HistoryModel> model(new HistoryModel(id));
		QVERIFY(model);
		QCOMPARE(model->rowCount(), 0);
	}
	
	void printableInventoryHeaderData_data() {
		QTest::addColumn<QVariant>("value");
		QTest::addColumn<int>("section");
		QTest::addColumn<int>("orientation");

		QTest::newRow("type")   << QVariant("Item type")   << 0 << int(Qt::Horizontal);
		QTest::newRow("name")   << QVariant("Name")        << 1 << int(Qt::Horizontal);
		QTest::newRow("count")  << QVariant("Count")       << 2 << int(Qt::Horizontal);
		QTest::newRow("place")  << QVariant("Place")       << 3 << int(Qt::Horizontal);
		QTest::newRow("inn")    << QVariant("INN")         << 3 << int(Qt::Horizontal);
		QTest::newRow("wr off") << QVariant("Written off") << 3 << int(Qt::Horizontal);
	}
	void printableInventoryHeaderData() {
		QFETCH(QVariant, value);
		QFETCH(int, section);
		QFETCH(int, orientation);

		QScopedPointer<PrintableInventoryModel> model(new PrintableInventoryModel());
		QVERIFY(model);
		QCOMPARE(model->columnCount(), 9);
		QCOMPARE(model->rowCount(), 0);
		QCOMPARE(model->headerData(section, Qt::Orientation(orientation), Qt::DisplayRole), value);
	}
	void printableInventory_data() {
		QTest::addColumn<QString>("name");
		QTest::addColumn<int>("rowcount");
		QTest::addColumn<int>("totalcount");

		QTest::newRow("invalid") << "Invalid" << 0 << 0;

		bool ok = false;

		QScopedPointer<InventoryModel> model(new InventoryModel());
		QVERIFY(model);
		ok = model->removeRows(0, model->rowCount()); QVERIFY(ok); QCOMPARE(model->rowCount(), 0);
		QScopedPointer<ReferenceModel> itemTypes(new ReferenceModel(ReferenceModel::ITEM_TYPES));
			QVERIFY(itemTypes);
			itemTypes->removeRows(0, itemTypes->rowCount());
			itemTypes->insertRows(0, 3);
			itemTypes->setData(itemTypes->index(0, 0), "Type 1");
			itemTypes->setData(itemTypes->index(1, 0), "Type 2");
			itemTypes->setData(itemTypes->index(2, 0), "Type 3");
		QScopedPointer<ReferenceModel> places   (new ReferenceModel(ReferenceModel::PLACES));
			QVERIFY(places);
			places->removeRows(0, places->rowCount());
			places->insertRows(0, 3);
			places->setData(places->index(0, 0), "Place 1");
			places->setData(places->index(1, 0), "Place 2");
			places->setData(places->index(2, 0), "Place 3");
		QScopedPointer<ReferenceModel> persons   (new ReferenceModel(ReferenceModel::PERSONS));
			QVERIFY(persons);
			persons->removeRows(0, persons->rowCount());
			persons->insertRows(0, 3);
			persons->setData(persons->index(0, 0), "Person 1");
			persons->setData(persons->index(1, 0), "Person 2");
			persons->setData(persons->index(2, 0), "Person 3");
		ok = model->insertRows(0, 8); QVERIFY(ok); QCOMPARE(model->rowCount(), 8);

		model->setData(model->index(0, ITEM_TYPE),   itemTypes->idAt(0));
		model->setData(model->index(0, PLACE),       places->idAt(0));
		model->setData(model->index(0, WRITTEN_OFF), Qt::Checked);
		model->setData(model->index(0, NAME),        "First");

		model->setData(model->index(1, ITEM_TYPE),   itemTypes->idAt(0));
		model->setData(model->index(1, PLACE),       places->idAt(0));
		model->setData(model->index(1, WRITTEN_OFF), Qt::Unchecked);
		model->setData(model->index(1, NAME),        "First");

		model->setData(model->index(2, ITEM_TYPE),   itemTypes->idAt(1));
		model->setData(model->index(2, PLACE),       places->idAt(0));
		model->setData(model->index(2, WRITTEN_OFF), Qt::Checked);
		model->setData(model->index(2, NAME),        "Second");

		model->setData(model->index(3, ITEM_TYPE),   itemTypes->idAt(1));
		model->setData(model->index(3, PLACE),       places->idAt(0));
		model->setData(model->index(3, WRITTEN_OFF), Qt::Unchecked);
		model->setData(model->index(3, NAME),        "Second");

		model->setData(model->index(4, ITEM_TYPE),   itemTypes->idAt(1));
		model->setData(model->index(4, PLACE),       places->idAt(0));
		model->setData(model->index(4, WRITTEN_OFF), Qt::Checked);
		model->setData(model->index(4, NAME),        "Second");

		model->setData(model->index(5, ITEM_TYPE),   itemTypes->idAt(1));
		model->setData(model->index(5, PLACE),       places->idAt(1));
		model->setData(model->index(5, WRITTEN_OFF), Qt::Checked);
		model->setData(model->index(5, NAME),        "Third");

		model->setData(model->index(6, ITEM_TYPE),   itemTypes->idAt(1));
		model->setData(model->index(6, PLACE),       places->idAt(1));
		model->setData(model->index(6, WRITTEN_OFF), Qt::Checked);
		model->setData(model->index(6, NAME),        "Third");

		model->setData(model->index(7, ITEM_TYPE),   itemTypes->idAt(1));
		model->setData(model->index(7, PLACE),       places->idAt(1));
		model->setData(model->index(7, WRITTEN_OFF), Qt::Checked);
		model->setData(model->index(7, NAME),        "Third");

		QTest::newRow("first")  << QVariant("First") << 2 << 2;
		QTest::newRow("second") << QVariant("Second") << 2 << 3;
		QTest::newRow("third")  << QVariant("Third") << 1 << 3;
	}
	void printableInventory() {
		QFETCH(QString, name);
		QFETCH(int, rowcount);
		QFETCH(int, totalcount);

		QScopedPointer<PrintableInventoryModel> model(new PrintableInventoryModel());
		QVERIFY(model);
		int sum = 0, matches = 0;
		for(int row = 0; row < model->rowCount(); ++row) {
			if(model->data(model->index(row, 1)) == QVariant(name)) {
				++matches;
				sum += model->data(model->index(row, 2)).toInt();
			}
		}
		QCOMPARE(matches, rowcount);
		QCOMPARE(sum, totalcount);
	}
	
	void printableInventoryFilter_data() {
		QTest::addColumn<int>("count");
		QTest::addColumn<bool>("itemtypefilter");
		QTest::addColumn<QVariant>("itemtype");
		QTest::addColumn<bool>("placefilter");
		QTest::addColumn<QVariant>("place");
		QTest::addColumn<bool>("writtenofffilter");
		QTest::addColumn<QVariant>("writtenoff");

		QTest::newRow("invalid") << 8 << false << QVariant(-1) << false << QVariant(-1) << false << QVariant(0);

		bool ok = false;

		QScopedPointer<InventoryModel> model(new InventoryModel());
		QVERIFY(model);
		ok = model->removeRows(0, model->rowCount()); QVERIFY(ok); QCOMPARE(model->rowCount(), 0);
		QScopedPointer<ReferenceModel> itemTypes(new ReferenceModel(ReferenceModel::ITEM_TYPES));
			QVERIFY(itemTypes);
			itemTypes->removeRows(0, itemTypes->rowCount());
			itemTypes->insertRows(0, 3);
			itemTypes->setData(itemTypes->index(0, 0), "Type 1");
			itemTypes->setData(itemTypes->index(1, 0), "Type 2");
			itemTypes->setData(itemTypes->index(2, 0), "Type 3");
		QScopedPointer<ReferenceModel> places   (new ReferenceModel(ReferenceModel::PLACES));
			QVERIFY(places);
			places->removeRows(0, places->rowCount());
			places->insertRows(0, 3);
			places->setData(places->index(0, 0), "Place 1");
			places->setData(places->index(1, 0), "Place 2");
			places->setData(places->index(2, 0), "Place 3");
		ok = model->insertRows(0, 8); QVERIFY(ok); QCOMPARE(model->rowCount(), 8);

		model->setData(model->index(0, ITEM_TYPE),   itemTypes->idAt(0));
		model->setData(model->index(0, PLACE),       places->idAt(0));
		model->setData(model->index(0, WRITTEN_OFF), Qt::Checked);

		model->setData(model->index(1, ITEM_TYPE),   itemTypes->idAt(0));
		model->setData(model->index(1, PLACE),       places->idAt(0));
		model->setData(model->index(1, WRITTEN_OFF), Qt::Unchecked);

		model->setData(model->index(2, ITEM_TYPE),   itemTypes->idAt(1));
		model->setData(model->index(2, PLACE),       places->idAt(0));
		model->setData(model->index(2, WRITTEN_OFF), Qt::Checked);

		model->setData(model->index(3, ITEM_TYPE),   itemTypes->idAt(1));
		model->setData(model->index(3, PLACE),       places->idAt(0));
		model->setData(model->index(3, WRITTEN_OFF), Qt::Unchecked);

		model->setData(model->index(4, ITEM_TYPE),   itemTypes->idAt(0));
		model->setData(model->index(4, PLACE),       places->idAt(1));
		model->setData(model->index(4, WRITTEN_OFF), Qt::Checked);

		model->setData(model->index(5, ITEM_TYPE),   itemTypes->idAt(0));
		model->setData(model->index(5, PLACE),       places->idAt(1));
		model->setData(model->index(5, WRITTEN_OFF), Qt::Unchecked);

		model->setData(model->index(6, ITEM_TYPE),   itemTypes->idAt(1));
		model->setData(model->index(6, PLACE),       places->idAt(1));
		model->setData(model->index(6, WRITTEN_OFF), Qt::Checked);

		model->setData(model->index(7, ITEM_TYPE),   itemTypes->idAt(1));
		model->setData(model->index(7, PLACE),       places->idAt(1));
		model->setData(model->index(7, WRITTEN_OFF), Qt::Unchecked);

		QTest::newRow("000")   << 8 << false << itemTypes->idAt(0) << false << places->idAt(0) << false << int(Qt::Unchecked);
		QTest::newRow("100")   << 4 << true  << itemTypes->idAt(1) << false << places->idAt(0) << false << int(Qt::Unchecked);
		QTest::newRow("010")   << 4 << false << itemTypes->idAt(1) << true  << places->idAt(0) << false << int(Qt::Unchecked);
		QTest::newRow("110")   << 2 << true  << itemTypes->idAt(0) << true  << places->idAt(1) << false << int(Qt::Checked);
		QTest::newRow("001")   << 4 << false << itemTypes->idAt(0) << false << places->idAt(1) << true  << int(Qt::Checked);
		QTest::newRow("101")   << 2 << true  << itemTypes->idAt(1) << false << places->idAt(1) << true  << int(Qt::Checked);
		QTest::newRow("011")   << 2 << false << itemTypes->idAt(1) << true  << places->idAt(1) << true  << int(Qt::Unchecked);
		QTest::newRow("111 1") << 1 << true  << itemTypes->idAt(1) << true  << places->idAt(1) << true  << int(Qt::Checked);
		QTest::newRow("111 2") << 1 << true  << itemTypes->idAt(0) << true  << places->idAt(0) << true  << int(Qt::Unchecked);
		QTest::newRow("111 3") << 1 << true  << itemTypes->idAt(1) << true  << places->idAt(0) << true  << int(Qt::Checked);
	}
	void printableInventoryFilter() {
		QFETCH(int, count);
		QFETCH(bool, itemtypefilter);
		QFETCH(QVariant, itemtype);
		QFETCH(bool, placefilter);
		QFETCH(QVariant, place);
		QFETCH(bool, writtenofffilter);
		QFETCH(QVariant, writtenoff);

		QScopedPointer<PrintableInventoryModel> model(new PrintableInventoryModel());
		QVERIFY(model);
		model->switchItemTypeFilter(itemtypefilter);
		model->setItemTypeFilter(itemtype.toInt());
		model->switchPlaceFilter(placefilter);
		model->setPlaceFilter(place.toInt());
		model->switchWrittenOffFilter(writtenofffilter);
		model->setWrittenOffFilter(writtenoff.toBool());

		QCOMPARE(model->rowCount(), count);

		if(itemtypefilter) {
			for(int row = 0; row < model->rowCount(); ++row) {
				QCOMPARE(model->data(model->index(row, 0), Qt::EditRole), itemtype);
			}
		}

		if(placefilter) {
			for(int row = 0; row < model->rowCount(); ++row) {
				QCOMPARE(model->data(model->index(row, 3), Qt::EditRole), place);
			}
		}

		if(writtenofffilter) {
			for(int row = 0; row < model->rowCount(); ++row) {
				QCOMPARE(model->data(model->index(row, 4), Qt::EditRole), writtenoff);
			}
		}
	}
};

QTEST_MAIN(InventoryTest)
#include "test.moc"

