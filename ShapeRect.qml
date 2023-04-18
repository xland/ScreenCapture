import QtQuick 2.15
import QtQuick.Shapes 1.15

Item {
    property string customColor: "red"
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
    function done() {
        if (showDragger) {
            showDragger = false
            shapeMouseArea.cursorShape = Qt.CrossCursor
            //解绑
            circleShape.visible = !App.rectCircle.isRect
            rectShape.visible = App.rectCircle.isRect

            circleShape.strokeColor = App.rectCircle.strokeColor
            rectShape.strokeColor = App.rectCircle.strokeColor

            circleShape.fillColor = App.rectCircle.fillColor
            rectShape.fillColor = App.rectCircle.fillColor

            circleShape.strokeWidth = App.rectCircle.strokeWidth
            rectShape.strokeWidth = App.rectCircle.strokeWidth
        }
    }

    id: root
    anchors.fill: parent
    Shape {
        id: rectShape
        containsMode: Shape.FillContains
        visible: App.rectCircle.isRect
        anchors.fill: parent
        ShapePath {
            strokeWidth: App.rectCircle.strokeWidth
            strokeColor: customColor
            fillColor: customColor
            startX: startX1
            startY: startY1
            PathLine {
                x: endX
                y: startY1
            }
            PathLine {
                x: endX
                y: endY
            }
            PathLine {
                x: startX1
                y: endY
            }
            PathLine {
                x: startX1
                y: startY1
            }
        }
    }
    Shape {
        id: circleShape
        visible: !App.rectCircle.isRect
        antialiasing: true
        layer.enabled: true
        layer.samples: 8
        anchors.fill: parent
        ShapePath {
            fillColor: customColor
            strokeColor: customColor
            strokeWidth: App.rectCircle.strokeWidth
            PathAngleArc {
                centerX: startX1 + (endX - startX1) / 2
                centerY: startY1 + (endY - startY1) / 2
                radiusX: (endX - startX1) / 2
                radiusY: (endY - startY1) / 2
                startAngle: 0
                sweepAngle: 360
            }
        }
    }
    MouseArea {
        property real pressX: 0
        property real pressY: 0
        id: shapeMouseArea
        anchors.fill: parent
        containmentMask: App.rectCircle.isRect ? rectShape : circleShape
        hoverEnabled: true
        cursorShape: Qt.SizeAllCursor
        visible: showDragger
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        propagateComposedEvents: true
        onPressed: mouse => {
                       if (mouse.buttons === Qt.LeftButton) {
                           shapeMouseArea.pressX = mouse.x
                           shapeMouseArea.pressY = mouse.y
                       } else {
                           done()
                       }
                   }
        onPositionChanged: mouse => {
                               if (mouse.buttons === Qt.LeftButton) {
                                   let spanX = mouse.x - shapeMouseArea.pressX
                                   let spanY = mouse.y - shapeMouseArea.pressY
                                   startX1 += spanX
                                   startY1 += spanY
                                   endX += spanX
                                   endY += spanY
                                   setDraggerPosition()
                                   shapeMouseArea.pressX = mouse.x
                                   shapeMouseArea.pressY = mouse.y
                               } else {
                                   mouse.accepted = false
                               }
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
}
