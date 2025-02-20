import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    property real value: diskMonitor.memoryUsage  // CPU 사용률

    property var thresholds: [
        { threshold: 50, color: "#e74c3c" },  // 빨간색 (위험)
        { threshold: 30, color: "#f1c40f" },  // 노란색 (주의)
        { threshold: 0, color: "#2ecc71" }   // 초록색 (안전)
    ]

    // ✅ 원본을 변경하지 않고 정렬한 후 가장 적절한 색상을 찾음
    property color gaugeColor: thresholds
        .slice().sort((a, b) => b.threshold - a.threshold) // 내림차순 정렬
        .find(t => value >= t.threshold)?.color || "#ffffff"


    property alias canvas: canvas  // 🔥 Canvas 외부 접근 가능

    width: 220
    height: 220

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

            // 배경 원 (게이지 바탕)
            ctx.beginPath()
            ctx.arc(centerX, centerY, radius, startAngle, endAngle)
            ctx.lineWidth = 10
            ctx.strokeStyle = "#ddd"
            ctx.stroke()

            // 값에 따른 원호 (게이지 값)
            ctx.beginPath()
            ctx.arc(centerX, centerY, radius, startAngle, angle)
            ctx.lineWidth = 10
            ctx.strokeStyle = gaugeColor  // MEM 사용률 강조
            ctx.stroke()

            // 중앙 텍스트 (MEM 사용률 %)
            ctx.font = "bold 14px sans-serif"
            ctx.fillStyle = "white"//"#333"
            ctx.textAlign = "center"
            ctx.textBaseline = "middle"
            ctx.fillText(value.toFixed(1) + "%", centerX, centerY - 10)

            // 추가된 텍스트 (MEM Usage)
            ctx.font = "12px sans-serif"
            ctx.fillStyle = "white"//"#555"
            ctx.fillText("MEM", centerX, centerY + 12)
        }
    }

    // 🔥 QML에서 Memory 사용률이 변경될 때 강제 업데이트
    Connections {
        target: memoryMonitor
        function onMemoryUsageChanged() {
            //console.log("[QML] MEM Usage Updated:", memMonitor.memUsage);
            memGauge.value = memoryMonitor.memoryUsage;  // 강제 업데이트

            // 🔥 canvas가 존재하는 경우에만 requestPaint 실행
            if (memGauge.canvas) {
                memGauge.canvas.requestPaint();
            } else {
                console.warn("[QML] memGauge.canvas is undefined!");
            }
        }
    }

    // 🔥 값 변경 시 애니메이션
    NumberAnimation on value {
        duration: 500
    }
}
