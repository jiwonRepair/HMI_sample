#include "uitest.h"
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTest>
#include <QQuickItem>
#include <QQuickWindow>
#include "pagemanager.h"
#include "cpumonitor.h"
#include "batterymonitor.h"
#include "memorymonitor.h"
#include "diskmonitor.h"
#include "hardwarestatusmodel.h"
#include "wifimonitor.h"

#include <QSignalSpy>  // ✅ QSignalSpy 추가 (신호 감지 용도)


// ✅ QML 및 객체 초기화 (테스트 시작 전에 실행됨)
void QmlTest::setUp()
{
    if (!engine.rootObjects().isEmpty()) return; // ✅ 이미 로드되었으면 중복 실행 방지

    // ✅ PageManager를 멤버 변수로 유지 (지역 변수 X)
    static PageManager pageManagerInstance;
    pageManager = &pageManagerInstance; // ✅ 멤버 변수에 저장

    // ✅ C++ 객체를 QML에 등록
    engine.rootContext()->setContextProperty("pageManager", pageManager);
    engine.rootContext()->setContextProperty("cpuMonitor", new CpuMonitor(this));
    engine.rootContext()->setContextProperty("batteryMonitor", new BatteryMonitor(this));
    engine.rootContext()->setContextProperty("memoryMonitor", new MemoryMonitor(this));
    engine.rootContext()->setContextProperty("diskMonitor", new DiskMonitor(this));
    engine.rootContext()->setContextProperty("hardwareStatusModel", new HardwareStatusModel(this));
    engine.rootContext()->setContextProperty("wifiMonitor", new WifiMonitor(this));

    // ✅ QML 파일 로드
    QUrl qmlUrl = QUrl::fromLocalFile("D:/qtprojects/HMI_sample/Main.qml");
    engine.load(qmlUrl);
    QTest::qWait(1000);

    // ✅ QML이 정상적으로 로드되었는지 확인
    const QList<QObject *> &rootObjects = engine.rootObjects();
    if (rootObjects.isEmpty()) {
        QFAIL("❌ ERROR: QML failed to load! Check file path.");
    }

    qDebug() << "✅ SUCCESS: QML loaded successfully!";

    // ✅ rootObject 설정
    rootObject = rootObjects.first();
    QVERIFY2(rootObject, "❌ ERROR: Root QML object not found!");

    // ✅ `contextProperty()`를 통해 `pageManager` 가져오기 (이제 에러 없음)
    pageManager = engine.rootContext()->contextProperty("pageManager").value<PageManager *>();
    QVERIFY2(pageManager, "❌ ERROR: pageManager not found in contextProperty!");
}


void QmlTest::testPageNavigation()
{
    setUp(); // ✅ QML 및 객체 초기화

    // ✅ `pageManager`를 PageManager*로 변환
    PageManager *pm = qobject_cast<PageManager *>(pageManager);
    QVERIFY2(pm, "❌ ERROR: pageManager could not be cast to PageManager*");

    // ✅ `pageChanged` 신호 감지 (QSignalSpy 사용)
    QSignalSpy pageChangedSpy(pm, &PageManager::pageChanged);
    QVERIFY2(pageChangedSpy.isValid(), "❌ ERROR: QSignalSpy could not attach to pageChanged signal!");

    // ✅ "Page 2"로 이동 (버튼 클릭 없이 `showPage()` 직접 호출)
    bool invoked = QMetaObject::invokeMethod(pageManager, "showPage", Q_ARG(QString, "page2"));
    QVERIFY2(invoked, "❌ ERROR: showPage(\"page2\") method invocation failed!");

    // ✅ `pageChanged` 신호가 발생했는지 확인
    QTRY_VERIFY(pageChangedSpy.count() > 0);

    // ✅ 다시 "H/W Status"로 이동
    pageChangedSpy.clear();
    invoked = QMetaObject::invokeMethod(pageManager, "showPage", Q_ARG(QString, "hwstatus"));
    QVERIFY2(invoked, "❌ ERROR: showPage(\"hwstatus\") method invocation failed!");

    // ✅ `pageChanged` 신호가 발생했는지 확인
    QTRY_VERIFY(pageChangedSpy.count() > 0);
}
