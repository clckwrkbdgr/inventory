#include <QtTest/QtTest>

class InventoryTest: public QObject
{
	Q_OBJECT
private slots:
	void initTestCase() {} 
	void myFirstTest() {
		QVERIFY(1 == 1);
	}
	void cleanupTestCase() {}
};

QTEST_MAIN(InventoryTest)
#include "test.moc"

