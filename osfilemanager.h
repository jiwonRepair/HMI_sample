#ifndef OSFILEMANAGER_H
#define OSFILEMANAGER_H

#include <QObject>
#include <QString>
#include <QProcess>
#include <QFile>
#include "LoggingDecorator.h"

// ✅ QFile 객체를 자동 관리하는 RAII 패턴 클래스
class QFileGuard {
public:
    explicit QFileGuard(QFile *file) : file(file) {}
    ~QFileGuard() { if (file && file->isOpen()) file->close(); }
private:
    QFile *file;
};

class OsFileManager : public QObject {
    Q_OBJECT
public:
    explicit OsFileManager(QObject *parent = nullptr);

    // ✅ URL 다운로드 및 업로드 (진행률 지원)
    Q_INVOKABLE void downloadFromUrl(const QString &url, const QString &savePath);
    Q_INVOKABLE void uploadToUrl(const QString &filePath, const QString &serverUrl);

    // ✅ USB 파일 복사 및 업로드 (RAII 패턴 + 로깅 포함)
    Q_INVOKABLE void copyFromUsb(const QString &usbPath, const QString &destinationPath, bool convertToUtf16 = false);
    Q_INVOKABLE void copyToUsb(const QString &localPath, const QString &usbPath);

    Q_INVOKABLE void cancelCopy();
    Q_INVOKABLE void cancelDownload();
    Q_INVOKABLE bool isDownloadInProgress();

    bool isUsbConnected() const;

signals:
    void progressChanged(int progress);
    void downloadCompleted(const QString &savePath);
    void uploadCompleted(const QString &filePath);
    void errorOccurred(const QString &error);

private:
    bool cancelRequested;
    bool downloadInProgress;
    QProcess *downloadProcess;
    LoggingDecorator logger;

private slots:
    void handleDownloadProgress();
    void handleDownloadFinished(int exitCode, QProcess::ExitStatus exitStatus);
};

#endif // OSFILEMANAGER_H
