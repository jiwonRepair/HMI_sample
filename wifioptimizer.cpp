#include "wifioptimizer.h"
#include <QProcess>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QRandomGenerator>
#include <QCoreApplication>
#include <algorithm>
#include <numeric>
#include <QRegularExpression>

WifiOptimizer::WifiOptimizer(QObject* parent)
    : QObject(parent), m_signalStrength(0)
{
    m_networkManager = new QNetworkAccessManager(this);
    refreshSignalStrength();
}

void WifiOptimizer::refreshSignalStrength() {
    updateSignalStrength();
    updateSignalHistory();
    emit wifiStatusChanged();
    emit graphDataUpdated();
}

void WifiOptimizer::updateSignalStrength() {
#ifdef Q_OS_WIN
    QProcess process;
    process.start("netsh wlan show interfaces");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();

#ifdef Q_OS_WIN
    static const QRegularExpression ssidRx("SSID\\s+:\\s(.+)");
    static const QRegularExpression signalRx("Signal\\s+:\\s(\\d+)");
#elif defined(Q_OS_LINUX)
    static const QRegularExpression ssidRx("ESSID:\"(.+)\"");
    static const QRegularExpression signalRx("Signal level=(-\\d+)");
#elif defined(Q_OS_MACOS)
    static const QRegularExpression ssidRx(" SSID: (.+)");
    static const QRegularExpression signalRx("agrCtlRSSI: (-\\d+)");
#endif

    QRegularExpressionMatch match;
    match = ssidRx.match(output);
    if (match.hasMatch()) m_ssid = match.captured(1).trimmed();

    match = signalRx.match(output);
    if (match.hasMatch()) m_signalStrength = match.captured(1).toInt();
#elif defined(Q_OS_LINUX)
    QProcess process;
    process.start("iwconfig");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();

    QRegularExpression ssidRx("ESSID:\"(.+)\"");
    QRegularExpression signalRx("Signal level=(-\\d+)");

    QRegularExpressionMatch match;
    match = ssidRx.match(output);
    if (match.hasMatch()) m_ssid = match.captured(1).trimmed();

    match = signalRx.match(output);
    if (match.hasMatch()) m_signalStrength = 100 + match.captured(1).toInt();
#elif defined(Q_OS_MACOS)
    QProcess process;
    process.start("/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -I");
    process.waitForFinished();
    QString output = process.readAllStandardOutput();

    QRegularExpression ssidRx(" SSID: (.+)");
    QRegularExpression signalRx("agrCtlRSSI: (-\\d+)");

    QRegularExpressionMatch match;
    match = ssidRx.match(output);
    if (match.hasMatch()) m_ssid = match.captured(1).trimmed();

    match = signalRx.match(output);
    if (match.hasMatch()) m_signalStrength = 100 + match.captured(1).toInt();
#endif
}

void WifiOptimizer::updateSignalHistory() {
    m_signalHistory.append(m_signalStrength);
    if (m_signalHistory.size() > 30)
        m_signalHistory.removeFirst();
    emit graphDataUpdated();
}

void WifiOptimizer::applyAllOptimizations() {
    refreshSignalStrength();
    float prediction = runOnnxPrediction();
    evaluatePrediction(prediction);
}

void WifiOptimizer::testGraphUpdate() {
    m_signalStrength = QRandomGenerator::global()->bounded(60, 100);
    updateSignalHistory();
    emit wifiStatusChanged();
}

void WifiOptimizer::testFastApiIntegration() {
    sendToFastApi();
}

void WifiOptimizer::sendToFastApi() {
    QJsonObject obj;
    obj["ssid"] = m_ssid;
    obj["strength"] = m_signalStrength;

    QNetworkRequest request(QUrl("http://127.0.0.1:8000/wifi/update"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* reply = m_networkManager->post(request, QJsonDocument(obj).toJson());

    connect(reply, &QNetworkReply::finished, this, [reply]() {
        qDebug() << "FastAPI response:" << reply->readAll();
        reply->deleteLater();
    });
}

void WifiOptimizer::testPredictiveEnhancement() {
    float value = runOnnxPrediction();
    evaluatePrediction(value);
}

float WifiOptimizer::runOnnxPrediction() {
    if (m_signalHistory.size() < 10)
        return 0.0f;

    int start = std::max(0, static_cast<int>(m_signalHistory.size() - 10));
    QVector<int> last10 = m_signalHistory.mid(start, 10);

    float sum = std::accumulate(last10.begin(), last10.end(), 0);
    return sum / 10.0f;
}

void WifiOptimizer::evaluatePrediction(float value) {
    QString result;
    if (value > 80)
        result = "✅ 연결 양호";
    else if (value > 50)
        result = "⚠️ 품질 보통";
    else
        result = "❌ 신호 불량";

    emit predictionResult(result);
}

void WifiOptimizer::exportSignalHistoryToFile() {
    QString filename = QCoreApplication::applicationDirPath() + "/signal_history.txt";
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        const QVector<int>& history = m_signalHistory;
        for (int val : history)
            stream << val << "\n";
        file.close();
    }
}

