#include <QtTest>
#include <QBoxLayout>
#include <QWidget>
#include "../src/Battery.h"

class TestBattery : public QObject {
    Q_OBJECT
private slots:
    void testWidgetDisplay();
    void testChargeValue();

private:
    QWidget container;
    QHBoxLayout layout;
};


void TestBattery::testWidgetDisplay() {
    Battery bat1(30, 16);
    Battery bat2(100, 20);
    Battery bat3(30, 240);
    //bat1.setRounding(8.0);

    layout.addStretch();
    layout.addWidget(&bat1);
    layout.addWidget(&bat2);
    layout.addWidget(&bat3);
    layout.setSpacing(0);
    layout.addStretch();

    container.setLayout(&layout);
    container.setAttribute(Qt::WA_TranslucentBackground);
    container.show();

    QVERIFY(bat1.isVisible());
    QTest::qWait(300000); // ms
}

void TestBattery::testChargeValue() {

}

QTEST_MAIN(TestBattery)
#include "TestBattery.moc"
