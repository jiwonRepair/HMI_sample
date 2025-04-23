// #ifndef OSFILEMANAGER_H
// #define OSFILEMANAGER_H

// #include <QObject>
// #include <QString>
// #include <QProcess>
// #include <QFile>
// #include "LoggingDecorator.h"

// // ✅ QFile 객체를 자동 관리하는 RAII 패턴 클래스
// class QFileGuard {
// public:
//     explicit QFileGuard(QFile *file) : file(file) {}
//     ~QFileGuard() { if (file && file->isOpen()) file->close(); }
// private:
//     QFile *file;
// };

// class OsFileManager : public QObject {
//     Q_OBJECT
// public:
//     explicit OsFileManager(QObject *parent = nullptr);

//     // ✅ URL 다운로드 및 업로드 (진행률 지원)
//     Q_INVOKABLE void downloadFromUrl(const QString &url, const QString &savePath);
//     Q_INVOKABLE void uploadToUrl(const QString &filePath, const QString &serverUrl);

//     // ✅ USB 파일 복사 및 업로드 (RAII 패턴 + 로깅 포함)
//     Q_INVOKABLE void copyFromUsb(const QString &usbPath, const QString &destinationPath, bool convertToUtf16 = false);
//     Q_INVOKABLE void copyToUsb(const QString &localPath, const QString &usbPath);

//     Q_INVOKABLE void cancelCopy();
//     Q_INVOKABLE void cancelDownload();
//     Q_INVOKABLE bool isDownloadInProgress();

//     bool isUsbConnected() const;

// signals:
//     void progressChanged(int progress);
//     void downloadCompleted(const QString &savePath);
//     void uploadCompleted(const QString &filePath);
//     void errorOccurred(const QString &error);

//     void downloadStarted();
//     void uploadStarted();

// private:
//     bool cancelRequested;
//     bool downloadInProgress;
//     int lastProgress = -1;  // 진행률 중복 방지용
//     QProcess *downloadProcess;
//     LoggingDecorator logger;
//     void copyFile(const QString &sourcePath, const QString &destPath, bool isUpload);
//     void handleGenericFinished(int exitCode, QProcess::ExitStatus status, const QString &path, QProcess *process, bool isUpload);
//     void parseProgressFromOutput(const QString &stdOut);

// private slots:

// };

// #endif // OSFILEMANAGER_H

#ifndef OSFILEMANAGER_H
#define OSFILEMANAGER_H

#include <QObject>
#include <QString>
#include <QProcess>
#include <QFile>
#include "LoggingDecorator.h"

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

    Q_INVOKABLE void downloadFromUrl(const QString &url, const QString &savePath);
    Q_INVOKABLE void uploadToUrl(const QString &filePath, const QString &serverUrl);
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
    void downloadStarted();
    void uploadStarted();

private:
    bool cancelRequested;
    bool downloadInProgress;
    int lastProgress = -1;
    QProcess *downloadProcess;
    LoggingDecorator logger;

    void copyFile(const QString &sourcePath, const QString &destPath, bool isUpload);
    void handleGenericFinished(int exitCode, QProcess::ExitStatus status, const QString &path, QProcess *process, bool isUpload);
    void parseProgressFromOutput(const QString &stdOut);
};

#endif // OSFILEMANAGER_H
