import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    visible: true
    width: 800
    height: 480
    objectName: "mainWindow"  // ✅ C++에서 찾기 쉽게 설정

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

                // ✅ 다운로드 버튼
                Button {
                    id: downloadButton
                    text: "Download File"
                    onClicked: {
                        osFileManager.downloadFile("https://example.com/sample.zip", "/home/user/Downloads/sample.zip")
                        downloadButton.text = "Downloading..."
                    }
                }

                // ✅ 업로드 버튼
                Button {
                    id: uploadButton
                    text: "Upload File"
                    onClicked: {
                        osFileManager.uploadFile("/home/user/Documents/test.txt", "https://example.com/upload")
                        uploadButton.text = "Uploading..."
                    }
                }

                // ✅ 다운로드/업로드 취소 버튼
                Button {
                    id: cancelButton
                    text: "Cancel Download"
                    onClicked: {
                        osFileManager.cancelDownload()
                        downloadButton.text = "Download File"
                        uploadButton.text = "Upload File"
                    }
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

        // ✅ 다운로드 완료 이벤트 처리
        Connections {
            target: osFileManager
            onDownloadCompleted: downloadButton.text = "Download File"
            onUploadCompleted: uploadButton.text = "Upload File"
        }

    }
}
