#ifndef TEST_OSFILEMANAGER_H
#define TEST_OSFILEMANAGER_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QSignalSpy>
#include "osfilemanager.h"

class TestOsFileManager : public QObject {
    Q_OBJECT

private:
    QQmlApplicationEngine engine;
    QObject *rootObject = nullptr;
    OsFileManager *osFileManager = nullptr;
    QString localTestFilePath;
    QString usbTestFilePath;

    void setUp();  // QML 초기화 및 osFileManager 연결
    void clickButtonAndWaitForSignal(const QString &buttonId, QSignalSpy &spy, int timeout = 10000);
    void clickPopupClose();

private slots:
    void initTestCase();
    void testCopyFromUsb();
    void testCopyToUsb();
    void testDownloadFromUrl();
    void testUploadToUrl();
    void buttonStressTest();
    void cleanupTestCase();
};

#endif // TEST_OSFILEMANAGER_H
