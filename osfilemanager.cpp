#include "OsFileManager.h"
#include <QDebug>
#include <QProcess>

#ifdef Q_OS_WIN
#include <bits.h>
#include <windows.h>
#endif

OsFileManager::OsFileManager(QObject *parent)
    : QObject(parent), logger("OsFileManager"), downloadInProgress(false) {}

// ✅ 파일 다운로드 (RAII + 예외처리 + 로깅 적용)
void OsFileManager::downloadFile(const QString &url, const QString &savePath) {
    logger.execute([&]() {
        downloadInProgress = true;
#ifdef Q_OS_WIN
        // Windows BITS 다운로드
        IBackgroundCopyManager* pManager = nullptr;
        IBackgroundCopyJob* pJob = nullptr;

        CoInitialize(nullptr);
        CoCreateInstance(__uuidof(BackgroundCopyManager), nullptr, CLSCTX_LOCAL_SERVER, __uuidof(IBackgroundCopyManager), (void**)&pManager);

        if (!pManager) throw std::runtime_error("Failed to create BackgroundCopyManager");

        struct BackgroundCopyGuard {
            IBackgroundCopyManager* manager;
            IBackgroundCopyJob* job;
            ~BackgroundCopyGuard() {
                if (job) job->Release();
                if (manager) manager->Release();
                CoUninitialize();
            }
        } guard { pManager, pJob };

        pManager->CreateJob(L"FileDownloadJob", BG_JOB_TYPE_DOWNLOAD, nullptr, &pJob);
        if (!pJob) throw std::runtime_error("Failed to create BITS download job");

        pJob->AddFile(reinterpret_cast<const wchar_t*>(url.utf16()),
                      reinterpret_cast<const wchar_t*>(savePath.utf16()));
        pJob->Resume();

#elif defined(Q_OS_LINUX) || defined(Q_OS_MACOS)
        // Linux/macOS: curl 사용 (nohup으로 UI 종료 후에도 다운로드 유지)
        QString command = QString("nohup curl -L -o \"%1\" \"%2\" > /dev/null 2>&1 &").arg(savePath, url);
        if (!QProcess::startDetached(command)) {
            throw std::runtime_error("Failed to start download process");
        }
#endif
        qDebug() << "[INFO] Download started: " << url;
    });
}

// ✅ 파일 업로드 (RAII + 예외처리 + 로깅 적용)
void OsFileManager::uploadFile(const QString &filePath, const QString &serverUrl) {
    logger.execute([&]() {
#ifdef Q_OS_WIN
        // Windows BITS 업로드
        IBackgroundCopyManager* pManager = nullptr;
        IBackgroundCopyJob* pJob = nullptr;

        CoInitialize(nullptr);
        CoCreateInstance(__uuidof(BackgroundCopyManager), nullptr, CLSCTX_LOCAL_SERVER, __uuidof(IBackgroundCopyManager), (void**)&pManager);

        if (!pManager) throw std::runtime_error("Failed to create BackgroundCopyManager");

        struct BackgroundCopyGuard {
            IBackgroundCopyManager* manager;
            IBackgroundCopyJob* job;
            ~BackgroundCopyGuard() {
                if (job) job->Release();
                if (manager) manager->Release();
                CoUninitialize();
            }
        } guard { pManager, pJob };

        pManager->CreateJob(L"FileUploadJob", BG_JOB_TYPE_UPLOAD, nullptr, &pJob);
        if (!pJob) throw std::runtime_error("Failed to create BITS upload job");

        pJob->AddFile(reinterpret_cast<const wchar_t*>(filePath.utf16()),
                      reinterpret_cast<const wchar_t*>(serverUrl.utf16()));
        pJob->Resume();

#elif defined(Q_OS_LINUX) || defined(Q_OS_MACOS)
        // Linux/macOS: curl 사용 (nohup으로 UI 종료 후에도 업로드 유지)
        QString command = QString("nohup curl -T \"%1\" \"%2\" > /dev/null 2>&1 &").arg(filePath, serverUrl);
        if (!QProcess::startDetached(command)) {
            throw std::runtime_error("Failed to start upload process");
        }
#endif
        qDebug() << "[INFO] Upload started: " << filePath;
    });
}

// ✅ 다운로드 진행 상태 확인
bool OsFileManager::isDownloadInProgress() {
    return downloadInProgress;
}

// ✅ 다운로드 취소 (Windows BITS만 지원, Linux/macOS는 사용자가 직접 종료 필요)
void OsFileManager::cancelDownload() {
    logger.execute([&]() {
#ifdef Q_OS_WIN
        // Windows BITS 다운로드 취소
        IBackgroundCopyManager* pManager = nullptr;
        IEnumBackgroundCopyJobs* pEnumJobs = nullptr;
        IBackgroundCopyJob* pJob = nullptr;
        ULONG count = 0;

        CoInitialize(nullptr);
        CoCreateInstance(__uuidof(BackgroundCopyManager), nullptr, CLSCTX_LOCAL_SERVER, __uuidof(IBackgroundCopyManager), (void**)&pManager);
        if (!pManager) throw std::runtime_error("Failed to create BackgroundCopyManager");

        struct BackgroundCopyGuard {
            IBackgroundCopyManager* manager;
            ~BackgroundCopyGuard() { if (manager) manager->Release(); CoUninitialize(); }
        } guard { pManager };

        pManager->EnumJobs(0, &pEnumJobs);
        if (!pEnumJobs) throw std::runtime_error("Failed to enumerate BITS jobs");

        while (pEnumJobs->Next(1, &pJob, &count) == S_OK) {
            pJob->Cancel();
            pJob->Release();
        }

        pEnumJobs->Release();
#elif defined(Q_OS_LINUX) || defined(Q_OS_MACOS)
        qDebug() << "[WARNING] Canceling downloads on Linux/macOS requires manual process termination.";
#endif
        downloadInProgress = false;
        qDebug() << "[INFO] Download canceled.";
    });
}
