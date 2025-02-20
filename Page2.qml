import QtQuick 2.15
import QtQuick.Controls 2.15
//import QtCharts 2.15
//import QtCharts 1.0

Item {
    width: parent.width
    height: parent.height

    // ChartView {
    //     id: chart
    //     anchors.fill: parent
    //     antialiasing: true
    //     theme: ChartView.ChartThemeDark

        // // ✅ X축 (CategoryAxis로 시간 표현)
        // CategoryAxis {
        //     id: axisX
        //     titleText: "Time"
        //     labelsPosition: CategoryAxis.AxisLabelsPositionOnValue
        // }

        // // ✅ Y축 (Wi-Fi 신호 강도)
        // CategoryAxis {
        //     id: axisY
        //     titleText: "Signal Strength (%)"
        // }

        // // ✅ Wi-Fi 신호 강도를 나타내는 선 그래프
        // LineSeries {
        //     id: signalSeries
        //     name: "Wi-Fi Signal Strength"
        //     axisX: axisX
        //     axisY: axisY
        // }

        // // ✅ 타이머: 1초마다 랜덤 신호 값 추가
        // Timer {
        //     id: updateTimer
        //     interval: 1000  // 1초마다 실행
        //     running: true
        //     repeat: true
        //     property int timeIndex: 0  // X축의 시간 인덱스 관리

        //     onTriggered: {
        //         let yValue = Math.random() * 100;  // 0~100 사이 랜덤 신호 값
        //         let label = Qt.formatTime(new Date(), "hh:mm:ss");  // 현재 시간

        //         // X축에 새로운 시간 라벨 추가 (최대 10개까지만 유지)
        //         if (axisX.count >= 10) {
        //             axisX.remove(0);  // 오래된 시간 삭제
        //         }
        //         axisX.append(label, timeIndex);  // 새로운 시간 추가

        //         // Y축에 값 추가 (고정된 범주 사용)
        //         if (axisY.count === 0) {
        //             axisY.append("0%", 0);
        //             axisY.append("20%", 20);
        //             axisY.append("40%", 40);
        //             axisY.append("60%", 60);
        //             axisY.append("80%", 80);
        //             axisY.append("100%", 100);
        //         }

        //         // 데이터 추가 (최대 10개까지만 유지)
        //         if (signalSeries.count >= 10) {
        //             signalSeries.remove(0);  // 오래된 데이터 삭제
        //         }
        //         signalSeries.append(timeIndex, yValue);  // 새 데이터 추가
        //         timeIndex += 1;  // X축 인덱스 증가
        //     }
        // }
    //}

    // ChartView {
    //     id: chart
    //     width: parent.width
    //     height: parent.height
    //     antialiasing: true
    //     theme: ChartView.ChartThemeDark

    //     // ❌ ValueAxis 대신 CategoryAxis 사용
    //     CategoryAxis {
    //         id: axisX
    //         titleText: "Time"
    //         labelsPosition: CategoryAxis.AxisLabelsPositionOnValue
    //     }

    //     CategoryAxis {
    //         id: axisY
    //         titleText: "Signal Strength (%)"
    //     }

    //     LineSeries {
    //         id: signalSeries
    //         name: "Wi-Fi Signal Strength"
    //         axisX: axisX
    //         axisY: axisY
    //     }
    // }

    // Component.onCompleted: {
    //     wifiMonitor.series = signalSeries;  // ✅ C++에서 QML의 LineSeries 연결
    // }

    // Timer {
    //     interval: 2000
    //     running: true
    //     repeat: true
    //     onTriggered: wifiMonitor.updateSignalStrength()
    // }

    // ChartView {
    //     id: chart
    //     width: parent.width
    //     height: parent.height
    //     antialiasing: true
    //     theme: ChartView.ChartThemeDark

    //     ValueAxis { id: axisX; min: 0; max: 10; titleText: "Time" }  // ✅ 축을 미리 정의
    //     ValueAxis { id: axisY; min: 0; max: 100; titleText: "Signal Strength (%)" }

    //     LineSeries {
    //         id: signalSeries
    //         name: "Wi-Fi Signal Strength"
    //         axisX: axisX  // ✅ 축을 별도로 정의 후 바인딩
    //         axisY: axisY
    //     }
    // }

    // Timer {
    //     interval: 2000
    //     running: true
    //     repeat: true
    //     onTriggered: {
    //         if (signalSeries.count >= 10) {
    //             signalSeries.remove(0);
    //         }
    //         signalSeries.append(signalSeries.count, wifiMonitor.signalStrength)
    //     }
    // }
}
