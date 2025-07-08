// testosfilemanager.cpp 전체 반영본
#include "testosfilemanager.h"
#include <QQmlContext>
#include <QTest>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSignalSpy>
#include <QFile>
#include <QDebug>

#include "cpumonitor.h"
#include "pagemanager.h"
#include "BatteryMonitor.h"
#include "memorymonitor.h"
#include "diskmonitor.h"
#include "hardwarestatusmodel.h"
#include "wifimonitor.h"
#include "wifioptimizer.h"
#include "LoggingDecorator.h"

#define LOCAL_TEST_FILE_PATH = "D:/test_local.txt";
#define USB_TEST_FILE_PATH = "E:/test_usb.txt";

// ✅ 클릭 후 시그널 대기 도우미 함수
void TestOsFileManager::clickButtonAndWaitForSignal(const QString &buttonId, QSignalSpy &spy, int timeout)
{
    auto *button = rootObject->findChild<QQuickItem*>(buttonId);
    QVERIFY2(button, qUtf8Printable(QString("❌ Button not found: %1").arg(buttonId)));

    auto *window = button->window();
    QVERIFY2(window, qUtf8Printable(QString("❌ QQuickWindow not found for: %1").arg(buttonId)));

    QPoint center = button->mapToScene(QPointF(button->width() / 2, button->height() / 2)).toPoint();
    spy.clear();
    QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, center);
    QTest::qWait(50); // 버튼 클릭 후 UI 반응 기다림
    QTRY_VERIFY_WITH_TIMEOUT(spy.count() > 0, timeout);

    QTRY_VERIFY_WITH_TIMEOUT(button->isVisible() && button->isEnabled(), 3000);

}

// ✅ 팝업 닫기 도우미 함수
void TestOsFileManager::clickPopupClose()
{
    QQuickItem *popupButton = nullptr;
    QTRY_VERIFY_WITH_TIMEOUT((popupButton = rootObject->findChild<QQuickItem*>("popupCloseButton")), 5000);

    auto *popupWindow = popupButton->window();
    QVERIFY2(popupWindow, "❌ QQuickWindow not found for popupCloseButton");

    QPoint center = popupButton->mapToScene(QPointF(popupButton->width() / 2, popupButton->height() / 2)).toPoint();
    QTest::mouseClick(popupWindow, Qt::LeftButton, Qt::NoModifier, center);

    // ✅ 팝업이 완전히 사라질 때까지 대기
    QTRY_VERIFY_WITH_TIMEOUT(!popupButton->isVisible(), 3000);
}

// ✅ 1. 테스트 시작 시 초기화
void TestOsFileManager::initTestCase() {
// 테스트 파일 경로 설정 (USB 드라이브가 /media/usb 또는 D:/usb로 마운트된다고 가정)

    OsFileManager osFileManager;
    QVERIFY(osFileManager.isUsbConnected()); //USB E drive가 존재하는지 확인

    // USB에 테스트용 파일 생성
    QFile usbTestFile1(usbTestFilePath);
    if (usbTestFile1.open(QIODevice::WriteOnly | QIODevice::Text)) {
        usbTestFile1.write("This is a test file on USB.\n");
        usbTestFile1.close();
    }

    QVERIFY(QFile::exists(usbTestFilePath)); // USB 파일이 존재하는지 확인

    // USB에 테스트용 파일 생성
    QFile usbTestFile2(localTestFilePath);
    if (usbTestFile2.open(QIODevice::WriteOnly | QIODevice::Text)) {
        usbTestFile2.write("This is a test file on USB.\n");
        usbTestFile2.close();
    }

    QVERIFY(QFile::exists(usbTestFilePath)); // USB 파일이 존재하는지 확인
    QVERIFY(QFile::exists(localTestFilePath)); // USB 파일이 존재하는지 확인
}

// ✅ QML 세팅 및 단일 인스턴스 공유
void TestOsFileManager::setUp()
{
    if (!engine.rootObjects().isEmpty()) return;

    // 글로벌에서 하나만 생성
    static OsFileManager sharedInstance;                  // ✅ 딱 하나의 인스턴스
    osFileManager = &sharedInstance;

    PageManager pageManager;
    CpuMonitor cpuMonitor;
    BatteryMonitor batteryMonitor;
    MemoryMonitor memoryMonitor;  // ✅ 객체 생성
    DiskMonitor diskMonitor;
    HardwareStatusModel hardwareStatusModel;
    WifiMonitor wifiMonitor;
    WifiOptimizer wifiOptimizer;

    LoggingDecorator logger("HMI Application TEST");
    Q_UNUSED(logger);  // Qt Creator에서 사용된 것으로 간주되도록

    // ✅ 예외 처리를 감싸서 QML 컨텍스트에 객체 등록
    logger.execute([&]() {
        engine.rootContext()->setContextProperty("wifiOptimizer", &wifiOptimizer);
        engine.rootContext()->setContextProperty("osFileManager", osFileManager);
        engine.rootContext()->setContextProperty("pageManager", &pageManager);
        engine.rootContext()->setContextProperty("cpuMonitor", &cpuMonitor);
        engine.rootContext()->setContextProperty("batteryMonitor", &batteryMonitor);
        engine.rootContext()->setContextProperty("memoryMonitor", &memoryMonitor);
        engine.rootContext()->setContextProperty("diskMonitor", &diskMonitor);
        engine.rootContext()->setContextProperty("hardwareStatusModel", &hardwareStatusModel);
        engine.rootContext()->setContextProperty("wifiMonitor", &wifiMonitor);
    });

    // ✅ 예외 발생 시 안전한 QML 로딩
    logger.execute([&]() {
        //engine.load(QUrl::fromLocalFile("D:/qtprojects/HMI_sample/Main.qml"));
        QString qmlPath = QCoreApplication::applicationDirPath() + "/Main.qml";
        engine.load(QUrl::fromLocalFile(qmlPath));

    });

    engine.load(QUrl::fromLocalFile("D:/qtprojects/HMI_sample/Main.qml"));
    QTest::qWait(1000);

    const auto rootList = engine.rootObjects();
    QVERIFY2(!rootList.isEmpty(), "❌ No root object");
    rootObject = rootList.first();
    QVERIFY2(rootObject, "❌ Root object null");

    localTestFilePath = "D:/test_local.txt";
    usbTestFilePath = "E:/test_usb.txt";

    if (!QFile::exists(localTestFilePath)) {
        QFile file(localTestFilePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write("Local test file created\n");
            file.close();
        }
    }
    if (!QFile::exists(usbTestFilePath)) {
        QFile file(usbTestFilePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write("USB test file created\n");
            file.close();
        }
    }

    qDebug() << "[CHECK] localTestFilePath exists =" << QFile::exists(localTestFilePath);
    qDebug() << "[CHECK] usbTestFilePath exists =" << QFile::exists(usbTestFilePath);
}

void TestOsFileManager::buttonStressTest()
{
    setUp();

    OsFileManager *osf = qobject_cast<OsFileManager *>(osFileManager);
    QVERIFY2(osf, "❌ OsFileManager cast failed");

    QSignalSpy downloadSpy(osf, &OsFileManager::downloadCompleted);
    QSignalSpy uploadSpy(osf, &OsFileManager::uploadCompleted);
    QVERIFY2(downloadSpy.isValid(), "❌ Spy attach failed: download");
    QVERIFY2(uploadSpy.isValid(), "❌ Spy attach failed: upload");

    // ✅ USB → 로컬 테스트
    clickButtonAndWaitForSignal("copyFromUsbButton", downloadSpy, 10000);

    // ✅ 팝업 닫기
    clickPopupClose();

    // ✅ 로컬 → USB 테스트
    clickButtonAndWaitForSignal("copyToUsbButton", uploadSpy, 10000);

    // ✅ 마지막 팝업 닫기
    clickPopupClose();
}

// ✅ 2. USB → 로컬 파일 복사 테스트
void TestOsFileManager::testCopyFromUsb() {
    OsFileManager fileManager;

    fileManager.copyFromUsb(usbTestFilePath, localTestFilePath);
    QVERIFY(QFile::exists(localTestFilePath)); // 복사 성공 여부 확인
}

// ✅ 3. 로컬 → USB 파일 업로드 테스트
void TestOsFileManager::testCopyToUsb() {
    OsFileManager fileManager;

    fileManager.copyToUsb(localTestFilePath, usbTestFilePath);
    QVERIFY(QFile::exists(usbTestFilePath)); // 업로드 성공 여부 확인
}

// ✅ 4. URL에서 로컬로 다운로드 테스트
void TestOsFileManager::testDownloadFromUrl(){
    // QString downloadPath = localTestFilePath;
    // fileManager->downloadFromUrl("https://example.com/test.txt", downloadPath);

    // QTRY_VERIFY_WITH_TIMEOUT(QFile::exists(downloadPath), 10000); // 다운로드 완료 여부 확인 (10초 이내)
}

// ✅ 5. 로컬 파일을 서버로 업로드 테스트
void TestOsFileManager::testUploadToUrl() {
    // fileManager->uploadToUrl(localTestFilePath, "https://file.io");

    // // 업로드 성공 시 `uploadCompleted` 시그널을 받을 것으로 예상
    // QSignalSpy spy(fileManager, &OsFileManager::uploadCompleted);
    // QTRY_VERIFY_WITH_TIMEOUT(!spy.isEmpty(), 10000);
}

void TestOsFileManager::cleanupTestCase() {
    QVERIFY(QFile::remove(localTestFilePath));
    QVERIFY(QFile::remove(usbTestFilePath));
}



