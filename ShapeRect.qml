import QtQuick 2.15
import QtQuick.Shapes 1.15

MouseArea {
    property string customColor: "red"
    property alias startX: path.startX
    property alias startY: path.startY
    property real endX: 0
    property real endY: 0
    property bool showDragger: false
    property real span: draggerLeftTop.width / 2
    function setDraggerPosition() {
        draggerLeftTop.x = startX - span
        draggerLeftTop.y = startY - span

        draggerTop.x = startX + (endX - startX) / 2
        draggerTop.y = startY - span

        draggerRightTop.x = endX - span
        draggerRightTop.y = startY - span

        draggerRight.x = endX - span
        draggerRight.y = startY + (endY - startY) / 2

        draggerRightBottom.x = endX - span
        draggerRightBottom.y = endY - span

        draggerBottom.x = startX + (endX - startX) / 2
        draggerBottom.y = endY - span

        draggerLeftBottom.x = startX - span
        draggerLeftBottom.y = endY - span

        draggerLeft.x = startX - span
        draggerLeft.y = startY + (endY - startY) / 2
    }
    anchors.fill: parent
    acceptedButtons: Qt.LeftButton | Qt.RightButton
    hoverEnabled: true
    cursorShape: Qt.CrossCursor
    onPressed: mouse => {
                   if (mouse.buttons === Qt.LeftButton) {
                       console.log("b", App.pressPosition.x)
                       if (showDragger) {
                           //绘图完成后的点击,接下去要画第二个了
                           showDragger = false
                       } else {
                           //开始绘图前的点击
                           startX = mouse.x
                           startY = mouse.y
                           endX = mouse.x
                           endY = mouse.y
                       }
                   }
               }
    onPositionChanged: mouse => {
                           if (mouse.buttons === Qt.LeftButton) {
                               endX = mouse.x
                               endY = mouse.y
                           }
                       }
    onReleased: () => {
                    showDragger = true
                }
    Shape {
        id: shape
        containsMode: Shape.FillContains
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
    MouseArea {
        id: shapeMouseArea
        anchors.fill: parent
        containmentMask: shape
        hoverEnabled: true
        cursorShape: Qt.SizeAllCursor
        property real pressX: 0
        property real pressY: 0
        onPressed: mouse => {
                       shapeMouseArea.pressX = mouse.x
                       shapeMouseArea.pressY = mouse.y
                   }
        onPositionChanged: mouse => {
                               if (mouse.buttons === Qt.LeftButton) {
                                   let spanX = mouse.x - shapeMouseArea.pressX
                                   let spanY = mouse.y - shapeMouseArea.pressY
                                   startX += spanX
                                   startY += spanY
                                   endX += spanX
                                   endY += spanY
                                   setDraggerPosition()
                                   shapeMouseArea.pressX = mouse.x
                                   shapeMouseArea.pressY = mouse.y
                               }
                           }
    }
    //左上
    Dragger {
        id: draggerLeftTop
        visible: showDragger
        x: startX - span
        y: startY - span
        onMoved: (x, y) => {
                     startX = x + span
                     startY = y + span
                     setDraggerPosition()
                 }
    }
    //上
    Dragger {
        id: draggerTop
        visible: showDragger
        cursor: Qt.SizeVerCursor
        x: startX + (endX - startX) / 2
        y: startY - span
        onMoved: (x, y) => {
                     startY = y + span
                     setDraggerPosition()
                 }
    }
    //右上
    Dragger {
        id: draggerRightTop
        visible: showDragger
        cursor: Qt.SizeBDiagCursor
        x: endX - span
        y: startY - span
        onMoved: (x, y) => {
                     endX = x + span
                     startY = y + span
                     setDraggerPosition()
                 }
    }
    //右
    Dragger {
        id: draggerRight
        visible: showDragger
        cursor: Qt.SizeHorCursor
        x: endX - span
        y: startY + (endY - startY) / 2
        onMoved: (x, y) => {
                     endX = x + span
                     setDraggerPosition()
                 }
    }
    //右下
    Dragger {
        id: draggerRightBottom
        visible: showDragger
        x: endX - span
        y: endY - span
        onMoved: (x, y) => {
                     endX = x + span
                     endY = y + span
                     setDraggerPosition()
                 }
    }
    //下
    Dragger {
        id: draggerBottom
        visible: showDragger
        cursor: Qt.SizeVerCursor
        x: startX + (endX - startX) / 2
        y: endY - span
        onMoved: (x, y) => {
                     endY = y + span
                     setDraggerPosition()
                 }
    }
    //左下
    Dragger {
        id: draggerLeftBottom
        visible: showDragger
        cursor: Qt.SizeBDiagCursor
        x: startX - span
        y: endY - span
        onMoved: (x, y) => {
                     startX = x + span
                     endY = y + span
                     setDraggerPosition()
                 }
    }
    //左
    Dragger {
        id: draggerLeft
        visible: showDragger
        cursor: Qt.SizeHorCursor
        x: startX - span
        y: startY + (endY - startY) / 2
        onMoved: (x, y) => {
                     startX = x + span
                     setDraggerPosition()
                 }
    }
}
