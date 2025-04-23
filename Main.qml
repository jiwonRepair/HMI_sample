import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

ApplicationWindow {
    visible: true
    width: 1000
    height: 680
    objectName: "mainWindow"  // ✅ C++에서 찾기 쉽게 설정

    property string popupTitle: "기본 제목"
    property string popupMessage: "기본 메시지"
    property string popupImage: ""  // 이미지 경로

    // ✅ 진행률 변수
    property int progressValue: 0

    // ✅ Qt Quick Controls 스타일 적용 (Material 또는 Fusion)
    Material.theme: Material.Light

    Rectangle {
        width: parent.width
        height: parent.height
        color: "#f5f5f5"

        // 🔥 팝업 창 (이미지 포함)
        Popup {
            id: myPopup
            width: 350
            height: 300
            modal: true
            focus: true
            visible: false
            closePolicy: Popup.NoAutoClose
            // 부모(ApplicationWindow)의 가운데에 배치
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2

            Rectangle {
                anchors.fill: parent
                color: "white"
                border.color: "black"
                border.width: 1
                radius: 10

                Column {
                    anchors.centerIn: parent
                    spacing: 10

                    Item {  // ⚡ Image를 감싸는 Item 추가
                        width: parent.width
                        height: 150  // 적절한 높이 지정

                        Image {
                            source: popupImage // 이미지 파일 경로
                            // ✅ QML에서 자동 크기 조정
                            // ✅ 너무 크면 강제로 축소
                            width: Math.min(parent.width * 0.8, implicitWidth)
                            height: Math.min(parent.height * 0.8, implicitHeight)
                            fillMode: Image.PreserveAspectFit // 원본 비율 유지
                            visible: popupImage !== ""  // 이미지가 있을 때만 표시
                            anchors.bottom: parent.bottom  // 부모의 아래쪽에 배치
                            anchors.bottomMargin: 10  // 10px 위로 띄우기
                            anchors.horizontalCenter: parent.horizontalCenter  // 가로 중앙 정렬
                        }
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
            width: 200
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
                    onClicked: {
                        osFileManager.downloadFromUrl("https://example.com/file.txt", "D:/test_local.txt")
                    }
                }

                Button {
                    text: "Upload to URL"
                    width: parent.width
                    onClicked: {
                        osFileManager.uploadToUrl("D:/test_local.txt", "https://example.com/upload")
                    }
                }

                Button {
                    text: "Copy from USB"
                    objectName: "copyFromUsbButton"
                    width: parent.width
                    onClicked: {
                        console.log("copy from usb button clicked");
                        osFileManager.copyFromUsb("E:/test.zip", "D:/test.zip")
                    }
                }

                Button {
                    text: "Copy to USB"
                    objectName: "copyToUsbButton"
                    width: parent.width
                    onClicked: {
                        console.log("copy to usb button clicked");
                        osFileManager.copyToUsb("D:/test.zip", "E:/test.zip")
                    }
                }

                // ✅ 진행률 바 (다운로드 & 복사 공용)
                Item {
                    width: parent.width
                    height: parent.height

                    ProgressBar {                        
                        id: progressBar
                        width: parent.width
                        height: 20
                        from: 0
                        to: 100
                        value: progressValue
                        background: Rectangle {
                            color: "lightgray" // ✅ 진행되지 않은 부분의 색상
                            radius: 5
                        }
                        contentItem: Item {
                            Rectangle {
                                width: progressBar.visualPosition * progressBar.width
                                height: progressBar.height
                                //color: progressValue < 100 ? "green" : "white" // ✅ 진행 중일 때 초록색, 완료 시 흰색
                                radius: 5

                                // ✅ 색상 변경 없이 일정하게 유지 + 애니메이션 부드럽게
                                color: "green"

                                Behavior on width {
                                    NumberAnimation { duration: 150 }
                                }

                            }

                            Text {
                                text: progressValue + "%"
                                anchors.centerIn: parent
                                font.bold: true
                                color: "black"
                            }
                        }
                    }

                    // ✅ 반드시 ProgressBar와 같은 부모 안에서 정의해야 target: progressBar 인식됨
                    PropertyAnimation {
                        id: progressFade
                        target: progressBar
                        property: "opacity"
                        to: 0
                        duration: 500
                        onStopped: progressBar.visible = false
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

        // Connections {
        //     target: osFileManager

        //     function onProgressChanged(progress) {
        //         progressValue = progress
        //         //console.log("진행률:", progress)
        //     }

        //     function onDownloadStarted() {
        //         progressBar.visible = true
        //         progressBar.opacity = 1
        //         progressValue = 0      // ✅ 다운로드 시 초기화
        //     }

        //     function onUploadStarted() {
        //         progressBar.visible = true
        //         progressBar.opacity = 1
        //         progressValue = 0      // ✅ 다운로드 시 초기화
        //     }

        //     function onDownloadCompleted(savePath) {
        //         progressValue = 100
        //         popupTitle = "✅ 다운로드 완료"
        //         popupMessage = "파일이 정상적으로 다운로드되었습니다."
        //         popupImage = "qrc:/images/download.png"
        //         myPopup.visible = true
        //     }

        //     function onUploadCompleted(filePath) {
        //         progressValue = 100
        //         popupTitle = "✅ 업로드 완료"
        //         popupMessage = "파일이 정상적으로 업로드되었습니다."
        //         popupImage = "qrc:/images/download.png"
        //         myPopup.visible = true
        //     }

        //     function onErrorOccurred(error) {
        //         popupTitle = "❌ 오류 발생"
        //         popupMessage = "오류 메시지: " + error
        //         console.log("FileManager ERROR : " + error)
        //         popupImage = "qrc:/images/error.png"
        //         myPopup.visible = true
        //     }
        // }

        Connections {
            target: osFileManager

            function onUploadStarted() {
                progressValue = 0;
                progressBar.visible = true;
                progressBar.opacity = 1;
            }

            function onDownloadStarted() {
                progressValue = 0;
                progressBar.visible = true;
                progressBar.opacity = 1;
            }

            function onProgressChanged(progress) {
                progressValue = progress;
            }

            function onDownloadCompleted(savePath) {
                progressValue = 100
                popupTitle = "✅ 다운로드 완료"
                popupMessage = "파일이 정상적으로 다운로드되었습니다."
                popupImage = "qrc:/images/download.png"
                myPopup.visible = true
            }

            function onUploadCompleted(filePath) {
                progressValue = 100
                popupTitle = "✅ 업로드 완료"
                popupMessage = "파일이 정상적으로 업로드되었습니다."
                popupImage = "qrc:/images/download.png"
                myPopup.visible = true
            }

            function onErrorOccurred(error) {
                popupTitle = "❌ 오류 발생"
                popupMessage = "오류 메시지: " + error
                console.log("FileManager ERROR : " + error)
                popupImage = "qrc:/images/error.png"
                myPopup.visible = true
            }
        }

    }
}
