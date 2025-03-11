#ifndef OSFILEMANAGER_H
#define OSFILEMANAGER_H

#include <QObject>
#include <QString>
#include "LoggingDecorator.h"

class OsFileManager : public QObject {
    Q_OBJECT
public:
    explicit OsFileManager(QObject *parent = nullptr);

    Q_INVOKABLE void downloadFile(const QString &url, const QString &savePath);
    Q_INVOKABLE void uploadFile(const QString &filePath, const QString &serverUrl);
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
