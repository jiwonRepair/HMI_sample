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
