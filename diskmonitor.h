#ifndef DISKMONITOR_H
#define DISKMONITOR_H

#ifdef _WIN32
#include <winsock2.h>  // 반드시 windows.h 보다 먼저!
#endif

#include <QObject>
#include <QTimer>
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#elif defined(Q_OS_LINUX)
#include <sys/statvfs.h>
#include <QFile>
#include <QTextStream>
#endif

class DiskMonitor : public QObject {
    Q_OBJECT
    Q_PROPERTY(int diskUsage READ diskUsage NOTIFY diskUsageChanged)

public:
    explicit DiskMonitor(QObject *parent = nullptr) : QObject(parent), m_diskUsage(0) {
        connect(&timer, &QTimer::timeout, this, &DiskMonitor::updateDiskUsage);
        timer.start(5000); // 5초마다 업데이트
        QTimer::singleShot(0, this, &DiskMonitor::updateDiskUsage); // 실행 후 즉시 업데이트
    }

    int diskUsage() const {
        return m_diskUsage;
    }

public slots:
    void updateDiskUsage() {
        //qDebug() << "[DiskMonitor] 디스크 사용량 업데이트 중...";

#ifdef Q_OS_WIN
        //qDebug() << "[DiskMonitor] Windows 환경 - GetDiskFreeSpaceEx() 사용";
        ULARGE_INTEGER freeBytesAvailable, totalBytes, totalFreeBytes;
        if (GetDiskFreeSpaceEx(L"C:\\", &freeBytesAvailable, &totalBytes, &totalFreeBytes)) {
            m_diskUsage = static_cast<int>(100 - (totalFreeBytes.QuadPart * 100 / totalBytes.QuadPart));
            //qDebug() << "[DiskMonitor] 디스크 사용량:" << m_diskUsage << "%";
            emit diskUsageChanged();
        } else {
            //qDebug() << "[DiskMonitor] 디스크 사용량 조회 실패!";
        }

#elif defined(Q_OS_LINUX)
        qDebug() << "[DiskMonitor] Linux 환경 - statvfs() 사용";
        struct statvfs stat;
        if (statvfs("/", &stat) == 0) {
            m_diskUsage = static_cast<int>(100 - (stat.f_bavail * 100 / stat.f_blocks));
            qDebug() << "[DiskMonitor] 디스크 사용량:" << m_diskUsage << "%";
            emit diskUsageChanged();
        } else {
            qDebug() << "[DiskMonitor] 디스크 사용량 조회 실패!";
        }
#endif
    }

signals:
    void diskUsageChanged();

private:
    int m_diskUsage;
    QTimer timer;
};

#endif // DISKMONITOR_H
