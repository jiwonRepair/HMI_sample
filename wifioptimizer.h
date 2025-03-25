#ifndef WIFI_OPTIMIZER_H
#define WIFI_OPTIMIZER_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QTimer>

class WifiOptimizer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString ssid READ ssid NOTIFY wifiStatusChanged)
    Q_PROPERTY(int signalStrength READ signalStrength NOTIFY wifiStatusChanged)
    Q_PROPERTY(QVector<int> signalHistory READ signalHistory NOTIFY graphDataUpdated)

public:
    explicit WifiOptimizer(QObject* parent = nullptr);

    QString ssid() const { return m_ssid; }
    int signalStrength() const { return m_signalStrength; }
    QVector<int> signalHistory() const { return m_signalHistory; }

    Q_INVOKABLE void refreshSignalStrength();
    Q_INVOKABLE void applyAllOptimizations();
    Q_INVOKABLE void testGraphUpdate();
    Q_INVOKABLE void testFastApiIntegration();
    Q_INVOKABLE void testPredictiveEnhancement();
    Q_INVOKABLE void exportSignalHistoryToFile();

signals:
    void wifiStatusChanged();
    void graphDataUpdated();
    void predictionResult(const QString& message);

private:
    void updateSignalStrength();
    void updateSignalHistory();
    void evaluatePrediction(float value);
    float runOnnxPrediction();  // 현재는 평균 기반 가상 예측
    void sendToFastApi();

    QString m_ssid;
    int m_signalStrength;
    QVector<int> m_signalHistory;

    QNetworkAccessManager* m_networkManager;
};

#endif // WIFI_OPTIMIZER_H
