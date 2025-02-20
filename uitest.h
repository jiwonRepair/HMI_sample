#ifndef UITEST_H
#define UITEST_H

#include <QTest>

class UITest : public QObject {
    Q_OBJECT

private slots:
    void testAddition();
    void testString();
};

void UITest::testAddition() {
    int a = 2;
    int b = 3;
    QCOMPARE(a + b, 5);
}

void UITest::testString() {
    QString str = "hello";
    QVERIFY(str.startsWith("h"));
}

//QTEST_MAIN(UITest) // 테스트 실행 엔트리 포인트
//#include "test.moc" // MOC 생성 필요
#endif // UITEST_H
