#include "osfilemanager.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QStorageInfo>
#include <QStandardPaths>
#include <QStringView>
#include <QTimer>

#ifdef Q_OS_WIN
#define COPY_COMMAND "robocopy"
#else
#define COPY_COMMAND "rsync"
#endif

OsFileManager::OsFileManager(QObject *parent) : QObject(parent), cancelRequested(false), downloadInProgress(false), logger("OsFileManager") {
    downloadProcess = new QProcess(this);
    connect(downloadProcess, &QProcess::readyReadStandardOutput, this, [=]() {
        parseProgressFromOutput(downloadProcess->readAllStandardOutput());
    });

    connect(downloadProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, [=](int exitCode, QProcess::ExitStatus status) {
        handleGenericFinished(exitCode, status, "Download finished successfully.", nullptr, false);
    });

}

void OsFileManager::copyFromUsb(const QString &usbPath, const QString &localPath, bool convertToUtf16) {
    if (!isUsbConnected()) {
        emit errorOccurred("USB drive does not exist.");
        return;
    }

    copyFile(usbPath, localPath, false);
}

void OsFileManager::copyToUsb(const QString &localPath, const QString &usbPath) {
    if (!isUsbConnected()) {
        emit errorOccurred("USB drive does not exist.");
        return;
    }

    copyFile(localPath, usbPath, true);
}

void OsFileManager::copyFile(const QString &sourcePath, const QString &destPath, bool isUpload)
{
    lastProgress = -1;
    logger.execute([=]() {
        try {
            if (!QFileInfo::exists(sourcePath)) {
                emit errorOccurred("Source file does not exist.");
                return;
            }

            QString sourceDir = QFileInfo(sourcePath).absolutePath();
            QString fileName  = QFileInfo(sourcePath).fileName();
            QString destDir   = QFileInfo(destPath).absolutePath();
            QDir().mkpath(destDir);

            QProcess *process = new QProcess(this);

            QStringList args;
            args << fileName << "/bytes"
                             << "/r:0" << "/w:0" << "/tee"
                             << "/IS" << "/IT";  // 덮어쓰기 강제
            args.prepend(QDir::toNativeSeparators(destDir));
            args.prepend(QDir::toNativeSeparators(sourceDir));
            args.prepend("robocopy");

            QString command = args.join(" ");
            logger.log("[INFO] Starting robocopy: " + command);

            connect(process, &QProcess::readyReadStandardOutput, this, [=]() {
                parseProgressFromOutput(process->readAllStandardOutput());
            });

            connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                    this, [=](int exitCode, QProcess::ExitStatus status) {
                        handleGenericFinished(exitCode, status, destPath, process, isUpload);
                    });


            process->start("cmd.exe", QStringList() << "/c" << command);

        } catch (const std::exception &e) {
            emit errorOccurred(e.what());
        } catch (...) {
            emit errorOccurred("Unknown error occurred during copy.");
        }
    });
}



// ✅ URL 다운로드
void OsFileManager::downloadFromUrl(const QString &url, const QString &savePath) {
    lastProgress = -1;
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

// ✅ 취소 기능
void OsFileManager::cancelCopy() { cancelRequested = true; }
void OsFileManager::cancelDownload() { if (downloadInProgress) downloadProcess->terminate(); }
bool OsFileManager::isDownloadInProgress() { return downloadInProgress; }

// ✅ USB 드라이브 확인
bool OsFileManager::isUsbConnected() const {
#ifdef Q_OS_WIN
    const QString usbRoot = "E:/";
#else
    const QString usbRoot = "/media/usb";
#endif

    QFileInfo usbDir(usbRoot);
    bool exists = usbDir.exists() && usbDir.isDir();

    qDebug() << "[USB CHECK] Path:" << usbRoot << " Exists:" << exists;
    return exists;
}

void OsFileManager::handleGenericFinished(int exitCode,
                                          QProcess::ExitStatus status,
                                          const QString &path,
                                          QProcess *process,
                                          bool isUpload)
{
    if (isUpload) {
        logger.log("[DEBUG] handleGenericFinished (upload)");
    } else {
        logger.log("[DEBUG] handleGenericFinished (download)");
        downloadInProgress = false;
    }

    if (exitCode <= 7 && status == QProcess::NormalExit) {
        if (isUpload){
            emit uploadCompleted(path);
        }
        else{
            emit downloadCompleted(path);
        }
        logger.log("[INFO] robocopy finished successfully. File copied to: " + path);
    } else {
        emit errorOccurred((isUpload ? "Upload" : "Download") + QString(" failed. Exit code: %1").arg(exitCode));
        logger.log("[ERROR] robocopy failed. Exit code: " + QString::number(exitCode));
    }

    if (process)
        process->deleteLater();
}

void OsFileManager::parseProgressFromOutput(const QString &stdOut) {
    static const QRegularExpression percentRegex(R"((\d{1,3})(?:\.\d+)?%)");
    static const QRegularExpression lineBreakRegex(R"(\r|\n)");  // ✅ 여기 추가

    QStringList segments = stdOut.split(lineBreakRegex, Qt::SkipEmptyParts);  // ✅ static 사용

    for (int i = 0; i < segments.size(); ++i) {
        QString segment = segments.at(i).trimmed();  // 복사 발생하지만 안전하게 처리
        QRegularExpressionMatch match = percentRegex.match(segment.trimmed());
        if (match.hasMatch()) {
            bool ok = false;
            int percent = match.captured(1).toInt(&ok);
            if (ok && percent >= lastProgress && percent <= 100) {//if (ok && percent != lastProgress && percent < 100) {//if (ok && percent != lastProgress) {
                lastProgress = percent;
                emit progressChanged(percent);
                qDebug() << "[PROGRESS]" << percent << "%";
            }
        }
    }
}


