#include "osfilemanager.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QStorageInfo>
#include <QStandardPaths>

#ifdef Q_OS_WIN
#define COPY_COMMAND "robocopy"
#else
#define COPY_COMMAND "rsync"
#endif

OsFileManager::OsFileManager(QObject *parent) : QObject(parent), cancelRequested(false), downloadInProgress(false), logger("OsFileManager") {
    downloadProcess = new QProcess(this);
    connect(downloadProcess, &QProcess::readyReadStandardOutput, this, &OsFileManager::handleDownloadProgress);
    connect(downloadProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &OsFileManager::handleDownloadFinished);
}

// ✅ USB → 로컬 복사 (`copyFromUsb`)
void OsFileManager::copyFromUsb(const QString &usbPath, const QString &destinationPath, bool convertToUtf16) {
    if (!QFileInfo::exists(usbPath)) {
        emit errorOccurred("Source file does not exist.");
        logger.log("ERROR: Source file does not exist: " + usbPath);
        return;
    }

    QString sourceDir = QFileInfo(usbPath).absolutePath();
    QString fileName = QFileInfo(usbPath).fileName();
    QString destDir = QFileInfo(destinationPath).absolutePath();

    QDir().mkpath(destDir); // 대상 폴더 없으면 생성

    QProcess *process = new QProcess(this);

    QStringList args;
    args << fileName  // 복사할 파일명
         << "/nfl" << "/ndl" << "/njh" << "/njs" << "/nc" << "/ns"
         << "/bytes" << "/r:0" << "/w:0";
    args.prepend(QDir::toNativeSeparators(destDir));      // 대상
    args.prepend(QDir::toNativeSeparators(sourceDir));    // 원본
    args.prepend("robocopy");

    QString command = args.join(" ");
    logger.log("[INFO] Starting robocopy: " + command);

    connect(process, &QProcess::readyReadStandardOutput, this, [=]() {
        QString output = process->readAllStandardOutput();
        logger.log("[robocopy stdout]\n" + output);

        static const QRegularExpression regex(R"(\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+))");
        QRegularExpressionMatch match = regex.match(output);
        if (match.hasMatch()) {
            qint64 totalBytes = match.captured(3).toLongLong();
            qint64 copiedBytes = match.captured(4).toLongLong();
            if (totalBytes > 0) {
                int percent = static_cast<int>((copiedBytes * 100) / totalBytes);
                emit progressChanged(percent);
                logger.log(QString("[progress] %1 / %2 bytes (%3%)")
                               .arg(copiedBytes).arg(totalBytes).arg(percent));
            }
        }
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [=](int exitCode, QProcess::ExitStatus) {
                if (exitCode <= 7) {
                    emit progressChanged(100);
                    emit downloadCompleted(destinationPath);
                    logger.log("[INFO] robocopy finished successfully. File copied to: " + destinationPath);
                } else {
                    emit errorOccurred("robocopy failed. Exit code: " + QString::number(exitCode));
                    logger.log("[ERROR] robocopy failed. Exit code: " + QString::number(exitCode));
                }
                process->deleteLater();
            });

    process->start("cmd.exe", QStringList() << "/c" << command);
}


// ✅ 로컬 → USB 복사 (`copyToUsb`)
void OsFileManager::copyToUsb(const QString &localPath, const QString &usbPath) {
    logger.execute([=]() {
        if (!QFileInfo::exists(localPath)) {
            emit errorOccurred("Error: Local file does not exist.");
            return;
        }

        QString usbDirPath = QFileInfo(usbPath).absolutePath();
        QDir usbDir(usbDirPath);
        if (!usbDir.exists()) {
            usbDir.mkpath(usbDirPath);
        }

        QStorageInfo usbStorage(usbPath);
        if (usbStorage.isValid() && usbStorage.bytesAvailable() < QFileInfo(localPath).size()) {
            emit errorOccurred("Error: Not enough space on USB drive.");
            return;
        }

        QProcess *process = new QProcess(this);

        static const QRegularExpression robocopyProgressRegex(R"(\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+))");

        QString sourceDir = QFileInfo(localPath).absolutePath();
        QString fileName = QFileInfo(localPath).fileName();
        QString destDir = QFileInfo(usbPath).absolutePath();

        QStringList args;
        args << fileName << "/COPY" << "/Z" << "/NFL" << "/NDL" << "/NJH" << "/NJS" << "/NC" << "/NS" << "/BYTES" << "/R:0" << "/W:0";
        args.prepend(QDir::toNativeSeparators(destDir));
        args.prepend(QDir::toNativeSeparators(sourceDir));
        args.prepend("robocopy");

        logger.execute([=]() {
            qDebug() << "[INFO] robocopy to USB command:" << args.join(" ");
        });

        connect(process, &QProcess::readyReadStandardOutput, this, [=]() {
            QString output = process->readAllStandardOutput();
            logger.execute([&]() { qDebug() << "[robocopy stdout]" << output; });

            QRegularExpressionMatch match = robocopyProgressRegex.match(output);
            if (match.hasMatch()) {
                qint64 totalBytes = match.captured(3).toLongLong();
                qint64 copiedBytes = match.captured(4).toLongLong();

                if (totalBytes > 0) {
                    int percent = static_cast<int>((copiedBytes * 100) / totalBytes);
                    emit progressChanged(percent);
                }
            }
        });

        connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int exitCode, QProcess::ExitStatus status) {
            if (exitCode <= 7) {
                emit progressChanged(100);
                emit uploadCompleted(usbPath);
                logger.execute([&]() { qDebug() << "[INFO] File copy to USB completed: " << usbPath; });
            } else {
                emit errorOccurred("File copy to USB failed. Exit code: " + QString::number(exitCode));
            }
            process->deleteLater();
        });

        process->start("cmd.exe", QStringList() << "/c" << args.join(" "));
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
