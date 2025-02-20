import QtQuick 2.15
import QtQuick.Controls 2.15


Item {
    width: parent.width
    height: parent.height

    Text {
        //text: "Hardware Status"
        font.pixelSize: 20
        color: "black"
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 10
    }

    // ✅ `hardwareStatusModel`의 `count`가 정상적으로 로드되었는지 확인
    Component.onCompleted: {
        console.log("[HardwareStatusModel] Model initialized with:", hardwareStatusModel.count, "items");
    }

    // ✅ 페이지가 다시 보이면 강제로 갱신
    onVisibleChanged: {
        if (visible) {
            console.log("[DEBUG] Page1 ListView is now visible. Refreshing model...");
            hardwareList.model = null;
            hardwareList.model = hardwareStatusModel;
        }
    }

    ListView {
        id: hardwareList
        width: parent.width - 20
        height: parent.height - 50
        anchors.centerIn: parent
        spacing: 10
        clip: true
        model: hardwareStatusModel

        delegate: Rectangle {
            width: hardwareList.width
            height: 60
            radius: 10
            color: "lightblue"
            border.color: "black"
            border.width: 1

            Column {
                anchors.fill: parent
                spacing: 5

                Text {
                    text: model.deviceName  // ✅ 장치 이름
                    color: "black"
                    font.pixelSize: 18
                }

                Text {
                    text: model.instanceId  // ✅ Instance ID 표시
                    color: "gray"
                    font.pixelSize: 14
                }
            }
        }
    }

}
