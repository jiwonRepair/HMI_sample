import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Shapes 2.15

Item {
    width: parent.width
    height: parent.height

    property var wifiData: []  // Wi-Fi 신호 강도 저장
    property var timeLabels: []  // X축 시간 저장
    property int maxPoints: 20  // 최대 표시할 데이터 개수
    property real yMin: 0  // Y축 최소값
    property real yMax: 100  // Y축 최대값

    // ✅ Wi-Fi 신호 강도 변경 시 업데이트
    Connections {
        target: wifiMonitor
        function onSignalStrengthChanged() {
            if (wifiData.length >= maxPoints) {
                wifiData.shift();  // 가장 오래된 데이터 제거
                timeLabels.shift();
            }
            wifiData.push(wifiMonitor.signalStrength);  // 새로운 데이터 추가
            timeLabels.push(Qt.formatTime(new Date(), "hh:mm:ss"));  // 시간 추가

            yMin = 0;
            yMax = height;

            if (graphCanvas) {
                graphCanvas.requestPaint();
            }
        }
    }

    // ✅ 그래프 스타일링 개선
    Rectangle {
        id: background
        anchors.fill: parent
        color: "#101820"  // 어두운 블루 계열 배경
        radius: 10
    }

    Canvas {
        id: graphCanvas
        anchors.fill: parent

        onPaint: {
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);

            // ✅ X, Y축 스타일링
            ctx.strokeStyle = "white";
            ctx.lineWidth = 2;
            ctx.beginPath();
            ctx.moveTo(50, height - 40);
            ctx.lineTo(50, 20);
            ctx.moveTo(50, height - 40);
            ctx.lineTo(width - 20, height - 40);
            ctx.stroke();

            // ✅ Wi-Fi 신호 강도 데이터 곡선 그리기
            var stepX = (width - 80) / maxPoints;
            var stepY = (height - 80) / (yMax - yMin);

            ctx.strokeStyle = "#00ffff";  // 네온 블루
            ctx.lineWidth = 3;
            ctx.beginPath();

            if (wifiData.length > 1) {
                ctx.moveTo(50, height - 40 - (wifiData[0] - yMin) * stepY);

                for (let j = 1; j < wifiData.length; j++) {  // ✅ `let j` 사용하여 `i`와 충돌 방지
                    let x1 = 50 + (j - 1) * stepX;
                    let y1 = height - 40 - (wifiData[j - 1] - yMin) * stepY;
                    let x2 = 50 + j * stepX;
                    let y2 = height - 40 - (wifiData[j] - yMin) * stepY;

                    let xc = (x1 + x2) / 2;
                    let yc = (y1 + y2) / 2;

                    ctx.quadraticCurveTo(x1, y1, xc, yc);  // 곡선으로 연결
                }
            }
            ctx.stroke();

            // ✅ 데이터 포인트 (둥근 점) 추가
            ctx.fillStyle = "#00ffcc";
            for (let k = 0; k < wifiData.length; k++) {  // ✅ `let k` 사용하여 중복 방지
                let x = 50 + k * stepX;
                let y = height - 40 - (wifiData[k] - yMin) * stepY;
                ctx.beginPath();
                ctx.arc(x, y, 4, 0, 2 * Math.PI);
                ctx.fill();
            }

            // ✅ X축 라벨 추가 (시간 표시)
            ctx.fillStyle = "white";
            ctx.font = "10px Arial";
            for (let m = 0; m < timeLabels.length; m += 5) {  // ✅ `let m` 사용하여 중복 방지
                let x = 50 + m * stepX;
                ctx.fillText(timeLabels[m], x - 10, height - 20);
            }
        }
    }
}
