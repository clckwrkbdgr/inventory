#include <QtTest/QtTest>
#include "models.h"

using namespace Inventory;

class InventoryTest: public QObject {
	Q_OBJECT
private slots:
	void referenceTypes_data() {
		QTest::addColumn<int>("original");
		QTest::addColumn<int>("result");

		QTest::newRow("item types") << int(ReferenceModel::ITEM_TYPES) << int(ReferenceModel::ITEM_TYPES);
		QTest::newRow("places")     << int(ReferenceModel::PLACES)     << int(ReferenceModel::PLACES);
		QTest::newRow("persons")    << int(ReferenceModel::PERSONS)    << int(ReferenceModel::PERSONS);
		QTest::newRow("invalid")    << int(ReferenceModel::INVALID)    << int(ReferenceModel::INVALID);
		QTest::newRow("too big")    << int(ReferenceModel::COUNT)      << int(ReferenceModel::INVALID);
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
		QCOMPARE(0, model->rowCount());
		QCOMPARE(1, model->columnCount());
		ok = model->insertColumn(0);
		QVERIFY(!ok);
		QCOMPARE(1, model->columnCount());
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
		QCOMPARE(value, model->headerData(section, Qt::Orientation(orientation), Qt::DisplayRole));
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
		QCOMPARE(rowcount, model->rowCount());
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

		bool ok = model->removeRows(0, model->rowCount() - 1);
		QVERIFY(ok);
		QCOMPARE(0, model->rowCount());

		ok = model->insertRows(0, 3);
		QVERIFY(ok);
		QCOMPARE(3, model->rowCount());

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
		QCOMPARE(Qt::ItemFlags(flags), model->flags(index));
		bool ok = model->setData(index, value);
		QCOMPARE(ok, success);
		if(ok) {
			QCOMPARE(value, model->data(index));
		}
	}
	void referenceMultiline() {
		QScopedPointer<ReferenceModel> model(new ReferenceModel(ReferenceModel::PLACES));
		QVERIFY(model);

		bool ok = model->removeRows(0, model->rowCount() - 1);
		QVERIFY(ok);
		QCOMPARE(0, model->rowCount());

		ok = model->insertRow(0);
		QVERIFY(ok);
		QCOMPARE(1, model->rowCount());

		ok = model->addMultiline(QStringList() << "First line" << "Second line");
		QVERIFY(ok);
		QCOMPARE(3, model->rowCount());

		QCOMPARE(QVariant(""),            model->data(model->index(0, 0)));
		QCOMPARE(QVariant("First line"),  model->data(model->index(1, 0)));
		QCOMPARE(QVariant("Second line"), model->data(model->index(2, 0)));
	}
};

QTEST_MAIN(InventoryTest)
#include "test.moc"

