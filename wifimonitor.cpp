#include "wifimonitor.h"
#include <QtDebug>

WifiMonitor::WifiMonitor(QObject *parent) : QObject(parent) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &WifiMonitor::updateSignalStrength);
    timer->start(2000);  // 2초마다 업데이트

    // Windows WLAN API 초기화
    DWORD negotiatedVersion;
    if (WlanOpenHandle(2, NULL, &negotiatedVersion, &wlanHandle) != ERROR_SUCCESS) {
        wlanHandle = nullptr;
    }
}

WifiMonitor::~WifiMonitor() {
    if (wlanHandle) {
        WlanCloseHandle(wlanHandle, NULL);
    }
}

void WifiMonitor::updateSignalStrength() {
    if (!wlanHandle) {
        qWarning() << "WLAN Handle is not initialized!";
        return;
    }

    PWLAN_INTERFACE_INFO_LIST pIfList = nullptr;
    PWLAN_INTERFACE_INFO pIfInfo = nullptr;

    if (WlanEnumInterfaces(wlanHandle, NULL, &pIfList) != ERROR_SUCCESS) {
        return;
    }

    for (int i = 0; i < (int)pIfList->dwNumberOfItems; i++) {
        pIfInfo = (WLAN_INTERFACE_INFO*)&pIfList->InterfaceInfo[i];

        PWLAN_BSS_LIST pBssList = nullptr;
        if (WlanGetNetworkBssList(wlanHandle, &pIfInfo->InterfaceGuid, NULL, dot11_BSS_type_any, false, NULL, &pBssList) == ERROR_SUCCESS) {
            if (pBssList->dwNumberOfItems > 0) {
                m_signalStrength = (int)pBssList->wlanBssEntries[0].lRssi;
                m_signalStrength = (m_signalStrength + 100) * 2;  // dBm → 0~100 변환
                emit signalStrengthChanged();
                //qDebug("m_signalStrength : %d", m_signalStrength);
            }
            WlanFreeMemory(pBssList);
        }
    }

    if (pIfList) {
        WlanFreeMemory(pIfList);
    }
}
