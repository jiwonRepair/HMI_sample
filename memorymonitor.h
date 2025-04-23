#ifndef MEMORYMONITOR_H
#define MEMORYMONITOR_H

#ifdef _WIN32
#include <winsock2.h>  // 반드시 windows.h 보다 먼저!
#endif

#include <QObject>
#include <QTimer>
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#elif defined(Q_OS_LINUX)
#include <QFile>
#include <QTextStream>
#endif

class MemoryMonitor : public QObject {
    Q_OBJECT
    Q_PROPERTY(int memoryUsage READ memoryUsage NOTIFY memoryUsageChanged)

public:
    explicit MemoryMonitor(QObject *parent = nullptr) : QObject(parent), m_memoryUsage(0) {
        //qDebug() << "[MemoryMonitor] 객체 생성됨! 초기 메모리 사용량:" << m_memoryUsage;

        connect(&timer, &QTimer::timeout, this, &MemoryMonitor::updateMemoryUsage);
        timer.start(5000); // 🔥 5초마다 업데이트
        //qDebug() << "[MemoryMonitor] QTimer 시작됨! 5초마다 updateMemoryUsage() 호출";

        // 프로그램 실행 후 즉시 한 번 업데이트 실행
        QTimer::singleShot(0, this, &MemoryMonitor::updateMemoryUsage);
    }

    int memoryUsage() const {
        return m_memoryUsage;
    }

public slots:
    void updateMemoryUsage() {
        //qDebug() << "[MemoryMonitor] updateMemoryUsage() 호출됨!";

#ifdef Q_OS_WIN
        //qDebug() << "[MemoryMonitor] Windows 환경에서 실행 중, GlobalMemoryStatusEx() 호출 시도";
        MEMORYSTATUSEX memStatus;
        memStatus.dwLength = sizeof(MEMORYSTATUSEX);
        if (GlobalMemoryStatusEx(&memStatus)) {
            m_memoryUsage = static_cast<int>(memStatus.dwMemoryLoad);  // 전체 메모리 사용량 %
            //qDebug() << "[MemoryMonitor] 메모리 사용량 업데이트됨:" << m_memoryUsage << "%";
            emit memoryUsageChanged();
        } else {
            //qDebug() << "[MemoryMonitor] 메모리 정보를 가져오는 데 실패!";
        }

#elif defined(Q_OS_LINUX)
        qDebug() << "[MemoryMonitor] Linux 환경에서 실행 중, /proc/meminfo 읽기 시도";
        QFile file("/proc/meminfo");
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            QString line;
            int totalMemory = 0, freeMemory = 0, availableMemory = 0;

            while (!in.atEnd()) {
                line = in.readLine();
                if (line.startsWith("MemTotal:")) {
                    totalMemory = line.split(QRegExp("\\s+"))[1].toInt();
                } else if (line.startsWith("MemAvailable:")) {
                    availableMemory = line.split(QRegExp("\\s+"))[1].toInt();
                }
                if (totalMemory > 0 && availableMemory > 0) {
                    break;
                }
            }
            file.close();

            if (totalMemory > 0) {
                m_memoryUsage = 100 - (availableMemory * 100 / totalMemory); // 메모리 사용률 %
                qDebug() << "[MemoryMonitor] 메모리 사용량 업데이트됨:" << m_memoryUsage << "%";
                emit memoryUsageChanged();
            } else {
                qDebug() << "[MemoryMonitor] /proc/meminfo에서 메모리 정보를 읽는 데 실패!";
            }
        }
#endif
    }

signals:
    void memoryUsageChanged();

private:
    int m_memoryUsage;
    QTimer timer;
};

#endif // MEMORYMONITOR_H
