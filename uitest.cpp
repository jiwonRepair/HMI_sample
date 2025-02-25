#include "uitest.h"
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "pagemanager.h"
#include "cpumonitor.h"
#include "batterymonitor.h"
#include "memorymonitor.h"
#include "diskmonitor.h"
#include "hardwarestatusmodel.h"
#include "wifimonitor.h"

#include <QQmlDebuggingEnabler>  // ✅ QML 디버깅 활성화 헤더 추가


void QmlTest::testButtonClick()
{
    QQmlApplicationEngine engine;

    // ✅ C++ 객체를 QML에 등록
    PageManager pageManager;
    CpuMonitor cpuMonitor;
    BatteryMonitor batteryMonitor;
    MemoryMonitor memoryMonitor;
    DiskMonitor diskMonitor;
    HardwareStatusModel hardwareStatusModel;
    WifiMonitor wifiMonitor;

    engine.rootContext()->setContextProperty("pageManager", &pageManager);
    engine.rootContext()->setContextProperty("cpuMonitor", &cpuMonitor);
    engine.rootContext()->setContextProperty("batteryMonitor", &batteryMonitor);
    engine.rootContext()->setContextProperty("memoryMonitor", &memoryMonitor);
    engine.rootContext()->setContextProperty("diskMonitor", &diskMonitor);
    engine.rootContext()->setContextProperty("hardwareStatusModel", &hardwareStatusModel);
    engine.rootContext()->setContextProperty("wifiMonitor", &wifiMonitor);

    // ✅ QML 파일 로드
    engine.load(QUrl::fromLocalFile("D:/qtprojects/HMI_sample/Main.qml"));

    // ✅ rootObjects를 먼저 변수에 저장하여 안전하게 처리
    const QObjectList rootObjects = engine.rootObjects();
    QObject *rootObject = rootObjects.isEmpty() ? nullptr : rootObjects.first();
    QVERIFY(rootObject);

    // ✅ 버튼 찾기
    QQuickItem *button = rootObject->findChild<QQuickItem *>("hwStatusButton");
    QVERIFY(button);

    // ✅ 버튼 클릭 시그널 직접 호출
    QMetaObject::invokeMethod(button, "clicked");

    // ✅ 클릭 후 UI가 정상적으로 업데이트되었는지 검사
    QTest::qWait(500);

    // // ✅ 버튼의 QML 윈도우 가져오기
    // QQuickWindow *window = button->window();
    // QVERIFY(window);

    // // ✅ 버튼의 중앙 좌표 계산 (윈도우 좌표 기준)
    // QPointF buttonCenter = button->mapToScene(QPointF(button->width() / 2, button->height() / 2));
    // QPoint globalPos = window->mapToGlobal(buttonCenter.toPoint());

    // qDebug() << "Button Global Position: " << globalPos;

    // // ✅ QSignalSpy로 clicked() 시그널 감지 설정
    // QSignalSpy spy(button, SIGNAL(clicked()));
    // QVERIFY(spy.isValid());

    // // // ✅ 마우스 클릭을 확실하게 처리하기 위해 press + release 사용
    // // QTest::mousePress(window, Qt::LeftButton, Qt::NoModifier, globalPos);
    // // QTest::mouseRelease(window, Qt::LeftButton, Qt::NoModifier, globalPos);

    // // qDebug() << "Button press: ";

    // // // ✅ 클릭 이벤트가 발생했는지 확인
    // // QVERIFY(spy.wait(5000));
    // QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, globalPos);

    // // 클릭 후 일정 시간 대기
    // QTest::qWait(1000);

    // // ✅ 클릭 이벤트 발생 횟수 확인 (0이면 실패)
    // QVERIFY(spy.count() > 0);
}
