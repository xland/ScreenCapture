import QtQuick 2.15

Rectangle {
    property alias cursor: mouseArea.cursorShape
    //    property alias axis: mouseArea.drag.axis
    signal moved(real x, real y)
    QtObject {
        id: my
        property real x: 0
        property real y: 0
    }
    id: root
    width: 8
    height: 8
    color: "#dddddd"
    border.color: "#000000"
    border.width: 0.5
    antialiasing: true
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.SizeFDiagCursor
        onPressed: mouse => {
                       my.x = mouse.x
                       my.y = mouse.y
                   }
        onPositionChanged: mouse => {
                               if (mouse.buttons === Qt.LeftButton) {
                                   if (cursorShape === Qt.SizeVerCursor) {
                                       root.y = root.y + mouse.y - my.y
                                   } else if (cursorShape === Qt.SizeHorCursor) {
                                       root.x = root.x + mouse.x - my.x
                                   } else {
                                       root.y = root.y + mouse.y - my.y
                                       root.x = root.x + mouse.x - my.x
                                   }
                                   moved(root.x, root.y)
                               }
                           }
        onReleased: mouse => {}
        //        drag.minimumX: parent.parent.parent.x
        //        drag.minimumY: parent.parent.parent.y
        //        drag.maximumX: parent.parent.parent.width
        //        drag.maximumY: parent.parent.parent.width
        //        drag.axis: Drag.XAndYAxis
        //        drag.target: parent
    }
}
