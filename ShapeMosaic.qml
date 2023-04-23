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

    function positionChanged(x, y) {
        if (startX1 > x) {
            endX = startX1
            startX1 = x
        } else {
            endX = x
        }
        if (startY1 > y) {
            endY = startY1
            startY1 = y
        } else {
            endY = y
        }
        if (shaderImg.source) {
            //            shader.sourceSize.width = endX - startX1
            //            shader.sourceSize.height = endY - startY1
            shader.width = endX - startX1
            shader.height = endY - startY1
            shader.x = startX1
            shader.y = startY1
            shaderImg.x = 0 - startX1
            shaderImg.y = 0 - startY1
        }

        //        shadersource: "image://ScreenImage/background"
    }

    id: root
    anchors.fill: parent

    Item {
        id: shader
        clip: true
        Image {
            id: shaderImg
            //        visible: false
        }
    }

    FastBlur {
        id: blur
        anchors.fill: shader
        source: shader
        radius: 32
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
                           let p = Qt.point(mouse.x, mouse.y)
                           if ((isRect && rectShape.contains(p)) || (!isRect && circleShape.contains(p))) {
                               shapeMouseArea.pressX = mouse.x
                               shapeMouseArea.pressY = mouse.y
                               shapeMouseArea.dragging = true
                           }
                       } else {
                           Shapes.done()
                       }
                   }
        onPositionChanged: mouse => {
                               let p = Qt.point(mouse.x, mouse.y)
                               if (mouse.buttons === Qt.LeftButton) {
                                   if (shapeMouseArea.dragging) {
                                       let spanX = mouse.x - shapeMouseArea.pressX
                                       let spanY = mouse.y - shapeMouseArea.pressY
                                       startX1 += spanX
                                       startY1 += spanY
                                       endX += spanX
                                       endY += spanY
                                       setDraggerPosition()
                                       shapeMouseArea.pressX = mouse.x
                                       shapeMouseArea.pressY = mouse.y
                                   }
                               } else {
                                   if ((isRect && rectShape.contains(p)) || (!isRect && circleShape.contains(p))) {
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
                         setDraggerPosition()
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
                         setDraggerPosition()
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
                         setDraggerPosition()
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
                         setDraggerPosition()
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
                         setDraggerPosition()
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
                         setDraggerPosition()
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
                         setDraggerPosition()
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
                         setDraggerPosition()
                     }
        }
    }
    Component.onCompleted: {
        parent.parent.children[0].grabToImage(function (result) {
            shaderImg.source = result.url
        })
    }
}
