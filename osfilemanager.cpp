#include "OsFileManager.h"
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QTextStream>
#include <QProcess>
#include <QRegularExpression>
#include <stdexcept>
#include <QtConcurrent>

OsFileManager::OsFileManager(QObject *parent)
    : QObject(parent), cancelRequested(false), downloadInProgress(false), logger("OsFileManager") {
    downloadProcess = new QProcess(this);
    connect(downloadProcess, &QProcess::readyReadStandardOutput, this, &OsFileManager::handleDownloadProgress);
    connect(downloadProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &OsFileManager::handleDownloadFinished);
}

// ✅ USB → 로컬 복사 (비동기 처리)
void OsFileManager::copyFromUsb(const QString &usbPath, const QString &destinationPath, bool convertToUtf16) {
    cancelRequested = false;

    QFuture<void> future = QtConcurrent::run([=]() {
        try {
            QFile sourceFile(usbPath);
            QFile destinationFile(destinationPath);

            if (!sourceFile.open(QIODevice::ReadOnly)) {
                emit errorOccurred("Failed to open USB file.");
                return;
            }
            if (!destinationFile.open(QIODevice::WriteOnly)) {
                emit errorOccurred("Failed to create destination file.");
                return;
            }

            qint64 totalSize = sourceFile.size();
            qint64 copiedSize = 0;
            const qint64 bufferSize = 4096;
            char buffer[bufferSize];

            while (!sourceFile.atEnd()) {
                if (cancelRequested) {
                    emit errorOccurred("Copy operation canceled.");
                    return;
                }

                qint64 bytesRead = sourceFile.read(buffer, bufferSize);
                destinationFile.write(buffer, bytesRead);
                copiedSize += bytesRead;
                int progress = static_cast<int>((copiedSize * 100) / totalSize);

                // ✅ UI 스레드에서 진행률 업데이트
                QMetaObject::invokeMethod(this, "progressChanged", Qt::QueuedConnection, Q_ARG(int, progress));
            }

            emit downloadCompleted(destinationPath);
        } catch (...) {
            emit errorOccurred("Unknown error occurred during file copy.");
        }
    });
}

// ✅ 로컬 → USB 복사 (비동기 처리, `QThreadPool` 사용 가능)
void OsFileManager::copyToUsb(const QString &localPath, const QString &usbPath) {
    cancelRequested = false;

    QThreadPool::globalInstance()->start([=]() {
        try {
            QFile sourceFile(localPath);
            QFile destinationFile(usbPath);

            if (!sourceFile.open(QIODevice::ReadOnly)) {
                emit errorOccurred("Failed to open local file.");
                return;
            }
            if (!destinationFile.open(QIODevice::WriteOnly)) {
                emit errorOccurred("Failed to create USB file.");
                return;
            }

            qint64 totalSize = sourceFile.size();
            qint64 copiedSize = 0;
            const qint64 bufferSize = 4096;
            char buffer[bufferSize];

            while (!sourceFile.atEnd()) {
                if (cancelRequested) {
                    emit errorOccurred("Copy operation canceled.");
                    return;
                }

                qint64 bytesRead = sourceFile.read(buffer, bufferSize);
                destinationFile.write(buffer, bytesRead);
                copiedSize += bytesRead;
                int progress = static_cast<int>((copiedSize * 100) / totalSize);

                // ✅ UI 스레드에서 진행률 업데이트
                QMetaObject::invokeMethod(this, "progressChanged", Qt::QueuedConnection, Q_ARG(int, progress));
            }

            emit uploadCompleted(usbPath);
        } catch (...) {
            emit errorOccurred("Unknown error occurred during file copy.");
        }
    });
}

// ✅ URL 다운로드
void OsFileManager::downloadFromUrl(const QString &url, const QString &savePath) {
    logger.execute([&]() {
        try {
            if (downloadInProgress) {
                throw std::runtime_error("A download is already in progress.");
            }

            downloadInProgress = true;
            QString command = QString("curl -L --progress-bar -o \"%1\" \"%2\"").arg(savePath, url);
            downloadProcess->start(command);
        } catch (const std::exception &e) {
            emit errorOccurred(e.what());
        }
    });
}

// ✅ URL 업로드
void OsFileManager::uploadToUrl(const QString &filePath, const QString &serverUrl) {
    logger.execute([&]() {
        try {
            QString command = QString("curl -T \"%1\" \"%2\"").arg(filePath, serverUrl);
            if (!QProcess::startDetached(command)) {
                throw std::runtime_error("Failed to start upload process.");
            }
            emit uploadCompleted(filePath);
        } catch (const std::exception &e) {
            emit errorOccurred(e.what());
        }
    });
}

// ✅ 다운로드 진행률 처리
void OsFileManager::handleDownloadProgress() {
    QByteArray output = downloadProcess->readAllStandardOutput();
    QString progressData = QString::fromLocal8Bit(output).trimmed();

    // ✅ 정규 표현식을 static 변수로 변경하여 성능 최적화
    static const QRegularExpression progressRegex(R"(\s*(\d+)%\s*)");
    QRegularExpressionMatch match = progressRegex.match(progressData);

    if (match.hasMatch()) {
        int progress = match.captured(1).toInt();
        emit progressChanged(progress);
    }
}

// ✅ 다운로드 완료 처리
void OsFileManager::handleDownloadFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    downloadInProgress = false;
    if (exitCode == 0 && exitStatus == QProcess::NormalExit) {
        emit downloadCompleted("Download finished successfully.");
    } else {
        emit errorOccurred("Download failed.");
    }
}

// ✅ 취소 기능
void OsFileManager::cancelCopy() { cancelRequested = true; }
void OsFileManager::cancelDownload() { if (downloadInProgress) downloadProcess->terminate(); }
bool OsFileManager::isDownloadInProgress() { return downloadInProgress; }
