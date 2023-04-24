import QtQuick 2.15
import QtGraphicalEffects 1.15
import QtQuick.Shapes 1.15

Item {
    property real startX1: 0
    property real startY1: 0
    property real endX: 0
    property real endY: 0
    property bool showDragger: false
    property real span: draggerLeftTop.width / 2
    function setDraggerPosition() {
        draggerLeftTop.x = startX1 - span
        draggerLeftTop.y = startY1 - span

        draggerTop.x = startX1 + (endX - startX1) / 2 - span
        draggerTop.y = startY1 - span

        draggerRightTop.x = endX - span
        draggerRightTop.y = startY1 - span

        draggerRight.x = endX - span
        draggerRight.y = startY1 + (endY - startY1) / 2 - span

        draggerRightBottom.x = endX - span
        draggerRightBottom.y = endY - span

        draggerBottom.x = startX1 + (endX - startX1) / 2 - span
        draggerBottom.y = endY - span

        draggerLeftBottom.x = startX1 - span
        draggerLeftBottom.y = endY - span

        draggerLeft.x = startX1 - span
        draggerLeft.y = startY1 + (endY - startY1) / 2 - span
    }
    function updatePosition() {
        shader.width = endX - startX1
        shader.height = endY - startY1
        shader.x = startX1
        shader.y = startY1
        shaderImg.x = 0 - startX1
        shaderImg.y = 0 - startY1
        setDraggerPosition()
    }

    function positionChanged(x1, y1, x2, y2) {
        if (x1 > x2) {
            startX1 = x2
            endX = x1
        } else {
            startX1 = x1
            endX = x2
        }
        if (y1 > y2) {
            startY1 = y2
            endY = y1
        } else {
            startY1 = y1
            endY = y2
        }
        if (shaderImg.source) {
            updatePosition()
        }
    }

    id: root
    anchors.fill: parent

    Item {
        id: shader
        clip: true
        visible: false
        Image {
            id: shaderImg
        }
    }
    FastBlur {
        id: blur
        anchors.fill: shader
        source: shader
        radius: 32
    }
    Rectangle {
        border.color: "#000"
        border.width: 1
        anchors.fill: blur
        antialiasing: true
        color: "transparent"
    }
    MouseArea {
        property real pressX: 0
        property real pressY: 0
        property bool dragging: false
        id: shapeMouseArea
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.CrossCursor
        visible: showDragger
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        propagateComposedEvents: true
        onPressed: mouse => {
                       if (mouse.buttons === Qt.LeftButton) {
                           if (shapeMouseArea.cursorShape === Qt.SizeAllCursor) {
                               shapeMouseArea.pressX = mouse.x
                               shapeMouseArea.pressY = mouse.y
                               shapeMouseArea.dragging = true
                           }
                       } else {
                           Shapes.done()
                       }
                   }
        onPositionChanged: mouse => {
                               if (mouse.buttons === Qt.LeftButton) {
                                   if (shapeMouseArea.dragging) {
                                       let spanX = mouse.x - shapeMouseArea.pressX
                                       let spanY = mouse.y - shapeMouseArea.pressY
                                       startX1 += spanX
                                       startY1 += spanY
                                       endX += spanX
                                       endY += spanY
                                       updatePosition()
                                       shapeMouseArea.pressX = mouse.x
                                       shapeMouseArea.pressY = mouse.y
                                   }
                               } else {
                                   if (mouse.x > startX1 && mouse.y > startY1 && mouse.x < endX && mouse.y < endY) {
                                       shapeMouseArea.cursorShape = Qt.SizeAllCursor
                                   } else {
                                       shapeMouseArea.cursorShape = Qt.CrossCursor
                                   }
                               }
                           }
        onReleased: mouse => {
                        mouse.accepted = false
                    }

        //左上
        Dragger {
            id: draggerLeftTop
            x: startX1 - span
            y: startY1 - span
            onMoved: (x, y) => {
                         startX1 = x + span
                         startY1 = y + span
                         updatePosition()
                     }
        }
        //上
        Dragger {
            id: draggerTop
            cursor: Qt.SizeVerCursor
            x: startX1 + (endX - startX1) / 2 - span
            y: startY1 - span
            onMoved: (x, y) => {
                         startY1 = y + span
                         updatePosition()
                     }
        }
        //右上
        Dragger {
            id: draggerRightTop
            cursor: Qt.SizeBDiagCursor
            x: endX - span
            y: startY1 - span
            onMoved: (x, y) => {
                         endX = x + span
                         startY1 = y + span
                         updatePosition()
                     }
        }
        //右
        Dragger {
            id: draggerRight
            cursor: Qt.SizeHorCursor
            x: endX - span
            y: startY1 + (endY - startY1) / 2 - span
            onMoved: (x, y) => {
                         endX = x + span
                         updatePosition()
                     }
        }
        //右下
        Dragger {
            id: draggerRightBottom
            x: endX - span
            y: endY - span
            onMoved: (x, y) => {
                         endX = x + span
                         endY = y + span
                         updatePosition()
                     }
        }
        //下
        Dragger {
            id: draggerBottom
            cursor: Qt.SizeVerCursor
            x: startX1 + (endX - startX1) / 2 - span
            y: endY - span
            onMoved: (x, y) => {
                         endY = y + span
                         updatePosition()
                     }
        }
        //左下
        Dragger {
            id: draggerLeftBottom
            cursor: Qt.SizeBDiagCursor
            x: startX1 - span
            y: endY - span
            onMoved: (x, y) => {
                         startX1 = x + span
                         endY = y + span
                         updatePosition()
                     }
        }
        //左
        Dragger {
            id: draggerLeft
            cursor: Qt.SizeHorCursor
            x: startX1 - span
            y: startY1 + (endY - startY1) / 2 - span
            onMoved: (x, y) => {
                         startX1 = x + span
                         updatePosition()
                     }
        }
    }
    Component.onCompleted: {
        parent.parent.children[0].grabToImage(function (result) {
            shaderImg.source = result.url
        })
    }
}
