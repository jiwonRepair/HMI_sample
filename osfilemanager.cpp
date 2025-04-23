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

// ✅ USB → 로컬 복사 (`copyFromUsb`)
// void OsFileManager::copyFromUsb(const QString &usbPath, const QString &destinationPath, bool convertToUtf16) {
//     logger.execute([=]() {
//         try {
//             if (!isUsbConnected()){
//                 emit errorOccurred("USB drive does not exist.");
//                 return;
//             }

//             emit downloadStarted();  // ✅ USB → 로컬도 다운로드 시그널

//             if (!QFileInfo::exists(usbPath)) {
//                 emit errorOccurred("Source file does not exist.");
//                 logger.log("ERROR: Source file does not exist: " + usbPath);
//                 return;
//             }

//             QString sourceDir = QFileInfo(usbPath).absolutePath();
//             QString fileName = QFileInfo(usbPath).fileName();
//             QString destDir = QFileInfo(destinationPath).absolutePath();

//             QDir().mkpath(destDir); // 대상 폴더 없으면 생성

//             QProcess *process = new QProcess(this);

//             QStringList args;
//             args << fileName
//                  << "/bytes" << "/r:0" << "/w:0"
//                  << "/tee";  // 🔥 실시간 출력 강제

//             args.prepend(QDir::toNativeSeparators(destDir));   // 대상
//             args.prepend(QDir::toNativeSeparators(sourceDir)); // 원본
//             args.prepend("robocopy");

//             QString command = args.join(" ");
//             logger.log("[INFO] Starting robocopy: " + command);

//             // ✅ stdout 진행률 파싱
//             connect(process, &QProcess::readyReadStandardOutput, this, [=]() {
//                 QString stdOut = process->readAllStandardOutput();
//                 logger.log("[robocopy stdout]\n" + stdOut);

//                 static const QRegularExpression percentRegex(R"((\d+\.\d+)%|\d+%)");
//                 const auto lines = stdOut.split('\n');
//                 for (const QString &line : lines) {
//                     QRegularExpressionMatch match = percentRegex.match(line.trimmed());
//                     if (match.hasMatch()) {
//                         QString percentText = match.captured(0);
//                         int dotIndex = percentText.indexOf('.');
//                         int percent = dotIndex > 0
//                                           ? QStringView(percentText).left(dotIndex).toInt()
//                                           : percentText.remove('%').toInt();
//                         emit progressChanged(percent);
//                         //logger.log(QString("[progress] %1%").arg(percent));
//                     }
//                 }
//             });

//             // ✅ 복사 완료 시 처리
//             connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
//                     this, [=](int exitCode, QProcess::ExitStatus) {
//                         if (exitCode <= 7) {
//                             emit progressChanged(100);
//                             emit downloadCompleted(destinationPath);
//                             logger.log("[INFO] robocopy finished successfully. File copied to: " + destinationPath);
//                         } else {
//                             emit errorOccurred("robocopy failed. Exit code: " + QString::number(exitCode));
//                             logger.log("[ERROR] robocopy failed. Exit code: " + QString::number(exitCode));
//                         }
//                         process->deleteLater();
//                     });

//             process->start("cmd.exe", QStringList() << "/c" << command);

//         } catch (const std::exception &e) {
//             emit errorOccurred(e.what());
//             logger.log("[EXCEPTION] " + QString::fromUtf8(e.what()));
//         } catch (...) {
//             emit errorOccurred("Unknown error occurred during copy.");
//             logger.log("[EXCEPTION] Unknown error occurred during copy.");
//         }
//     });
// }

void OsFileManager::copyFromUsb(const QString &usbPath, const QString &localPath, bool convertToUtf16) {
    if (!isUsbConnected()) {
        emit errorOccurred("USB drive does not exist.");
        return;
    }

    emit downloadStarted();  // ✅ 다운로드 시작 알림
    copyFile(usbPath, localPath, false);
}

// ✅ 로컬 → USB 복사 (`copyToUsb`)
// void OsFileManager::copyToUsb(const QString &localPath, const QString &usbPath) {
//     logger.execute([=]() {
//         try {
//             if (!isUsbConnected()){
//                 emit errorOccurred("USB drive does not exist.");
//                 return;
//             }

//             if (!QFileInfo::exists(localPath)) {
//                 emit errorOccurred("Source file does not exist.");
//                 logger.log("ERROR: Source file does not exist: " + localPath);
//                 return;
//             }

//             QString sourceDir = QFileInfo(localPath).absolutePath();
//             QString fileName = QFileInfo(localPath).fileName();
//             QString destDir = QFileInfo(usbPath).absolutePath();

//             QDir().mkpath(destDir); // 대상 폴더 없으면 생성

//             QProcess *process = new QProcess(this);

//             QStringList args;
//             args << fileName
//                  << "/bytes" << "/r:0" << "/w:0"
//                  << "/tee";  // 🔥 실시간 진행률 출력

//             args.prepend(QDir::toNativeSeparators(destDir));   // 대상
//             args.prepend(QDir::toNativeSeparators(sourceDir)); // 원본
//             args.prepend("robocopy");

//             QString command = args.join(" ");
//             logger.log("[INFO] Starting robocopy: " + command);

//             // ✅ stdout 진행률 파싱
//             connect(process, &QProcess::readyReadStandardOutput, this, [=]() {
//                 QString stdOut = process->readAllStandardOutput();
//                 logger.log("[robocopy stdout]\n" + stdOut);

//                 static const QRegularExpression percentRegex(R"((\d+\.\d+)%|\d+%)");
//                 const auto lines = stdOut.split('\n');
//                 for (const QString &line : lines) {
//                     QRegularExpressionMatch match = percentRegex.match(line.trimmed());
//                     if (match.hasMatch()) {
//                         QString percentText = match.captured(0);
//                         int dotIndex = percentText.indexOf('.');
//                         int percent = dotIndex > 0
//                                           ? QStringView(percentText).left(dotIndex).toInt()
//                                           : percentText.remove('%').toInt();
//                         emit progressChanged(percent);
//                         //logger.log(QString("[progress] %1%").arg(percent));
//                     }
//                 }
//             });

//             connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
//                     this, [=](int exitCode, QProcess::ExitStatus) {
//                         logger.log("[DEBUG] connected to QProcess::finished for upload");
//                         if (exitCode <= 7) {
//                             emit progressChanged(100); // UI 리셋 타이밍 뒤로 밀기
//                             emit uploadCompleted(usbPath);
//                             logger.log("[INFO] robocopy finished successfully. File copied to: " + usbPath);
//                         } else {
//                             emit errorOccurred("robocopy failed. Exit code: " + QString::number(exitCode));
//                             logger.log("[ERROR] robocopy failed. Exit code: " + QString::number(exitCode));
//                         }

//                         process->deleteLater();
//                     });

//             process->start("cmd.exe", QStringList() << "/c" << command);

//         } catch (const std::exception &e) {
//             emit errorOccurred(e.what());
//             logger.log("[EXCEPTION] " + QString::fromUtf8(e.what()));
//         } catch (...) {
//             emit errorOccurred("Unknown error occurred during copy.");
//             logger.log("[EXCEPTION] Unknown error occurred during copy.");
//         }
//     });
// }

void OsFileManager::copyToUsb(const QString &localPath, const QString &usbPath) {
    if (!isUsbConnected()) {
        emit errorOccurred("USB drive does not exist.");
        return;
    }

    emit uploadStarted();  // ✅ 업로드 시작 알림
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
            args << fileName << "/bytes" << "/r:0" << "/w:0" << "/tee";
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

            emit downloadStarted();  // ✅ 다운로드 시작 알림

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
        emit progressChanged(100);
        if (isUpload)
            emit uploadCompleted(path);
        else
            emit downloadCompleted(path);
        logger.log("[INFO] robocopy finished successfully. File copied to: " + path);
    } else {
        emit errorOccurred((isUpload ? "Upload" : "Download") + QString(" failed. Exit code: %1").arg(exitCode));
        logger.log("[ERROR] robocopy failed. Exit code: " + QString::number(exitCode));
    }

    if (process)
        process->deleteLater();
}

void OsFileManager::parseProgressFromOutput(const QString &stdOut) {
    static const QRegularExpression percentRegex(R"((\d+\.\d+)%|\d+%)");
    const auto lines = stdOut.split('\n');
    for (const QString &line : lines) {
        QRegularExpressionMatch match = percentRegex.match(line.trimmed());
        if (match.hasMatch()) {
            QString percentText = match.captured(0);
            int dotIndex = percentText.indexOf('.');
            int percent = dotIndex > 0
                              ? QStringView(percentText).left(dotIndex).toInt()
                              : percentText.remove('%').toInt();
            // ✅ 중복 emit 방지
            if (percent != lastProgress) {
                lastProgress = percent;
                emit progressChanged(percent);
            }
        }
    }
}


