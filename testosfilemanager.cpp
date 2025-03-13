#include "TestOsFileManager.h"
#include <QFile>
#include <QDir>
#include <QTest>
// #include <QtQuick/QQuickItem>
// #include <QtQuick/QQuickView>
#include <QQuickItem>
#include <QQuickWindow>

// ✅ 1. 테스트 시작 시 초기화
void TestOsFileManager::initTestCase() {
    fileManager = new OsFileManager();

// 테스트 파일 경로 설정 (USB 드라이브가 /media/usb 또는 D:/usb로 마운트된다고 가정)
#ifdef Q_OS_WIN
    usbTestFilePath = "E:/test_usb.txt";
    localTestFilePath = "D:/test_local.txt";
#else
    usbTestFilePath = "/media/usb/test_usb.txt";
    localTestFilePath = "/home/user/temp/test_local.txt";
#endif

    // USB에 테스트용 파일 생성
    QFile usbTestFile(usbTestFilePath);
    if (usbTestFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        usbTestFile.write("This is a test file on USB.\n");
        usbTestFile.close();
    }

    QVERIFY(QFile::exists(usbTestFilePath)); // USB 파일이 존재하는지 확인
}

// ✅ 2. USB → 로컬 파일 복사 테스트
void TestOsFileManager::testCopyFromUsb() {
    fileManager->copyFromUsb(usbTestFilePath, localTestFilePath);
    QVERIFY(QFile::exists(localTestFilePath)); // 복사 성공 여부 확인
}

// ✅ 3. 로컬 → USB 파일 업로드 테스트
void TestOsFileManager::testUploadFromUsb() {
    fileManager->uploadFromUsb(localTestFilePath, usbTestFilePath);
    QVERIFY(QFile::exists(usbTestFilePath)); // 업로드 성공 여부 확인
}

// // ✅ 4. URL에서 로컬로 다운로드 테스트
// void TestOsFileManager::testDownloadFromUrl() {
//     QString downloadPath = localTestFilePath;
//     fileManager->downloadFromUrl("https://example.com/test.txt", downloadPath);

//     QTRY_VERIFY_WITH_TIMEOUT(QFile::exists(downloadPath), 10000); // 다운로드 완료 여부 확인 (10초 이내)
// }

// // ✅ 5. 로컬 파일을 서버로 업로드 테스트
// void TestOsFileManager::testUploadToUrl() {
//     fileManager->uploadToUrl(localTestFilePath, "https://file.io");

//     // 업로드 성공 시 `uploadCompleted` 시그널을 받을 것으로 예상
//     QSignalSpy spy(fileManager, &OsFileManager::uploadCompleted);
//     QTRY_VERIFY_WITH_TIMEOUT(!spy.isEmpty(), 10000);
// }

// ✅ 6. 테스트 종료 후 파일 정리
void TestOsFileManager::cleanupTestCase() {
    QVERIFY(QFile::remove(localTestFilePath));
    QVERIFY(QFile::remove(usbTestFilePath));
    delete fileManager;
}

// void TestOsFileManager::clickButtonAndVerify(const QString &buttonId, QSignalSpy &downloadCompletedSpy)
// {
//     // ✅ 버튼 찾기
//     QQuickItem *button = rootObject->findChild<QQuickItem*>(buttonId);
//     QVERIFY2(button, QString("❌ ERROR: %1 not found!").arg(buttonId).toUtf8().constData());

//     // ✅ 버튼이 속한 윈도우 가져오기
//     QQuickWindow *window = button->window();
//     QVERIFY2(window, QString("❌ ERROR: QQuickWindow not found for %1!").arg(buttonId).toUtf8().constData());

//     // ✅ 클릭 이벤트 시뮬레이션
//     QPoint center = button->mapToScene(QPointF(button->width() / 2, button->height() / 2)).toPoint();
//     downloadCompletedSpy.clear();
//     QTest::mouseClick(window, Qt::LeftButton, Qt::NoModifier, center);

//     // ✅ `downloadCompleted` 신호가 발생했는지 확인
//     QTRY_VERIFY(downloadCompletedSpy.count() > 0);
//     //QTRY_VERIFY_WITH_TIMEOUT(downloadCompletedSpy.count() > 0, 1600000);  // ✅ 10분(600,000ms)까지 대기
// }


