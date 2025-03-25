#include "wifioptimizer.h"
#include <QDebug>

WifiOptimizer::WifiOptimizer(QObject* parent)
    : QObject(parent),
    m_netManager(new QNetworkAccessManager(this))
{
    g_ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);
    g_ort->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "wifi_cpu", &m_env);
    refreshSignalStrength();
}

QString WifiOptimizer::detectOS() const {
#ifdef Q_OS_WIN
    return "Windows";
#elif defined(Q_OS_LINUX)
    return "Linux";
#elif defined(Q_OS_MAC)
    return "macOS";
#else
    return "Unknown";
#endif
}

void WifiOptimizer::updateSignalStrength() {
    QProcess proc;
    QString os = detectOS();
    if (os == "Windows") {
        proc.start("netsh wlan show interfaces");
        proc.waitForFinished();
        QString output = proc.readAllStandardOutput();
        QRegExp ssidRx("SSID\\s+:\\s(.+)");
        QRegExp signalRx("Signal\\s+:\\s(\\d+)");
        if (ssidRx.indexIn(output) != -1)
            m_ssid = ssidRx.cap(1).trimmed();
        if (signalRx.indexIn(output) != -1)
            m_signalStrength = signalRx.cap(1).toInt();
    } else if (os == "Linux") {
        proc.start("iwconfig");
        proc.waitForFinished();
        QString output = proc.readAllStandardOutput();
        QRegExp ssidRx("ESSID:\"(.+)\"");
        QRegExp signalRx("Signal level=(-\\d+)");
        if (ssidRx.indexIn(output) != -1)
            m_ssid = ssidRx.cap(1).trimmed();
        if (signalRx.indexIn(output) != -1)
            m_signalStrength = 100 + signalRx.cap(1).toInt(); // Rough conversion
    } else if (os == "macOS") {
        proc.start("/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -I");
        proc.waitForFinished();
        QString output = proc.readAllStandardOutput();
        QRegExp ssidRx(" *SSID: (.+)");
        QRegExp signalRx(" *agrCtlRSSI: (-\\d+)");
        if (ssidRx.indexIn(output) != -1)
            m_ssid = ssidRx.cap(1).trimmed();
        if (signalRx.indexIn(output) != -1)
            m_signalStrength = 100 + signalRx.cap(1).toInt(); // dBm to %
    }

    emit wifiStatusChanged();
}

void WifiOptimizer::refreshSignalStrength() {
    updateSignalStrength();
}

void WifiOptimizer::applyAllOptimizations() {
    QString os = detectOS();
    if (os == "Windows") {
        QProcess::execute("netsh interface ipv4 set subinterface \"Wi-Fi\" mtu=1400 store=persistent");
    } else if (os == "Linux") {
        QProcess::execute("iwconfig wlan0 txpower 20");
    }
}

void WifiOptimizer::testGraphUpdate() {
    refreshSignalStrength();
    m_signalHistory.append(m_signalStrength);
    emit graphDataUpdated();
}

void WifiOptimizer::testFastApiIntegration() {
    sendToFastApi();
}

void WifiOptimizer::sendToFastApi() {
    QUrl url("http://localhost:8000/wifi/update");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject payload;
    payload["ssid"] = m_ssid;
    payload["strength"] = m_signalStrength;

    QNetworkReply* reply = m_netManager->post(request, QJsonDocument(payload).toJson());
    connect(reply, &QNetworkReply::finished, this, [=]() {
        auto data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        auto obj = doc.object();
        QString msg = QString("API 응답 → 상태: %1, 권장사항: %2")
                          .arg(obj["status"].toString())
                          .arg(obj["recommendation"].toString());
        emit predictionResult(msg);
        reply->deleteLater();
    });
}

void WifiOptimizer::testPredictiveEnhancement() {
    float prediction = runOnnxPrediction();
    evaluatePrediction(prediction);
}

float WifiOptimizer::runOnnxPrediction() {
    OrtSession* session = nullptr;
    OrtAllocator* allocator = nullptr;
    OrtMemoryInfo* memory_info = nullptr;

    g_ort->CreateCpuMemoryInfo(OrtArenaAllocator, OrtMemTypeDefault, &memory_info);
    g_ort->GetAllocatorWithDefaultOptions(&allocator);

    // 세션 로드
    g_ort->CreateSession(m_env, L"wifi_predictor.onnx", nullptr, &session);

    const size_t inputSize = 10;
    std::vector<float> inputTensorValues(inputSize, 0.0f);
    int start = std::max(0, m_signalHistory.size() - static_cast<int>(inputSize));
    for (int i = 0; i < inputSize && (start + i) < m_signalHistory.size(); ++i)
        inputTensorValues[i] = m_signalHistory[start + i];

    int64_t inputShape[2] = {1, static_cast<int64_t>(inputSize)};
    OrtValue* input_tensor = nullptr;

    g_ort->CreateTensorWithDataAsOrtValue(
        memory_info,
        inputTensorValues.data(),
        inputTensorValues.size() * sizeof(float),
        inputShape, 2,
        ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT,
        &input_tensor);

    const char* input_names[] = {"signal_history"};
    const char* output_names[] = {"predicted_quality"};
    OrtValue* output_tensor = nullptr;

    g_ort->Run(session, nullptr,
               input_names, (const OrtValue* const*)&input_tensor, 1,
               output_names, 1, &output_tensor);

    float* output_data = nullptr;
    g_ort->GetTensorMutableData(output_tensor, (void**)&output_data);
    float prediction = output_data[0];

    g_ort->ReleaseValue(output_tensor);
    g_ort->ReleaseValue(input_tensor);
    g_ort->ReleaseSession(session);
    g_ort->ReleaseMemoryInfo(memory_info);

    return prediction;
}

void WifiOptimizer::evaluatePrediction(float prediction) {
    QString result = QString("🔮 ONNX 예측 결과: %1").arg(prediction, 0, 'f', 2);
    if (prediction < 50.0) {
        result += "\n[AUTO] 예측 품질 낮음 → 최적화 실행됨";
        applyAllOptimizations();
    }
    emit predictionResult(result);
}

void WifiOptimizer::exportSignalHistoryToFile() {
    QFile file("wifi_signal_log.csv");
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out << "Index,SignalStrength\n";
        for (int i = 0; i < m_signalHistory.size(); ++i)
            out << i << "," << m_signalHistory[i] << "\n";
        file.close();
    }
}
