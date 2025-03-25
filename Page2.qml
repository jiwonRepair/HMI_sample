import QtQuick 2.15
import QtQuick.Controls
import QtCharts
//title: "Wi-Fi Optimizer Demo"

Item {
    width: parent.width
    height: parent.height

    Rectangle {
        width: parent.width
        height: parent.height
        color: "lightcoral"

        WifiOptimizer {
            id: booster
            onWifiStatusChanged: updateStatus()
            onGraphDataUpdated: updateChart()
            onPredictionResult: message => predictionLabel.text = message
        }

        function updateStatus() {
            statusText.text = "📶 " + booster.ssid + " — " + booster.signalStrength + "%";
        }

        function updateChart() {
            series.clear();
            var data = booster.signalHistory;
            for (var i = 0; i < data.length; ++i)
                series.append(i, data[i]);
        }

        Column {
            anchors.centerIn: parent
            spacing: 10

            Text {
                id: statusText
                font.pixelSize: 18
                text: "신호 측정 중..."
            }

            Text {
                id: predictionLabel
                font.pixelSize: 16
                wrapMode: Text.WordWrap
                text: "예측 결과 없음"
            }

            ChartView {
                width: 500
                height: 200
                theme: ChartView.ChartThemeLight
                antialiasing: true

                LineSeries {
                    id: series
                    name: "신호 강도 (%)"
                }

                ValueAxis {
                    id: axisX
                    min: 0
                    max: 10
                    titleText: "측정 횟수"
                }

                ValueAxis {
                    id: axisY
                    min: 0
                    max: 100
                    titleText: "신호 세기"
                }

                axes: [
                    AxisPair { axis: axisX; orientation: Qt.Horizontal },
                    AxisPair { axis: axisY; orientation: Qt.Vertical }
                ]
            }

            Row {
                spacing: 10

                Button {
                    text: "신호 갱신"
                    onClicked: booster.refreshSignalStrength()
                }

                Button {
                    text: "그래프 테스트"
                    onClicked: booster.testGraphUpdate()
                }

                Button {
                    text: "최적화"
                    onClicked: booster.applyAllOptimizations()
                }
            }

            Row {
                spacing: 10

                Button {
                    text: "FastAPI 전송"
                    onClicked: booster.testFastApiIntegration()
                }

                Button {
                    text: "딥러닝 예측"
                    onClicked: booster.testPredictiveEnhancement()
                }

                Button {
                    text: "이력 저장"
                    onClicked: booster.exportSignalHistoryToFile()
                }
            }
        }
    }
}
