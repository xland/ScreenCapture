import QtQuick 2.15

Rectangle {
    property alias cursor: mouseArea.cursorShape
    width: 8
    height: 8
    border.width: 1
    border.color: "#555555"
    color: "transparent"
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.SizeFDiagCursor
        drag.minimumX: parent.parent.parent.x
        drag.minimumY: parent.parent.parent.y
        drag.maximumX: parent.parent.parent.width
        drag.maximumY: parent.parent.parent.width
        drag.target: parent
    }
}
