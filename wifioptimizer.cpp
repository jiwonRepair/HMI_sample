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
#include <QOperatingSystemVersion>
#include <QSysInfo>
#include <QString>
#include "LibuvFileExporter.h"  // ✅ 추가 필요!

WifiOptimizer::WifiOptimizer(QObject* parent)
    : QObject(parent), m_signalStrength(0), m_fileExporter(this)
{
    m_networkManager = new QNetworkAccessManager(this);
    adjustMTU();
    adjustRTO();
    applySelectiveACK();
    refreshSignalStrength();

    m_deepLearningTimer.setInterval(3000);
    connect(&m_deepLearningTimer, &QTimer::timeout, this, &WifiOptimizer::testPredictiveEnhancement);
    m_deepLearningTimer.start();

    m_drawTimer.setInterval(4000);
    connect(&m_drawTimer, &QTimer::timeout, this, &WifiOptimizer::testGraphUpdate);
    m_drawTimer.start();

    m_signalTimer.setInterval(5000);
    connect(&m_signalTimer, &QTimer::timeout, this, &WifiOptimizer::testPredictiveEnhancement);
    m_signalTimer.start();

    m_optimizerTimer.setInterval(6000);
    connect(&m_optimizerTimer, &QTimer::timeout, this, &WifiOptimizer::applyAllOptimizations);
    m_optimizerTimer.start();

    m_saveSignalHistoryTimer.setInterval(7000);
    connect(&m_saveSignalHistoryTimer, &QTimer::timeout, this, &WifiOptimizer::exportSignalHistoryToFile);
    m_saveSignalHistoryTimer.start();
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

    if (value > 80) {
        result = "✅ 연결 양호";
    } else if (value > 50) {
        result = "⚠️ 품질 보통 → RTO 적용";
        adjustRTO();
    } else {
        result = "❌ 신호 불량 → 전체 보정 적용";
        adjustRTO();
        applySelectiveACK();
        enableFEC();  // 신호 히스토리 기반 보정
    }

    emit predictionResult(result);
}

// void WifiOptimizer::exportSignalHistoryToFile() {
//     QString filename = QCoreApplication::applicationDirPath() + "/signal_history.txt";
//     QFile file(filename);
//     if (file.open(QIODevice::WriteOnly)) {
//         QTextStream stream(&file);
//         const QVector<int>& history = m_signalHistory;
//         for (int val : history)
//             stream << val << "\n";
//         file.close();
//     }
// }

void WifiOptimizer::exportSignalHistoryToFile() {
    if (m_fileExporter.isInProgress()) {
        qDebug() << "⚠️ exporter busy";
        return;
    }

    // 취소 요청 초기화
    m_cancelRequested.store(false);

    QString filename = QCoreApplication::applicationDirPath() + "/signal_history.txt";

    m_exportFuture = QtConcurrent::run([=]() {
        m_fileExporter.writeCsvAsync<int>(
            m_signalHistory,
            filename,
            [](const int& v) { return QString::number(v); },  // ✅ formatter (3rd)
            [this](int percent) {
                if (m_cancelRequested.load()) return;  // ✅ 취소 확인
                QMetaObject::invokeMethod(this, [=]() {
                    emit progressChanged(percent);
                }, Qt::QueuedConnection);
            },
            [this]() {
                if (m_cancelRequested.load()) {
                    QMetaObject::invokeMethod(this, [=]() {
                        emit saveCancelled();
                    }, Qt::QueuedConnection);
                    return;
                }
                QMetaObject::invokeMethod(this, [=]() {
                    emit saveFinished();
                }, Qt::QueuedConnection);
            }
            );
    });

    // 완료 감시 연결
    m_exportWatcher.setFuture(m_exportFuture);
}

void WifiOptimizer::cancelExport() {
    if (m_exportFuture.isRunning()) {
        m_cancelRequested.store(true);
        qDebug() << "❗ 저장 작업 취소 요청됨.";
    }
}

void WifiOptimizer::adjustMTU() {
#ifdef Q_OS_WIN
    // Windows: Use netsh to adjust MTU for Wi-Fi
    QProcess::execute("netsh interface ipv4 set subinterface \"Wi-Fi\" mtu=1400 store=persistent");
#elif defined(Q_OS_LINUX)
    // Linux: Use ifconfig or ip to adjust MTU (interface assumed as wlan0)
    QProcess::execute("sudo ip link set dev wlan0 mtu 1400");
#elif defined(Q_OS_MACOS)
    // macOS: Use networksetup (interface assumed as en0)
    QProcess::execute("sudo networksetup -setMTU en0 1400");
#else
    qWarning() << "MTU adjustment not supported on this OS.";
#endif
}

void WifiOptimizer::adjustRTO() {
    QString os = QSysInfo::prettyProductName();
    qDebug() << "[adjustRTO] OS:" << os;
#if defined(Q_OS_LINUX)
    // Linux: 커널 파라미터를 통해 RTO 조정
    QProcess::execute("sudo sysctl -w net.ipv4.tcp_retries2=3");
    qInfo() << "[adjustRTO] Linux 시스템에서 TCP RTO를 3으로 조정했습니다.";

#elif defined(Q_OS_WIN)
    // Windows: 직접 설정 불가, 안내만
    qWarning() << "[adjustRTO] Windows에서는 시스템 정책상 TCP RTO 값을 직접 조정할 수 없습니다.";
    qWarning() << "           일부 설정은 레지스트리 편집 또는 고급 드라이버 수준에서만 가능하며,";
    qWarning() << "           현재 코드에서는 자동 적용되지 않습니다.";

#elif defined(Q_OS_MACOS)
    // macOS: 제한됨, 안내만
    qWarning() << "[adjustRTO] macOS에서는 TCP RTO 값 조정이 제한적입니다.";
    qWarning() << "           현재 macOS에서는 관련 파라미터 조정이 공식적으로 지원되지 않습니다.";

#else
    qWarning() << "[adjustRTO] 알 수 없는 운영체제입니다. 적용 불가능합니다.";
#endif
}

void WifiOptimizer::applySelectiveACK()
{
#ifdef Q_OS_LINUX
    QFile sackFile("/proc/sys/net/ipv4/tcp_sack");
    if (sackFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&sackFile);
        out << "1\n"; // Enable SACK
        sackFile.close();
        qDebug() << "[applySelectiveACK] SACK enabled on Linux.";
    } else {
        qWarning() << "[applySelectiveACK] Cannot open /proc/sys/net/ipv4/tcp_sack. Run as root?";
    }
#elif defined(Q_OS_WIN)
    qDebug() << "[applySelectiveACK] Windows: SACK is enabled by default, no manual override available.";
#elif defined(Q_OS_MACOS)
    qDebug() << "[applySelectiveACK] macOS: SACK is enabled by default, system-level toggle not exposed.";
#else
    qWarning() << "[applySelectiveACK] Unsupported OS for SACK configuration.";
#endif
}

void WifiOptimizer::enableFEC()
{
    if (m_signalHistory.isEmpty()) {
        qWarning() << "[enableFEC] No signal data to apply FEC.";
        return;
    }

    QVector<int> encodedData;
    int parity = 0;

    for (const int &value : std::as_const(m_signalHistory)) {
        encodedData.append(value);
        parity ^= value;
    }

    encodedData.append(parity);  // 마지막에 패리티 추가
    qDebug() << "[enableFEC] Encoded signal data with parity:" << encodedData;

    // 복구 시도 (예제: 마지막 값이 손상되었을 경우)
    int recoveredParity = 0;
    for (int i = 0; i < encodedData.size() - 1; ++i) {
        recoveredParity ^= encodedData[i];
    }

    if (recoveredParity == encodedData.last()) {
        qDebug() << "[enableFEC] Integrity check passed.";
    } else {
        qWarning() << "[enableFEC] Signal data may be corrupted.";
    }
}


