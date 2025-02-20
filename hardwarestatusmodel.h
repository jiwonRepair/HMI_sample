#ifndef HARDWARESTATUSMODEL_H
#define HARDWARESTATUSMODEL_H

#include <QAbstractListModel>
#include <QVector>

struct HardwareDevice {
    QString name;
    QString type;
    bool isConnected;
    QString instanceId;  // ✅ Instance ID 추가
};

class HardwareStatusModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    enum DeviceRoles {
        NameRole = Qt::UserRole + 1,
        TypeRole,
        ConnectionRole,
        InstanceIdRole  // ✅ Instance ID 역할 추가
    };

    explicit HardwareStatusModel(QObject *parent = nullptr);
    void addDevice(const QString &name, const QString &type, bool isConnected, const QString &instanceId);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void countChanged();

private:
    QVector<HardwareDevice> m_devices;
    void setupHardwareDevices();
};

#endif // HARDWARESTATUSMODEL_H
