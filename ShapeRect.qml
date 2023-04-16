import QtQuick 2.15
import QtQuick.Shapes 1.15

MouseArea {
    property string customColor: "red"
    property alias startX: path.startX
    property alias startY: path.startY
    property real endX: 0
    property real endY: 0
    function setDraggerPosition() {
        draggerLeftTop.x = startX - my.span
        draggerLeftTop.y = startY - my.span

        draggerTop.x = startX + (endX - startX) / 2
        draggerTop.y = startY - my.span

        draggerRightTop.x = endX - my.span
        draggerRightTop.y = startY - my.span

        draggerRight.x = endX - my.span
        draggerRight.y = startY + (endY - startY) / 2

        draggerRightBottom.x = endX - my.span
        draggerRightBottom.y = endY - my.span

        draggerBottom.x = startX + (endX - startX) / 2
        draggerBottom.y = endY - my.span

        draggerLeftBottom.x = startX - my.span
        draggerLeftBottom.y = endY - my.span

        draggerLeft.x = startX - my.span
        draggerLeft.y = startY + (endY - startY) / 2
    }

    QtObject {
        id: my
        property bool showDragger: false
        property real span: draggerLeftTop.width / 2
    }
    anchors.fill: parent
    acceptedButtons: Qt.LeftButton | Qt.RightButton
    hoverEnabled: true
    cursorShape: Qt.CrossCursor
    onPressed: mouse => {
                   if (mouse.buttons === Qt.LeftButton) {
                       startX = mouse.x
                       startY = mouse.y
                       endX = mouse.x
                       endY = mouse.y
                   }
               }
    onPositionChanged: mouse => {
                           if (mouse.buttons === Qt.LeftButton) {
                               endX = mouse.x
                               endY = mouse.y
                           }
                       }
    onReleased: () => {
                    my.showDragger = true
                }
    Shape {
        id: shape
        ShapePath {
            id: path
            strokeWidth: 3
            strokeColor: customColor
            fillColor: "transparent"
            PathLine {
                x: endX
                y: startY
            }
            PathLine {
                x: endX
                y: endY
            }
            PathLine {
                x: startX
                y: endY
            }
            PathLine {
                x: startX
                y: startY
            }
        }
    }
    //左上
    Dragger {
        id: draggerLeftTop
        visible: my.showDragger
        x: startX - my.span
        y: startY - my.span
        onMoved: (x, y) => {
                     startX = x + my.span
                     startY = y + my.span
                     setDraggerPosition()
                 }
    }
    //上
    Dragger {
        id: draggerTop
        visible: my.showDragger
        cursor: Qt.SizeVerCursor
        x: startX + (endX - startX) / 2
        y: startY - my.span
        onMoved: (x, y) => {
                     startY = y + my.span
                     setDraggerPosition()
                 }
    }
    //右上
    Dragger {
        id: draggerRightTop
        visible: my.showDragger
        cursor: Qt.SizeBDiagCursor
        x: endX - my.span
        y: startY - my.span
        onMoved: (x, y) => {
                     endX = x + my.span
                     startY = y + my.span
                     setDraggerPosition()
                 }
    }
    //右
    Dragger {
        id: draggerRight
        visible: my.showDragger
        cursor: Qt.SizeHorCursor
        x: endX - my.span
        y: startY + (endY - startY) / 2
        onMoved: (x, y) => {
                     endX = x + my.span
                     setDraggerPosition()
                 }
    }
    //右下
    Dragger {
        id: draggerRightBottom
        visible: my.showDragger
        x: endX - my.span
        y: endY - my.span
        onMoved: (x, y) => {
                     endX = x + my.span
                     endY = y + my.span
                     setDraggerPosition()
                 }
    }
    //下
    Dragger {
        id: draggerBottom
        visible: my.showDragger
        cursor: Qt.SizeVerCursor
        x: startX + (endX - startX) / 2
        y: endY - my.span
        onMoved: (x, y) => {
                     endY = y + my.span
                     setDraggerPosition()
                 }
    }
    //左下
    Dragger {
        id: draggerLeftBottom
        visible: my.showDragger
        cursor: Qt.SizeBDiagCursor
        x: startX - my.span
        y: endY - my.span
        onMoved: (x, y) => {
                     startX = x + my.span
                     endY = y + my.span
                     setDraggerPosition()
                 }
    }
    //左
    Dragger {
        id: draggerLeft
        visible: my.showDragger
        cursor: Qt.SizeHorCursor
        x: startX - my.span
        y: startY + (endY - startY) / 2
        onMoved: (x, y) => {
                     startX = x + my.span
                     setDraggerPosition()
                 }
    }
}
