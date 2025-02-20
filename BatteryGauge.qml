import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    property real value: batteryMonitor.batteryLevel  // C++에서 가져온 값 사용

    // 🔥 배터리 상태별 색상 기준 (값이 작을수록 위험)
    property var thresholds: [
        { threshold: 10, color: "#e74c3c" },  // 빨간색 (위험)
        { threshold: 40, color: "#f1c40f" },  // 노란색 (주의)
        { threshold: 70, color: "#2ecc71" }   // 초록색 (안전)
    ]

    // ✅ 원본을 변경하지 않고 정렬한 후 가장 적절한 색상을 찾음
    property color gaugeColor: thresholds
        .slice().sort((a, b) => b.threshold - a.threshold) // 내림차순 정렬
        .find(t => value >= t.threshold)?.color || "#ffffff"

    property alias canvas: canvas  // 🔥 Canvas 외부 접근 가능

    width: 120
    height: 120

    Canvas {
        id: canvas
        width: parent.width
        height: parent.height
        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()

            var centerX = width / 2
            var centerY = height / 2
            var radius = width / 2 - 10
            var startAngle = Math.PI * 0.75
            var endAngle = Math.PI * 2.25
            var angle = startAngle + (endAngle - startAngle) * (value / 100)

            // 배경 원호
            ctx.beginPath()
            ctx.arc(centerX, centerY, radius, startAngle, endAngle)
            ctx.lineWidth = 10
            ctx.strokeStyle = "#ddd"
            ctx.stroke()

            // 배터리 잔량에 따른 원호
            ctx.beginPath()
            ctx.arc(centerX, centerY, radius, startAngle, angle)
            ctx.lineWidth = 10
            ctx.strokeStyle = gaugeColor
            ctx.stroke()

            // 중앙 텍스트 (배터리 %)
            ctx.font = "bold 16px sans-serif"
            ctx.fillStyle = "white"
            ctx.textAlign = "center"
            ctx.textBaseline = "middle"
            ctx.fillText(value.toFixed(0) + "%", centerX, centerY - 8)

            // 추가 텍스트 (Battery)
            ctx.font = "12px sans-serif"
            ctx.fillStyle = "white"
            ctx.fillText("Battery", centerX, centerY + 12)
        }
    }

    // 값 변경 시 UI 강제 갱신
    Connections {
        target: batteryMonitor
        function onBatteryLevelChanged() {  // ✅ 명시적인 함수 선언
            batteryGauge.value = batteryMonitor.batteryLevel;
            //console.log("[BatteryGauge] 배터리 값 변경됨:", value);

            // 🔥 canvas가 존재하는 경우에만 requestPaint 실행
            if (batteryGauge.canvas) {
                batteryGauge.canvas.requestPaint();
            } else {
                console.warn("[QML] batteryGauge.canvas is undefined!");
            }
        }
    }

    // 값 변경 시 애니메이션
    Behavior on value {
        NumberAnimation {
            duration: 500
        }
    }
}
