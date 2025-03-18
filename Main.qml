

import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 800
    height: 480
    objectName: "mainWindow"  // ✅ C++에서 찾기 쉽게 설정

    property string popupTitle: "기본 제목"
    property string popupMessage: "기본 메시지"
    property string popupImage: ""  // 이미지 경로

    Rectangle {
        width: parent.width
        height: parent.height
        color: "#f5f5f5"

        // 🔥 팝업 창 (이미지 포함)
        Popup {
            id: myPopup
            width: 300
            height: 250
            modal: true
            focus: true
            visible: false
            closePolicy: Popup.NoAutoClose

            Rectangle {
                anchors.fill: parent
                color: "white"
                border.color: "black"
                border.width: 1
                radius: 10

                Column {
                    anchors.centerIn: parent
                    spacing: 10

                    Image {
                        source: popupImage
                        // ✅ QML에서 자동 크기 조정
                        // ✅ 너무 크면 강제로 축소
                        width: parent.width
                        height: parent.height
                        fillMode: Image.PreserveAspectFit
                        visible: popupImage !== ""  // 이미지가 있을 때만 표시
                        anchors.horizontalCenter: parent.horizontalCenter  // ✅ 가로 중앙 정렬
                        anchors.top: parent.top  // ✅ 상단 정렬
                        anchors.topMargin: 2  // ✅ 상단 여백 추가
                    }

                    Text {
                        text: popupTitle
                        font.bold: true
                        font.pixelSize: 16
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    Text {
                        text: popupMessage
                        font.pixelSize: 14
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    Button {
                        text: "닫기"
                        objectName: "popupCloseButton"
                        anchors.horizontalCenter: parent.horizontalCenter
                        onClicked: myPopup.visible = false
                    }
                }
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
                    id: hwStatusButton
                    objectName: "hwStatusButton"
                    text: "H/W status"
                    width: parent.width
                    onClicked: {
                        pageManager.showPage("hwstatus")
                        console.log("H/W Status Button clicked");
                    }
                }
                Button { id: page2Button; objectName: "page2Button"; text: "Page 2"; width: parent.width; onClicked: pageManager.showPage("page2") }
                Button { id: page3Button; objectName: "page3Button"; text: "Page 3"; width: parent.width; onClicked: pageManager.showPage("page3") }
                Button { id: page4Button; objectName: "page4Button"; text: "Page 4"; width: parent.width; onClicked: pageManager.showPage("page4") }
                Button { id: page5Button; objectName: "page5Button"; text: "Page 5"; width: parent.width; onClicked: pageManager.showPage("page5") }

                Button {
                    text: "Download from URL"
                    width: parent.width
                    onClicked: osFileManager.downloadFromUrl("https://example.com/file.txt", "D:/test_local.txt")
                }

                Button {
                    text: "Upload to URL"
                    width: parent.width
                    onClicked: osFileManager.uploadToUrl("D:/test_local.txt", "https://example.com/upload")
                }

                Button {
                    text: "Copy from USB"
                    objectName: "copyFromUsbButton"
                    width: parent.width
                    onClicked: osFileManager.copyFromUsb("E:/test_usb.txt", "D:/test_local.txt")
                }

                Button {
                    text: "Copy to USB"
                    objectName: "copyToUsbButton"
                    width: parent.width
                    onClicked: osFileManager.copyToUsb("D:/test_local.txt", "E:/test_usb.txt")
                }
            }
        }

        // 🔥 상단바
        Rectangle {
            id: topBar
            width: parent.width - sidebar.width
            height: 120
            color: "#222"
            anchors.left: sidebar.right
            anchors.top: parent.top

            Text {
                text: "System Monitor"
                color: "#fff"
                font.pixelSize: 15
                anchors.left: parent.left
                anchors.leftMargin: 20
                anchors.verticalCenter: parent.verticalCenter
            }

            CpuGauge {
                id: cpuGauge
                width: 100
                height: 100
                anchors.right: parent.right
                anchors.rightMargin: 10
                anchors.verticalCenter: parent.verticalCenter
                value: (cpuMonitor && cpuMonitor.cpuUsage) ? cpuMonitor.cpuUsage : 0
            }

            MemoryGauge {
                id: memGauge
                width: 100
                height: 100
                anchors.right: parent.right
                anchors.rightMargin: 110
                anchors.verticalCenter: parent.verticalCenter
                value: (memoryMonitor && memoryMonitor.memoryUsage) ? memoryMonitor.memoryUsage : 0
            }

            BatteryGauge {
                id: batteryGauge
                width: 100
                height: 100
                anchors.right: parent.right
                anchors.rightMargin: 210
                anchors.verticalCenter: parent.verticalCenter
                value: (batteryMonitor && batteryMonitor.batteryLevel) ? batteryMonitor.batteryLevel : 0
            }

            DiskGauge {
                id: diskGauge
                width: 100
                height: 310
                anchors.right: parent.right
                anchors.rightMargin: 310
                anchors.verticalCenter: parent.verticalCenter
                value: (diskMonitor && diskMonitor.diskUsage) ? diskMonitor.diskUsage : 0
            }

            WifiGauge {
                id: wifiGauge
                width: 100
                height: 310
                anchors.right: parent.right
                anchors.rightMargin: 410
                anchors.verticalCenter: parent.verticalCenter
                value: (wifiMonitor && wifiMonitor.signalStrength) ? wifiMonitor.signalStrength : 0
            }
        }

        // 🔥 메인 콘텐츠 영역
        Rectangle {
            id: contentArea
            width: parent.width - sidebar.width
            height: parent.height - topBar.height
            anchors.left: sidebar.right
            anchors.top: topBar.bottom
            color: "#f5f5f5"

            HWstatus { id: hwstatus; objectName: "hwstatus"; visible: true }
            Page2 { id: page2; objectName: "page2"; visible: false }
            Page3 { id: page3; objectName: "page3"; visible: false }
            Page4 { id: page4; objectName: "page4"; visible: false }
            Page5 { id: page5; objectName: "page5"; visible: false }
        }

        // C++ `pageManager.pageChanged` 신호 감지
        Connections {
            target: pageManager
            function onPageChanged(pageName) {
                console.log("Changing page to:", pageName);
                hwstatus.visible = (pageName === "hwstatus");
                page2.visible = (pageName === "page2");
                page3.visible = (pageName === "page3");
                page4.visible = (pageName === "page4");
                page5.visible = (pageName === "page5");
            }
        }

        Connections {
            target: osFileManager
            function onDownloadCompleted(destinationPath) {
                popupTitle = "✅ 다운로드 완료"
                popupMessage = "파일이 정상적으로 다운로드되었습니다."
                popupImage = "qrc:/images/download.png"
                myPopup.visible = true
            }
        }

        Connections {
            target: osFileManager
            function onUploadCompleted(usbPath) {
                popupTitle = "✅ 업로드 완료"
                popupMessage = "파일이 정상적으로 업로드되었습니다."
                popupImage = "qrc:/images/download.png"
                myPopup.visible = true
            }
        }

    }
}
