#ifndef TEST_OSFILEMANAGER_H
#define TEST_OSFILEMANAGER_H

#include <QObject>
#include "osfilemanager.h"
#include <QSignalSpy>  // ✅ QSignalSpy 추가 (신호 감지 용도)
#include <QQmlApplicationEngine>

class TestOsFileManager : public QObject {
    Q_OBJECT

private:
    QQmlApplicationEngine engine;
    OsFileManager *fileManager;
    QObject *rootObject = nullptr;
    QObject *osFileManager = nullptr;
    QObject *pageManager = nullptr;
    QString localTestFilePath;
    QString usbTestFilePath;

    void setUp();

private slots:
    void initTestCase();
    void testCopyFromUsb();
    void testCopyToUsb();
    void clickButtonAndVerify(const QString &buttonId, QSignalSpy &CompletedSpy);
    void clickButton(const QString &buttonId);
    void testDownloadFromUrl();
    void testUploadToUrl();
    void buttonStressTest();
    void cleanupTestCase();
};

#endif // TEST_OSFILEMANAGER_H
