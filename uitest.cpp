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

void QmlTest::testPageNavigationWithButton()
{
    setUp(); // ✅ QML 초기화

    // ✅ PageManager 가져오기
    PageManager *pm = qobject_cast<PageManager *>(pageManager);
    QVERIFY2(pm, "❌ ERROR: pageManager could not be cast to PageManager*");

    // ✅ 버튼 찾기 (QML 내 id가 "page2Button"인 버튼을 찾음)
    QQuickItem *page2Button = rootObject->findChild<QQuickItem*>("page2Button");
    QVERIFY2(page2Button, "❌ ERROR: page2Button not found!");

    // ✅ 버튼이 속한 윈도우 가져오기
    QQuickWindow *window = page2Button->window();
    QVERIFY2(window, "❌ ERROR: QQuickWindow not found for page2Button!");

    // ✅ `pageChanged` 시그널 감지 (QSignalSpy 사용)
    QSignalSpy pageChangedSpy(pm, &PageManager::pageChanged);
    QVERIFY2(pageChangedSpy.isValid(), "❌ ERROR: QSignalSpy could not attach to pageChanged signal!");

    // ✅ 버튼 클릭 이벤트 시뮬레이션 (QQuickItem -> QWindow 사용)
    QPoint center = page2Button->mapToScene(QPointF(page2Button->width() / 2, page2Button->height() / 2)).toPoint();
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, center);

    // ✅ `pageChanged` 신호가 발생했는지 확인
    QTRY_VERIFY(pageChangedSpy.count() > 0);

    // ✅ "hwstatus" 페이지로 돌아가기 위해 버튼 찾기
    QQuickItem *hwStatusButton = rootObject->findChild<QQuickItem*>("hwStatusButton");
    QVERIFY2(hwStatusButton, "❌ ERROR: hwStatusButton not found!");

    // ✅ 버튼이 속한 윈도우 가져오기
    window = hwStatusButton->window();
    QVERIFY2(window, "❌ ERROR: QQuickWindow not found for hwStatusButton!");

    // ✅ 버튼 클릭 이벤트 시뮬레이션
    pageChangedSpy.clear();
    center = hwStatusButton->mapToScene(QPointF(hwStatusButton->width() / 2, hwStatusButton->height() / 2)).toPoint();
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, center);

    // ✅ `pageChanged` 신호가 발생했는지 확인
    QTRY_VERIFY(pageChangedSpy.count() > 0);
}

void QmlTest::testFullPageNavigationWithButtons()
{
    setUp(); // ✅ QML 초기화

    PageManager *pm = qobject_cast<PageManager *>(pageManager);
    QVERIFY2(pm, "❌ ERROR: pageManager could not be cast to PageManager*");

    // ✅ `pageChanged` 시그널 감지 (QSignalSpy 사용)
    QSignalSpy pageChangedSpy(pm, &PageManager::pageChanged);
    QVERIFY2(pageChangedSpy.isValid(), "❌ ERROR: QSignalSpy could not attach to pageChanged signal!");

    // ✅ Page1 → Page2 이동
    clickButtonAndVerify("page2Button", pageChangedSpy);

    // ✅ Page2 → Page3 이동
    clickButtonAndVerify("page3Button", pageChangedSpy);

    // ✅ Page3 → Page4 이동
    clickButtonAndVerify("page4Button", pageChangedSpy);

    // ✅ Page4 → Page5 이동
    clickButtonAndVerify("page5Button", pageChangedSpy);

    // ✅ Page5 → hwStatusButton 이동
    clickButtonAndVerify("hwStatusButton", pageChangedSpy);
}

/**
 * @brief 버튼 클릭을 수행하고 `pageChanged` 시그널이 발생했는지 확인하는 헬퍼 함수
 */
void QmlTest::clickButtonAndVerify(const QString &buttonId, QSignalSpy &pageChangedSpy)
{
    // ✅ 버튼 찾기
    QQuickItem *button = rootObject->findChild<QQuickItem*>(buttonId);
    QVERIFY2(button, QString("❌ ERROR: %1 not found!").arg(buttonId).toUtf8().constData());

    // ✅ 버튼이 속한 윈도우 가져오기
    QQuickWindow *window = button->window();
    QVERIFY2(window, QString("❌ ERROR: QQuickWindow not found for %1!").arg(buttonId).toUtf8().constData());

    // ✅ 클릭 이벤트 시뮬레이션
    QPoint center = button->mapToScene(QPointF(button->width() / 2, button->height() / 2)).toPoint();
    pageChangedSpy.clear();
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, center);

    // ✅ `pageChanged` 신호가 발생했는지 확인
    QTRY_VERIFY(pageChangedSpy.count() > 0);
    //QTRY_VERIFY_WITH_TIMEOUT(pageChangedSpy.count() > 0, 1600000);  // ✅ 10분(600,000ms)까지 대기

}

void QmlTest::testStressFullPageNavigation1()
{
    runStressTest(0, 10000);
}
void QmlTest::testStressFullPageNavigation2()
{
    runStressTest(0, 10000);
}
void QmlTest::testStressFullPageNavigation3()
{
    runStressTest(0, 10000);
}
void QmlTest::testStressFullPageNavigation4()
{
    runStressTest(0, 10000);
}
void QmlTest::testStressFullPageNavigation5()
{
    runStressTest(0, 10000);
}
void QmlTest::testStressFullPageNavigation6()
{
    runStressTest(0, 10000);
}

void QmlTest::runStressTest(int start, int end)
{
    setUp(); // ✅ QML 초기화

    PageManager *pm = qobject_cast<PageManager *>(pageManager);
    QVERIFY2(pm, "❌ ERROR: pageManager could not be cast to PageManager*");

    QSignalSpy pageChangedSpy(pm, &PageManager::pageChanged);
    QVERIFY2(pageChangedSpy.isValid(), "❌ ERROR: QSignalSpy could not attach to pageChanged signal!");

    qDebug() << "[INFO] Running stress test: 1000 iterations";

    for (int i = start; i < end; ++i) {
        qDebug() << "[INFO] Iteration:" << i + 1;

        // ✅ Page1 → Page2 이동
        clickButtonAndVerify("page2Button", pageChangedSpy);

        // ✅ Page2 → Page3 이동
        clickButtonAndVerify("page3Button", pageChangedSpy);

        // ✅ Page3 → Page4 이동
        clickButtonAndVerify("page4Button", pageChangedSpy);

        // ✅ Page4 → Page5 이동
        clickButtonAndVerify("page5Button", pageChangedSpy);

        // ✅ Page5 → hwStatusButton 이동
        clickButtonAndVerify("hwStatusButton", pageChangedSpy);

        // ✅ CPU 과부하 방지 (짧은 대기)
        QTest::qWait(10); // 10ms 대기 (필요시 조정)
    }

    qDebug() << "[INFO] Stress test completed successfully!";
}
