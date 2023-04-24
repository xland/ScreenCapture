import QtQuick 2.15
import QtQuick.Shapes 1.15
import "Shapes.js" as Shapes

Item {
    property bool isRect: true
    property bool isFill: false
    property string bgColor: "transparent"
    property real borderWidth: 3
    property string borderColor: "red"
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
    function getDrawInfo() {
        let result = {
            "type": "Rect",
            "strokeWidth": borderWidth,
            "strokeColor": borderColor,
            "fillColor": bgColor,
            "isFill": isFill,
            "points": []
        }
        if (isRect) {
            result.points.push({
                                   "x": startX1,
                                   "y": startY1
                               })
            result.points.push({
                                   "x": endX - startX1,
                                   "y": endY - startY1
                               })
        } else {
            result.type = "Circle"
            result.points.push({
                                   "x": startX1,
                                   "y": startY1
                               })
            result.points.push({
                                   "x": endX - startX1,
                                   "y": endY - startY1
                               })
        }
        return result
    }

    onBgColorChanged: () => {
                          circleShapePath.fillColor = bgColor
                          rectShapePath.fillColor = bgColor
                          rectShapePath.changed()
                          circleShapePath.changed()
                      }

    id: root
    anchors.fill: parent
    Shape {
        id: circleShape
        visible: !isRect
        antialiasing: true
        layer.enabled: true
        layer.samples: 8
        anchors.fill: parent
        containsMode: Shape.FillContains

        ShapePath {
            id: circleShapePath
            fillColor: bgColor
            strokeColor: borderColor
            strokeWidth: borderWidth
            PathAngleArc {
                id: pathAngleArc
                centerX: startX1 + (endX - startX1) / 2
                centerY: startY1 + (endY - startY1) / 2
                radiusX: (endX - startX1) / 2
                radiusY: (endY - startY1) / 2
                startAngle: 0
                sweepAngle: 360
            }
        }
    }
    Shape {
        id: rectShape
        antialiasing: true
        layer.samples: 8
        visible: isRect
        anchors.fill: parent
        containsMode: Shape.FillContains
        ShapePath {
            id: rectShapePath
            strokeWidth: borderWidth
            strokeColor: borderColor
            fillColor: bgColor
            PathMove {
                x: startX1
                y: startY1
            }
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
                           } else {
                               mouse.accepted = false
                           }
                       } else {
                           mouse.accepted = false
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
}