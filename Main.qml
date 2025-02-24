import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 800
    height: 480

    Rectangle {
        width: parent.width
        height: parent.height
        color: "#f5f5f5"

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

                Button { text: "H/W status"; width: parent.width; onClicked: pageManager.showPage("hwstatus") }
                Button { text: "Page 2"; width: parent.width; onClicked: pageManager.showPage("page2") }
                Button { text: "Page 3"; width: parent.width; onClicked: pageManager.showPage("page3") }
                Button { text: "Page 4"; width: parent.width; onClicked: pageManager.showPage("page4") }
                Button { text: "Page 5"; width: parent.width; onClicked: pageManager.showPage("page5") }
            }
        }

        // 🔥 사이드바 오른쪽에 위치하는 상단바
        Rectangle {
            id: topBar
            width: parent.width - sidebar.width  // 🔥 사이드바 오른쪽 영역만큼 차지
            height: 120
            color: "#222"
            anchors.left: sidebar.right  // 🔥 사이드바 오른쪽에 배치
            anchors.top: parent.top

            Text {
                text: "System Monitor"
                color: "#fff"
                font.pixelSize: 15
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
            }

            // 🔥 CPU 게이지 추가 (상단바 오른쪽)
            CpuGauge {
                id: cpuGauge
                width: 100
                height: 100
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                value: cpuMonitor.cpuUsage // 🔥 CPU 사용률 반영
            }

            // 🔥 CPU 게이지 추가 (상단바 오른쪽)
            MemoryGauge {
                id: memGauge
                width: 100
                height: 100
                anchors.right: parent.right
                anchors.rightMargin: 110
                anchors.verticalCenter: parent.verticalCenter
                value: memoryMonitor.memoryUsage // 🔥 CPU 사용률 반영
            }

            // 🔥 배터리 게이지 추가 (CPU 게이지 오른쪽)
            BatteryGauge {
                id: batteryGauge
                width: 100
                height: 100
                anchors.right: parent.right  // 🔥 상단바 오른쪽 끝에 배치
                anchors.rightMargin: 210
                anchors.verticalCenter: parent.verticalCenter
                value: batteryMonitor ? batteryMonitor.batteryLevel : 0  // ✅ NULL 체크 추가
                //onValueChanged: console.log("[BatteryGauge] 배터리 값 변경됨:", value)
            }

            // 🔥 배터리 게이지 추가 (CPU 게이지 오른쪽)
            DiskGauge {
                id: diskGauge
                width: 100
                height: 310
                anchors.right: parent.right  // 🔥 상단바 오른쪽 끝에 배치
                anchors.rightMargin: 310
                anchors.verticalCenter: parent.verticalCenter
                value: diskMonitor ? diskMonitor.diskUsage : 0  // ✅ NULL 체크 추가
                //onValueChanged: console.log("[DiskGauge] 배터리 값 변경됨:", value)
            }

            // 🔥 배터리 게이지 추가 (CPU 게이지 오른쪽)
            WifiGauge {
                id: wifiGauge
                width: 100
                height: 310
                anchors.right: parent.right  // 🔥 상단바 오른쪽 끝에 배치
                anchors.rightMargin: 410
                anchors.verticalCenter: parent.verticalCenter
                value: wifiMonitor ? wifiMonitor.wifiGauge : 0  // ✅ NULL 체크 추가
            }


        }

        // 🔥 메인 콘텐츠 영역 (상단바 아래에 배치)
        Rectangle {
            id: contentArea
            width: parent.width - sidebar.width
            height: parent.height - topBar.height
            anchors.left: sidebar.right
            anchors.top: topBar.bottom  // 🔥 상단바 아래로 배치
            color: "#f5f5f5"

            HWstatus { id: hwstatus; visible: true }
            Page2 { id: page2; visible: false }
            Page3 { id: page3; visible: false }
            Page4 { id: page4; visible: false }
            Page5 { id: page5; visible: false }
        }

        // C++ `pageManager.pageChanged` 신호를 감지하여 페이지 변경
        Connections {
            target: pageManager
            function onPageChanged(pageName) {
                console.log("Changing page to:", pageName); // 디버깅용 로그
                hwstatus.visible = (pageName === "hwstatus");
                page2.visible = (pageName === "page2");
                page3.visible = (pageName === "page3");
                page4.visible = (pageName === "page4");
                page5.visible = (pageName === "page5");
            }
        }
    }
}
