#ifndef BATTERYMONITOR_H
#define BATTERYMONITOR_H

#include <QObject>
#include <QDebug>
#include <QTimer>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

class BatteryMonitor : public QObject {
    Q_OBJECT
    Q_PROPERTY(int batteryLevel READ batteryLevel NOTIFY batteryLevelChanged)

public:
    explicit BatteryMonitor(QObject *parent = nullptr) : QObject(parent), m_batteryLevel(100) {
        //qDebug() << "[BatteryMonitor] 객체 생성됨! 초기 배터리 값:" << m_batteryLevel;

        connect(&timer, &QTimer::timeout, this, &BatteryMonitor::updateBatteryLevel);
        timer.start(5000); // 🔥 5초마다 업데이트
        //qDebug() << "[BatteryMonitor] QTimer 시작됨! 5초마다 updateBatteryLevel() 호출";

        // 프로그램 실행 후 즉시 한 번 업데이트 실행
        QTimer::singleShot(0, this, &BatteryMonitor::updateBatteryLevel);
    }

    int batteryLevel() const {
        return m_batteryLevel;
    }

public slots:
    void updateBatteryLevel() {
        //qDebug() << "[BatteryMonitor] updateBatteryLevel() 호출됨!";

#ifdef Q_OS_WIN
        //qDebug() << "[BatteryMonitor] Windows 환경에서 실행 중, GetSystemPowerStatus() 호출 시도";
        SYSTEM_POWER_STATUS status;
        if (GetSystemPowerStatus(&status)) {
            m_batteryLevel = status.BatteryLifePercent;
            //qDebug() << "[BatteryMonitor] 배터리 값 업데이트됨:" << m_batteryLevel << "%";
            emit batteryLevelChanged();
        } else {
            //qDebug() << "[BatteryMonitor] 배터리 정보를 가져오는 데 실패!";
        }
#else
        qDebug() << "[BatteryMonitor] Linux 환경에서 실행 중";
#endif
    }

signals:
    void batteryLevelChanged();

private:
    int m_batteryLevel;
    QTimer timer;
};

#endif // BATTERYMONITOR_H
