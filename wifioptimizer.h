#ifndef WIFI_OPTIMIZER_H
#define WIFI_OPTIMIZER_H

#ifdef _WIN32
#include <winsock2.h>  // 반드시 windows.h 보다 먼저!
#endif

#include <QObject>
#include <QVector>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>
#include <QTimer>
#include "LibuvFileExporter.h"
#include <QFuture>
#include <QFutureWatcher>

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

    void progressChanged(int percent);  // 진행률 알림
    void saveFinished();                // 저장 완료 알림

    void saveCancelled();

private:
    void updateSignalStrength();
    void updateSignalHistory();
    void evaluatePrediction(float value);
    float runOnnxPrediction();  // 현재는 평균 기반 가상 예측
    void sendToFastApi();
    void adjustMTU();
    void adjustRTO();
    void applySelectiveACK();
    void enableFEC();
    void cancelExport();

    QString m_ssid;
    int m_signalStrength;
    QVector<int> m_signalHistory;

    QNetworkAccessManager* m_networkManager;

    QTimer m_deepLearningTimer;
    QTimer m_drawTimer;
    QTimer m_signalTimer;
    QTimer m_optimizerTimer;
    QTimer m_saveSignalHistoryTimer;

    LibuvFileExporter m_fileExporter;  // ✅ 클래스 내부 전용

    QFuture<void> m_exportFuture;                  // ✅ 현재 실행 중인 저장
    QFutureWatcher<void> m_exportWatcher;          // ✅ 완료/취소 감시용
    std::atomic_bool m_cancelRequested = false;    // ✅ 중단 요청 flag
};

#endif // WIFI_OPTIMIZER_H
