#ifndef TEST_OSFILEMANAGER_H
#define TEST_OSFILEMANAGER_H

#include <QObject>
#include "osfilemanager.h"

class TestOsFileManager : public QObject {
    Q_OBJECT

private:
    OsFileManager *fileManager;
    QString localTestFilePath;
    QString usbTestFilePath;

private slots:
    void initTestCase();
    void testCopyFromUsb();
    void testUploadFromUsb();
    void testDownloadFromUrl();
    void testUploadToUrl();
    void cleanupTestCase();
};

#endif // TEST_OSFILEMANAGER_H
