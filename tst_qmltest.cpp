#include <QGuiApplication>
#include <QTest>
#include <vector>
#include <functional> // std::reference_wrapper 사용
#include "uitest.h" // ✅ QmlTest 클래스 포함
#include "testosfilemanager.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv); // ✅ QML을 사용할 경우 QGuiApplication 사용

    // ✅ 스택 기반 객체 생성 (동적 할당 X)
    QmlTest qmlTest;
    TestOsFileManager fileTest;

    // ✅ reference_wrapper를 사용하여 안전한 벡터 저장
    std::vector<std::reference_wrapper<QObject>> tests = { qmlTest, fileTest };

    // ✅ 모든 테스트 실행 (하나라도 실패하면 1 반환)
    int status = 0;
    for (QObject& test : tests) {
        status |= QTest::qExec(&test, argc, argv);
    }

    //QTEST_MAIN(QmlTest)  // ✅ 테스트 실행 매크로
    return status;
}

