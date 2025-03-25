#ifndef WIFIOPTIMIZER_H
#define WIFIOPTIMIZER_H

#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <array>

// ONNX Runtime C API
#include <onnxruntime_c_api.h>

class WifiOptimizer : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString ssid READ ssid NOTIFY wifiStatusChanged)
    Q_PROPERTY(int signalStrength READ signalStrength NOTIFY wifiStatusChanged)
    Q_PROPERTY(QVector<int> signalHistory READ signalHistory NOTIFY graphDataUpdated)
public:
    explicit WifiOptimizer(QObject* parent = nullptr);

    Q_INVOKABLE void refreshSignalStrength();
    Q_INVOKABLE void applyAllOptimizations();
    Q_INVOKABLE void testGraphUpdate();
    Q_INVOKABLE void testFastApiIntegration();
    Q_INVOKABLE void testPredictiveEnhancement();
    Q_INVOKABLE void exportSignalHistoryToFile();

    QString ssid() const { return m_ssid; }
    int signalStrength() const { return m_signalStrength; }
    QVector<int> signalHistory() const { return m_signalHistory; }

signals:
    void wifiStatusChanged();
    void graphDataUpdated();
    void predictionResult(const QString& message);

private:
    QString detectOS() const;
    void updateSignalStrength();
    void sendToFastApi();
    float runOnnxPrediction();
    void evaluatePrediction(float prediction);

    QString m_ssid;
    int m_signalStrength = -1;
    QVector<int> m_signalHistory;

    QNetworkAccessManager* m_netManager;

    // ONNX Runtime C API pointers
    const OrtApi* g_ort = nullptr;
    OrtEnv* m_env = nullptr;
};


#endif // WIFIOPTIMIZER_H
