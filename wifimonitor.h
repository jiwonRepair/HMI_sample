#ifndef WIFIMONITOR_H
#define WIFIMONITOR_H
#ifdef _WIN32
#include <winsock2.h>  // 반드시 windows.h 보다 먼저!
#endif

#include <QObject>
#include <windows.h>
#include <wlanapi.h>
#include <QTimer>

#pragma comment(lib, "Wlanapi.lib")

class WifiMonitor : public QObject {
    Q_OBJECT
    Q_PROPERTY(int signalStrength READ signalStrength NOTIFY signalStrengthChanged)

public:
    explicit WifiMonitor(QObject *parent = nullptr);
    ~WifiMonitor();

    int signalStrength() const { return m_signalStrength; }

signals:
    void signalStrengthChanged();

public slots:
    void updateSignalStrength();

private:
    QTimer *timer;
    int m_signalStrength = 0;
    HANDLE wlanHandle = nullptr;
};

#endif // WIFIMONITOR_H
