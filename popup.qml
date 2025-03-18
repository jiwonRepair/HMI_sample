import QtQuick
import QtQuick.Controls

ApplicationWindow {
    visible: true
    width: 640
    height: 480

    property string popupTitle: "기본 제목"
    property string popupMessage: "기본 메시지"

    // 재사용 가능한 Popup
    Popup {
        id: myPopup
        width: 300
        height: 150
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
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: myPopup.visible = false
                }
            }
        }
    }

    // 버튼을 눌러서 다른 내용으로 팝업 띄우기
    Column {
        anchors.centerIn: parent
        spacing: 10

        Button {
            text: "경고 메시지"
            onClicked: {
                popupTitle = "⚠️ 경고"
                popupMessage = "이 작업은 되돌릴 수 없습니다!"
                myPopup.visible = true
            }
        }

        Button {
            text: "성공 메시지"
            onClicked: {
                popupTitle = "✅ 성공"
                popupMessage = "작업이 정상적으로 완료되었습니다."
                myPopup.visible = true
            }
        }

        Button {
            text: "정보 메시지"
            onClicked: {
                popupTitle = "ℹ️ 정보"
                popupMessage = "이벤트가 예약되었습니다."
                myPopup.visible = true
            }
        }
    }
}
