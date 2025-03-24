#include "osfilemanager.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QStorageInfo>
#include <QStandardPaths>
#include <QStringView>

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
    logger.execute([=]() {
        try {
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
            args << fileName
                 << "/bytes" << "/r:0" << "/w:0"
                 << "/tee";  // 🔥 실시간 출력 강제

            args.prepend(QDir::toNativeSeparators(destDir));   // 대상
            args.prepend(QDir::toNativeSeparators(sourceDir)); // 원본
            args.prepend("robocopy");

            QString command = args.join(" ");
            //logger.log("[INFO] Starting robocopy: " + command);

            // ✅ stdout 진행률 파싱
            connect(process, &QProcess::readyReadStandardOutput, this, [=]() {
                QString stdOut = process->readAllStandardOutput();
                logger.log("[robocopy stdout]\n" + stdOut);

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
                        emit progressChanged(percent);
                        //logger.log(QString("[progress] %1%").arg(percent));
                    }
                }
            });

            // ✅ 복사 완료 시 처리
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

        } catch (const std::exception &e) {
            emit errorOccurred(e.what());
            logger.log("[EXCEPTION] " + QString::fromUtf8(e.what()));
        } catch (...) {
            emit errorOccurred("Unknown error occurred during copy.");
            logger.log("[EXCEPTION] Unknown error occurred during copy.");
        }
    });
}


// ✅ 로컬 → USB 복사 (`copyToUsb`)
void OsFileManager::copyToUsb(const QString &localPath, const QString &usbPath) {
    logger.execute([=]() {
        try {
            if (!QFileInfo::exists(localPath)) {
                emit errorOccurred("Source file does not exist.");
                logger.log("ERROR: Source file does not exist: " + localPath);
                return;
            }

            QString sourceDir = QFileInfo(localPath).absolutePath();
            QString fileName = QFileInfo(localPath).fileName();
            QString destDir = QFileInfo(usbPath).absolutePath();

            QDir().mkpath(destDir); // 대상 폴더 없으면 생성

            QProcess *process = new QProcess(this);

            QStringList args;
            args << fileName
                 << "/bytes" << "/r:0" << "/w:0"
                 << "/tee";  // 🔥 실시간 진행률 출력

            args.prepend(QDir::toNativeSeparators(destDir));   // 대상
            args.prepend(QDir::toNativeSeparators(sourceDir)); // 원본
            args.prepend("robocopy");

            QString command = args.join(" ");
            logger.log("[INFO] Starting robocopy: " + command);

            // ✅ stdout 진행률 파싱
            connect(process, &QProcess::readyReadStandardOutput, this, [=]() {
                QString stdOut = process->readAllStandardOutput();
                //logger.log("[robocopy stdout]\n" + stdOut);

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
                        emit progressChanged(percent);
                        //logger.log(QString("[progress] %1%").arg(percent));
                    }
                }
            });

            connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                    this, [=](int exitCode, QProcess::ExitStatus) {
                        if (exitCode <= 7) {
                            emit progressChanged(100);
                            emit downloadCompleted(usbPath);
                            logger.log("[INFO] robocopy finished successfully. File copied to: " + usbPath);
                        } else {
                            emit errorOccurred("robocopy failed. Exit code: " + QString::number(exitCode));
                            logger.log("[ERROR] robocopy failed. Exit code: " + QString::number(exitCode));
                        }
                        process->deleteLater();
                    });

            process->start("cmd.exe", QStringList() << "/c" << command);

        } catch (const std::exception &e) {
            emit errorOccurred(e.what());
            logger.log("[EXCEPTION] " + QString::fromUtf8(e.what()));
        } catch (...) {
            emit errorOccurred("Unknown error occurred during copy.");
            logger.log("[EXCEPTION] Unknown error occurred during copy.");
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
