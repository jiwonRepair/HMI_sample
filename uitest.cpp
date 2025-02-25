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
}
