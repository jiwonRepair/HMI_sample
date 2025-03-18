#ifndef OSFILEMANAGER_H
#define OSFILEMANAGER_H

#include <QObject>
#include <QString>
#include "LoggingDecorator.h"

class OsFileManager : public QObject {
    Q_OBJECT
public:
    explicit OsFileManager(QObject *parent = nullptr);

    // ✅ URL 다운로드 및 업로드
    Q_INVOKABLE void downloadFromUrl(const QString &url, const QString &savePath);
    Q_INVOKABLE void uploadToUrl(const QString &filePath, const QString &serverUrl);

    // ✅ USB 파일 복사 및 업로드
    Q_INVOKABLE void copyFromUsb(const QString &usbPath, const QString &destinationPath);
    Q_INVOKABLE void copyToUsb(const QString &usbPath, const QString &serverUrl);

    Q_INVOKABLE bool isDownloadInProgress();
    Q_INVOKABLE void cancelDownload();

signals:
    void downloadCompleted(const QString &savePath);
    void uploadCompleted(const QString &filePath);

private:
    LoggingDecorator logger;
    bool downloadInProgress;
};

#endif // OSFILEMANAGER_H
