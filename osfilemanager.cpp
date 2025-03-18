#include "OsFileManager.h"
#include <QDebug>
#include <QFile>
#include <QProcess>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

OsFileManager::OsFileManager(QObject *parent)
    : QObject(parent), logger("OsFileManager"), downloadInProgress(false) {}

// ✅ URL 다운로드 (curl 사용)
void OsFileManager::downloadFromUrl(const QString &url, const QString &savePath) {
    logger.execute([&]() {
        downloadInProgress = true;
#ifdef Q_OS_WIN
        QString command = QString("curl -L -o \"%1\" \"%2\"").arg(savePath, url);
#elif defined(Q_OS_LINUX) || defined(Q_OS_MACOS)
        QString command = QString("nohup curl -L -o \"%1\" \"%2\" > /dev/null 2>&1 &").arg(savePath, url);
#endif
        if (!QProcess::startDetached(command)) {
            throw std::runtime_error("Failed to start curl process for download.");
        }
        emit downloadCompleted(savePath);
        qDebug() << "[INFO] Download started: " << url;
    });
}

// ✅ URL 업로드 (curl 사용)
void OsFileManager::uploadToUrl(const QString &filePath, const QString &serverUrl) {
    logger.execute([&]() {
        QString command = QString("curl -T \"%1\" \"%2\"").arg(filePath, serverUrl);
        if (!QProcess::startDetached(command)) {
            throw std::runtime_error("Failed to start curl process for upload.");
        }
        emit uploadCompleted(filePath);
        qDebug() << "[INFO] Upload started: " << filePath;
    });
}

#include <QDir>

void OsFileManager::copyFromUsb(const QString &usbPath, const QString &destinationPath) {
    logger.execute([&]() {
        qDebug() << "[DEBUG] Checking USB file path: " << usbPath;

        // ✅ 파일 경로를 QDir을 통해 정규화 (슬래시 변환)
        QString normalizedUsbPath = QDir::toNativeSeparators(usbPath);

        qDebug() << "[DEBUG] Normalized USB path: " << normalizedUsbPath;

        if (!QFile::exists(normalizedUsbPath)) {
            throw std::runtime_error("USB file does not exist: " + normalizedUsbPath.toStdString());
        }

        qDebug() << "[INFO] USB file found, starting copy to: " << destinationPath;

        if (QFile::exists(destinationPath)) {
            QFile::remove(destinationPath);  // 기존 파일 삭제
        }

        if (!QFile::copy(normalizedUsbPath, destinationPath)) {
            throw std::runtime_error("Failed to copy file from USB to local storage.");
        }

        emit downloadCompleted(destinationPath);
        qDebug() << "[INFO] File copied successfully from USB to local: " << destinationPath;
    });
}



// ✅ 로컬 하드디스크에서 USB로 파일 이동 (업로드 개념)
void OsFileManager::copyToUsb(const QString &localPath, const QString &usbPath) {
    logger.execute([&]() {
        if (QFile::exists(usbPath)) {
            QFile::remove(usbPath);
        }
        if (!QFile::copy(localPath, usbPath)) {
            throw std::runtime_error("Failed to copy file from local storage to USB.");
        }
        emit uploadCompleted(usbPath);
        qDebug() << "[INFO] File copied from local to USB: " << usbPath;
    });
}

bool OsFileManager::isDownloadInProgress() {
    return downloadInProgress;
}

void OsFileManager::cancelDownload() {
    logger.execute([&]() {
#ifdef Q_OS_WIN
        qDebug() << "[WARNING] Canceling downloads on Windows (MinGW) requires manual process termination.";
#elif defined(Q_OS_LINUX) || defined(Q_OS_MACOS)
        qDebug() << "[WARNING] Canceling downloads on Linux/macOS requires manual process termination.";
#endif
        downloadInProgress = false;
        qDebug() << "[INFO] Download canceled.";
    });
}


