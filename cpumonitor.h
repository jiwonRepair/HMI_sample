#ifndef CPUMONITOR_H
#define CPUMONITOR_H

#ifdef _WIN32
#include <winsock2.h>  // 반드시 windows.h 보다 먼저!
#endif

#include <QObject>
#include <QTimer>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/sysinfo.h>
#include <unistd.h>
#endif



class CpuMonitor : public QObject {
    Q_OBJECT
    Q_PROPERTY(double cpuUsage READ getCpuUsage NOTIFY cpuUsageChanged)

public:
    explicit CpuMonitor(QObject *parent = nullptr) : QObject(parent), cpuUsage(0.0) {
        QTimer *timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &CpuMonitor::updateCpuUsage);
        timer->start(1000);  // 1초마다 업데이트
    }

    double getCpuUsage() const {
        return cpuUsage;
    }

signals:
    void cpuUsageChanged();

private:
    double cpuUsage;

    void updateCpuUsage() {
        double usage = getSystemCpuUsage();
        if (usage != cpuUsage) {
            cpuUsage = usage;
            //std::cout << "[CpuMonitor] Updated CPU usage: " << cpuUsage << "%" << std::endl;
            emit cpuUsageChanged();  // 🔥 QML에 변경 사항 알림
        }
    }

    double getSystemCpuUsage() {
#ifdef _WIN32
        static FILETIME prevIdleTime, prevKernelTime, prevUserTime;
        FILETIME idleTime, kernelTime, userTime;

        if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
            ULONGLONG idleDiff = (reinterpret_cast<ULONGLONG&>(idleTime) - reinterpret_cast<ULONGLONG&>(prevIdleTime));
            ULONGLONG kernelDiff = (reinterpret_cast<ULONGLONG&>(kernelTime) - reinterpret_cast<ULONGLONG&>(prevKernelTime));
            ULONGLONG userDiff = (reinterpret_cast<ULONGLONG&>(userTime) - reinterpret_cast<ULONGLONG&>(prevUserTime));

            prevIdleTime = idleTime;
            prevKernelTime = kernelTime;
            prevUserTime = userTime;

            ULONGLONG totalDiff = kernelDiff + userDiff;
            if (totalDiff == 0) return 0.0;

            return 100.0 * (1.0 - (double)idleDiff / (double)totalDiff);
        }
        return 0.0;
#else
        static long lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;
        long totalUser, totalUserLow, totalSys, totalIdle;
        double percent;

        std::ifstream file("/proc/stat");
        if (!file.is_open()) {
            std::cerr << "[CpuMonitor] Failed to open /proc/stat" << std::endl;
            return 0.0;
        }

        file >> std::skipws >> std::string(); // "cpu" 스킵
        file >> totalUser >> totalUserLow >> totalSys >> totalIdle;
        file.close();

        if (lastTotalUser == 0) {
            lastTotalUser = totalUser;
            lastTotalUserLow = totalUserLow;
            lastTotalSys = totalSys;
            lastTotalIdle = totalIdle;
            return 0.0;
        }

        percent = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) + (totalSys - lastTotalSys);
        percent /= (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) + (totalSys - lastTotalSys) + (totalIdle - lastTotalIdle);
        percent *= 100.0;

        lastTotalUser = totalUser;
        lastTotalUserLow = totalUserLow;
        lastTotalSys = totalSys;
        lastTotalIdle = totalIdle;

        return percent;
#endif
    }
};
#endif //CPUMONITOR_H
