// #include <QTest>
// #include "uitest.h"  // ✅ QmlTest 클래스 포함
// //#include "testosfilemanager.h"

// QTEST_MAIN(QmlTest)  // ✅ 테스트 실행 매크로

// // ✅ QtTest 실행 매크로
// //QTEST_MAIN(TestOsFileManager)

#include <QGuiApplication>
#include <QTest>
#include "uitest.h"
#include "testosfilemanager.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv); // ✅ QML을 사용할 경우 QGuiApplication 사용

    int status = 0;

    QmlTest qmlTest;
    TestOsFileManager fileTest;

    status |= QTest::qExec(&qmlTest, argc, argv);
    status |= QTest::qExec(&fileTest, argc, argv);

    return status;
}


