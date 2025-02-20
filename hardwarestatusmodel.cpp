// #include "hardwarestatusmodel.h"
// #include <QDebug>
// #include <windows.h>

// #ifdef Q_OS_WIN
// #include <setupapi.h>
// #include <bluetoothapis.h>
// #include <wlanapi.h>
// #include <cfgmgr32.h>  // ✅ 추가 (CONFIGFLAG_REMOVED 정의됨)
// #pragma comment(lib, "Cfgmgr32.lib")
// #pragma comment(lib, "Setupapi.lib")
// #pragma comment(lib, "Bthprops.lib")
// #pragma comment(lib, "Wlanapi.lib")
// #endif

// #ifndef CONFIGFLAG_REMOVED
// #define CONFIGFLAG_REMOVED 0x00000001  // ✅ 제거된 장치 플래그 수동 정의
// #endif


// HardwareStatusModel::HardwareStatusModel(QObject *parent)
//     : QAbstractListModel(parent) {
// #ifdef Q_OS_WIN
//     setupHardwareDevices();  // ✅ Windows에서만 실행
// #endif
// }

// #ifdef Q_OS_WIN
// void HardwareStatusModel::setupHardwareDevices() {
//     qDebug() << "[INFO] Scanning hardware devices...";

//     // // ✅ 1. USB 장치 검색
//     // HDEVINFO deviceInfoSet = SetupDiGetClassDevsA(nullptr, nullptr, nullptr, DIGCF_PRESENT | DIGCF_ALLCLASSES);
//     // if (deviceInfoSet == INVALID_HANDLE_VALUE) {
//     //     qDebug() << "[ERROR] Unable to retrieve USB device info.";
//     //     return;
//     // }

//     // SP_DEVINFO_DATA deviceInfoData;
//     // deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
//     // DWORD index = 0;
//     // while (SetupDiEnumDeviceInfo(deviceInfoSet, index++, &deviceInfoData)) {
//     //     char deviceName[256] = {0};
//     //     SetupDiGetDeviceInstanceIdA(deviceInfoSet, &deviceInfoData, deviceName, sizeof(deviceName), nullptr);
//     //     addDevice(QString(deviceName), "USB", true);
//     // }
//     // SetupDiDestroyDeviceInfoList(deviceInfoSet);

//     // // ✅ 1. USB 장치 검색 (친화적인 장치 이름 가져오기)
//     // HDEVINFO deviceInfoSet = SetupDiGetClassDevsA(nullptr, nullptr, nullptr, DIGCF_PRESENT | DIGCF_ALLCLASSES);
//     // if (deviceInfoSet != INVALID_HANDLE_VALUE) {
//     //     SP_DEVINFO_DATA deviceInfoData;
//     //     deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
//     //     DWORD index = 0;
//     //     while (SetupDiEnumDeviceInfo(deviceInfoSet, index++, &deviceInfoData)) {
//     //         char deviceName[256] = {0};
//     //         DWORD size = 0;
//     //         if (SetupDiGetDeviceRegistryPropertyA(deviceInfoSet, &deviceInfoData, SPDRP_FRIENDLYNAME, nullptr, (PBYTE)deviceName, sizeof(deviceName), &size)) {
//     //             addDevice(QString(deviceName), "USB", true);
//     //         } else {
//     //             // 기본 장치 ID라도 추가
//     //             SetupDiGetDeviceInstanceIdA(deviceInfoSet, &deviceInfoData, deviceName, sizeof(deviceName), nullptr);
//     //             addDevice(QString(deviceName), "USB", true);
//     //         }
//     //     }
//     //     SetupDiDestroyDeviceInfoList(deviceInfoSet);
//     // } else {
//     //     qDebug() << "[ERROR] Unable to retrieve USB device info.";
//     // }

//     // ✅ 1. USB 장치 검색 (현재 연결된 장치만)
//     HDEVINFO deviceInfoSet = SetupDiGetClassDevsA(nullptr, "USB", nullptr, DIGCF_PRESENT | DIGCF_ALLCLASSES);
//     if (deviceInfoSet == INVALID_HANDLE_VALUE) {
//         qDebug() << "[ERROR] Unable to retrieve USB device info.";
//         return;
//     }

//     SP_DEVINFO_DATA deviceInfoData;
//     deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
//     DWORD index = 0;
//     while (SetupDiEnumDeviceInfo(deviceInfoSet, index++, &deviceInfoData)) {
//         char deviceName[256] = {0};
//         DWORD size = 0;
//         DWORD configFlags = 0;

//         // ✅ 현재 장치가 실제로 연결된 상태인지 확인
//         if (SetupDiGetDeviceRegistryPropertyA(deviceInfoSet, &deviceInfoData, SPDRP_CONFIGFLAGS, nullptr, (PBYTE)&configFlags, sizeof(configFlags), nullptr)) {
//             if (configFlags & CONFIGFLAG_REMOVED) {
//                 continue;  // ❌ 제거된 장치는 무시
//             }
//         }

//         // ✅ 장치의 친화적인 이름 가져오기
//         if (SetupDiGetDeviceRegistryPropertyA(deviceInfoSet, &deviceInfoData, SPDRP_FRIENDLYNAME, nullptr, (PBYTE)deviceName, sizeof(deviceName), &size)) {
//             addDevice(QString(deviceName), "USB", true);
//         } else {
//             SetupDiGetDeviceInstanceIdA(deviceInfoSet, &deviceInfoData, deviceName, sizeof(deviceName), nullptr);
//             addDevice(QString(deviceName), "USB", true);
//         }
//     }
//     SetupDiDestroyDeviceInfoList(deviceInfoSet);


//     // // ✅ 2. Bluetooth 장치 검색
//     // BLUETOOTH_DEVICE_SEARCH_PARAMS btSearchParams = { sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS) };
//     // BLUETOOTH_DEVICE_INFO btDeviceInfo = { sizeof(BLUETOOTH_DEVICE_INFO) };
//     // btSearchParams.fReturnAuthenticated = TRUE;
//     // btSearchParams.fReturnRemembered = TRUE;
//     // btSearchParams.fReturnConnected = TRUE;
//     // btSearchParams.fReturnUnknown = TRUE;
//     // btSearchParams.hRadio = nullptr;

//     // HBLUETOOTH_DEVICE_FIND btDeviceFind = BluetoothFindFirstDevice(&btSearchParams, &btDeviceInfo);
//     // if (btDeviceFind) {
//     //     do {
//     //         addDevice(QString::fromWCharArray(btDeviceInfo.szName), "Bluetooth", true);
//     //     } while (BluetoothFindNextDevice(btDeviceFind, &btDeviceInfo));
//     //     BluetoothFindDeviceClose(btDeviceFind);
//     // }

//     // // ✅ 3. WiFi 장치 검색
//     // HANDLE hClient = nullptr;
//     // DWORD negotiatedVersion;
//     // if (WlanOpenHandle(2, nullptr, &negotiatedVersion, &hClient) == ERROR_SUCCESS) {
//     //     PWLAN_INTERFACE_INFO_LIST pInterfaceList = nullptr;
//     //     if (WlanEnumInterfaces(hClient, nullptr, &pInterfaceList) == ERROR_SUCCESS) {
//     //         for (DWORD i = 0; i < pInterfaceList->dwNumberOfItems; i++) {
//     //             PWLAN_INTERFACE_INFO pInterface = &pInterfaceList->InterfaceInfo[i];
//     //             addDevice(QString::fromWCharArray(pInterface->strInterfaceDescription), "WiFi", true);
//     //         }
//     //         WlanFreeMemory(pInterfaceList);
//     //     }
//     //     WlanCloseHandle(hClient, nullptr);
//     // }

//     qDebug() << "[INFO] Hardware scanning complete!";
// }
// #endif

// void HardwareStatusModel::addDevice(const QString &name, const QString &type, bool isConnected) {
//     beginInsertRows(QModelIndex(), m_devices.size(), m_devices.size());
//     m_devices.append({name, type, isConnected});
//     endInsertRows();
//     emit countChanged();
// }

// int HardwareStatusModel::rowCount(const QModelIndex &parent) const {
//     Q_UNUSED(parent);
//     return m_devices.size();
// }

// QVariant HardwareStatusModel::data(const QModelIndex &index, int role) const {
//     if (!index.isValid() || index.row() >= m_devices.size()) {
//         return QVariant();
//     }
//     const HardwareDevice &device = m_devices[index.row()];
//     switch (role) {
//     case NameRole: return QVariant(device.name);
//     case TypeRole: return QVariant(device.type);
//     case ConnectionRole: return QVariant(device.isConnected);
//     default: return QVariant();
//     }
// }

// QHash<int, QByteArray> HardwareStatusModel::roleNames() const {
//     QHash<int, QByteArray> roles;
//     roles[NameRole] = "deviceName";
//     roles[TypeRole] = "deviceType";
//     roles[ConnectionRole] = "isConnected";
//     return roles;
// }

// void HardwareStatusModel::updateConnectionStatus(const QString &name, bool isConnected) {
//     for (int i = 0; i < m_devices.size(); ++i) {
//         if (m_devices[i].name == name) {
//             m_devices[i].isConnected = isConnected;
//             QModelIndex index = createIndex(i, 0);
//             emit dataChanged(index, index);
//             return;
//         }
//     }
// }

#include "hardwarestatusmodel.h"
#include <QDebug>
#include <windows.h>

#ifdef Q_OS_WIN
#include <setupapi.h>
#include <cfgmgr32.h>
#pragma comment(lib, "Cfgmgr32.lib")
#pragma comment(lib, "Setupapi.lib")
#endif

#ifndef CONFIGFLAG_REMOVED
#define CONFIGFLAG_REMOVED 0x00000001  // ✅ MinGW에서 정의되지 않은 경우 수동으로 정의
#endif

HardwareStatusModel::HardwareStatusModel(QObject *parent)
    : QAbstractListModel(parent) {
#ifdef Q_OS_WIN
    setupHardwareDevices();  // ✅ Windows에서만 실행
#endif
}

#ifdef Q_OS_WIN
void HardwareStatusModel::setupHardwareDevices() {
    qDebug() << "[INFO] Scanning connected USB devices...";

    // ✅ 1. USB 장치 검색 (현재 연결된 장치만)
    HDEVINFO deviceInfoSet = SetupDiGetClassDevsA(nullptr, "USB", nullptr, DIGCF_PRESENT | DIGCF_ALLCLASSES);
    if (deviceInfoSet == INVALID_HANDLE_VALUE) {
        qDebug() << "[ERROR] Unable to retrieve USB device info.";
        return;
    }

    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    DWORD index = 0;

    while (SetupDiEnumDeviceInfo(deviceInfoSet, index++, &deviceInfoData)) {
        char friendlyName[256] = {0};
        char instanceId[256] = {0};
        DWORD size = 0;
        DWORD configFlags = 0;

        // ✅ 현재 장치가 실제로 연결된 상태인지 확인
        if (SetupDiGetDeviceRegistryPropertyA(deviceInfoSet, &deviceInfoData, SPDRP_CONFIGFLAGS, nullptr, (PBYTE)&configFlags, sizeof(configFlags), nullptr)) {
            if (configFlags & CONFIGFLAG_REMOVED) {
                qDebug() << "[INFO] Skipping removed device.";
                continue;  // ❌ 제거된 장치는 무시
            }
        }

        // ✅ 장치의 Instance ID 가져오기 (PowerShell과 동일한 정보)
        if (!SetupDiGetDeviceInstanceIdA(deviceInfoSet, &deviceInfoData, instanceId, sizeof(instanceId), nullptr)) {
            qDebug() << "[WARNING] Could not retrieve Instance ID.";
            continue;
        }

        // ✅ 장치의 친화적인 이름 가져오기
        if (!SetupDiGetDeviceRegistryPropertyA(deviceInfoSet, &deviceInfoData, SPDRP_FRIENDLYNAME, nullptr, (PBYTE)friendlyName, sizeof(friendlyName), &size)) {
            strcpy_s(friendlyName, "USB 장치");  // ✅ 이름이 없는 경우 기본값 설정
        }

        qDebug() << "[INFO] Found USB Device: " << friendlyName << " (" << instanceId << ")";
        addDevice(QString(friendlyName), "USB", true, QString(instanceId));
    }

    SetupDiDestroyDeviceInfoList(deviceInfoSet);
}
#endif

void HardwareStatusModel::addDevice(const QString &name, const QString &type, bool isConnected, const QString &instanceId) {
    beginInsertRows(QModelIndex(), m_devices.size(), m_devices.size());
    m_devices.append({name, type, isConnected, instanceId});
    endInsertRows();
    emit countChanged();
}

int HardwareStatusModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_devices.size();
}

QVariant HardwareStatusModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_devices.size()) {
        return QVariant();
    }
    const HardwareDevice &device = m_devices[index.row()];
    switch (role) {
    case NameRole: return QVariant(device.name);
    case TypeRole: return QVariant(device.type);
    case ConnectionRole: return QVariant(device.isConnected);
    case InstanceIdRole: return QVariant(device.instanceId);  // ✅ Instance ID 추가
    default: return QVariant();
    }
}

QHash<int, QByteArray> HardwareStatusModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "deviceName";
    roles[TypeRole] = "deviceType";
    roles[ConnectionRole] = "isConnected";
    roles[InstanceIdRole] = "instanceId";  // ✅ QML에서 사용 가능하도록 등록
    return roles;
}
