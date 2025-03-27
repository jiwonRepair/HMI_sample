import QtQuick 2.15
import QtQuick.Controls
import QtQuick.Controls.Material 2.15

Item {
    width: parent.width
    height: parent.height

    property var booster: wifiOptimizer

    Material.theme: Material.Light

    Rectangle {
        id: contentArea
        width: parent.width
        height: parent.height
        color: "lightcoral"

        Connections {
            target: booster

            function onWifiStatusChanged() {
                updateStatus()
            }

            function onGraphDataUpdated() {
                chart.requestPaint()
            }

            function onPredictionResult(message){
                predictionLabel.text = message
            }

            function updateStatus() {
                statusText.text = "📶 " + booster.ssid + " — " + booster.signalStrength + "%";
            }
        }

        // 🔥 왼쪽 사이드바
        Rectangle {
            id: sidebar
            width: 150
            height: parent.height
            color: "#333"
            anchors.left: parent.left

            Column {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                Button {
                    text: "신호 갱신"
                    width: parent.width
                    onClicked: booster.refreshSignalStrength()
                }

                Button {
                    text: "그래프 테스트"
                    width: parent.width
                    onClicked: booster.testGraphUpdate()
                }

                Button {
                    text: "최적화"
                    width: parent.width
                    onClicked: booster.applyAllOptimizations()
                }

                Button {
                    text: "FastAPI 전송"
                    width: parent.width
                    onClicked: booster.testFastApiIntegration()
                }

                Button {
                    text: "딥러닝 예측"
                    width: parent.width
                    onClicked: booster.testPredictiveEnhancement()
                }

                Button {
                    text: "이력 저장"
                    width: parent.width
                    onClicked: booster.exportSignalHistoryToFile()
                }

                Item {
                    width: 400
                    height: 100

                    property int progressValue: 0

                    Connections {
                        target: wifiOptimizer
                        function onProgressChanged(p) {
                            progressValue = p
                        }
                        function onSaveFinished() {
                            console.log("✅ 저장 완료")
                            progressValue = 100
                        }
                    }

                    ProgressBar {
                        id: progressBar
                        width: parent.width
                        height: 20
                        from: 0
                        to: 100
                        value: progressValue

                        background: Rectangle {
                            color: "lightgray"
                            radius: 5
                        }

                        contentItem: Item {
                            Rectangle {
                                width: progressBar.visualPosition * progressBar.width
                                height: progressBar.height
                                color: progressValue < 100 ? "green" : "white"
                                radius: 5
                            }
                            Text {
                                text: progressValue + "%"
                                anchors.centerIn: parent
                                font.bold: true
                                color: "black"
                            }
                        }
                    }
                }
            }

        }

        // 🔥 메인 콘텐츠 영역
        Rectangle {
            id: canvasArea
            width: parent.width - sidebar.width
            anchors.left: sidebar.right
            height: 120
            anchors.top: parent.top
            color: contentArea.color

            Column {
                //anchors.centerIn: parent
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
            }
        }
    }
}
