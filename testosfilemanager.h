#ifndef TEST_OSFILEMANAGER_H
#define TEST_OSFILEMANAGER_H

#include <QObject>
#include "osfilemanager.h"
#include <QSignalSpy>  // ✅ QSignalSpy 추가 (신호 감지 용도)
//#include <QQmlApplicationEngine>

class TestOsFileManager : public QObject {
    Q_OBJECT

private:
    //QQmlApplicationEngine engine;
    OsFileManager *fileManager;
    //QObject *rootObject = nullptr;
    //QObject *osFileManager = nullptr;
    QString localTestFilePath;
    QString usbTestFilePath;

private slots:
    void initTestCase();
    void testCopyFromUsb();
    void testUploadFromUsb();
    //void clickButtonAndVerify(const QString &buttonId, QSignalSpy &downloadCompletedSpy);
    // void testDownloadFromUrl();
    // void testUploadToUrl();
    void cleanupTestCase();
};

#endif // TEST_OSFILEMANAGER_H
