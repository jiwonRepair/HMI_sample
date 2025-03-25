import QtQuick 2.15
import QtQuick.Controls

Item {
    width: parent.width
    height: parent.height

    property var booster: wifiOptimizer

    Rectangle {
        width: parent.width
        height: parent.height
        color: "lightcoral"

        Connections {
            target: booster

            function onWifiStatusChanged() {
                updateStatus();
            }

            function onGraphDataUpdated() {
                chart.requestPaint();
            }

            function onPredictionResult(message) {
                predictionLabel.text = message;
            }
        }


        function updateStatus() {
            statusText.text = "📶 " + booster.ssid + " — " + booster.signalStrength + "%";
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

            Canvas {
                id: chart
                width: 500
                height: 200
                antialiasing: true
                onPaint: {
                    var ctx = getContext("2d");
                    ctx.clearRect(0, 0, width, height);

                    var data = booster.signalHistory;
                    if (!data || data.length < 2)
                        return;

                    ctx.strokeStyle = "white";
                    ctx.lineWidth = 2;
                    ctx.beginPath();

                    for (var i = 0; i < data.length; ++i) {
                        var x = i * width / (data.length - 1);
                        var y = height - (data[i] / 100.0 * height);
                        if (i === 0)
                            ctx.moveTo(x, y);
                        else
                            ctx.lineTo(x, y);
                    }

                    ctx.stroke();
                }
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
