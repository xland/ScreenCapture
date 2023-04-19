import QtQuick 2.15
import QtQuick.Shapes 1.15
import ScreenCapture.Cutter 1.0

Item {
    property bool isFill: false
    property string bgColor: "red"
    property string borderColor: "red"
    property real borderWidth: 1
    property bool showDragger: false
    function pathPointChanged(start, end) {
        let arr = Cutter.getArrowPoints(start, end, 20, 24)
        arrowShapePath.startX = start.x
        arrowShapePath.startY = start.y
        for (var i = 0; i < arr.length; i++) {
            arrowShapePath.pathElements[i].x = arr[i].x
            arrowShapePath.pathElements[i].y = arr[i].y
        }
        arrowShapePath.changed()
        draggerStart.x = start.x - draggerStart.width / 2
        draggerStart.y = start.y - draggerStart.width / 2
        draggerEnd.x = end.x - draggerStart.width / 2
        draggerEnd.y = end.y - draggerStart.width / 2
    }
    function done() {
        if (showDragger) {
            showDragger = false
            shapeMouseArea.cursorShape = Qt.CrossCursor
        }
    }
    function movePosition() {
        let start = Qt.point(draggerStart.x + draggerStart.width / 2, draggerStart.y + draggerStart.width / 2)
        let end = Qt.point(draggerEnd.x + draggerStart.width / 2, draggerEnd.y + draggerStart.width / 2)
        pathPointChanged(start, end)
    }

    onBgColorChanged: () => {
                          arrowShapePath.fillColor = bgColor
                          arrowShapePath.changed()
                      }
    id: root
    anchors.fill: parent
    Shape {
        id: arrowShape
        anchors.fill: parent
        containsMode: Shape.FillContains
        antialiasing: true
        layer.enabled: true
        layer.samples: 8
        ShapePath {
            id: arrowShapePath
            strokeWidth: borderWidth
            strokeColor: borderColor
            fillColor: bgColor
            startX: 0
            startY: 0
            PathLine {
                x: 0
                y: 0
            }
            PathLine {
                x: 0
                y: 0
            }
            PathLine {
                x: 0
                y: 0
            }
            PathLine {
                x: 0
                y: 0
            }
            PathLine {
                x: 0
                y: 0
            }
            PathLine {
                x: 0
                y: 0
            }
        }
    }
    MouseArea {
        property real pressX: 0
        property real pressY: 0
        property bool draging: false
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
                           if (arrowShape.contains(p)) {
                               shapeMouseArea.pressX = mouse.x
                               shapeMouseArea.pressY = mouse.y
                               shapeMouseArea.draging = true
                               shapeMouseArea.cursorShape = Qt.SizeAllCursor
                           }
                       } else {
                           done()
                           mouse.accepted = false
                       }
                   }
        onPositionChanged: mouse => {
                               let p = Qt.point(mouse.x, mouse.y)
                               if (arrowShape.contains(p)) {
                                   shapeMouseArea.cursorShape = Qt.SizeAllCursor
                               } else {
                                   shapeMouseArea.cursorShape = Qt.CrossCursor
                               }

                               if (shapeMouseArea.draging) {
                                   let spanX = mouse.x - shapeMouseArea.pressX
                                   let spanY = mouse.y - shapeMouseArea.pressY
                                   draggerStart.x += spanX
                                   draggerStart.y += spanY
                                   draggerEnd.x += spanX
                                   draggerEnd.y += spanY
                                   movePosition()
                                   shapeMouseArea.pressX = mouse.x
                                   shapeMouseArea.pressY = mouse.y
                               } else {
                                   mouse.accepted = false
                               }
                           }
        onReleased: mouse => {
                        shapeMouseArea.draging = false
                        mouse.accepted = false
                    }

        Dragger {
            id: draggerStart
            cursor: Qt.SizeAllCursor
            onMoved: () => movePosition()
        }
        Dragger {
            id: draggerEnd
            cursor: Qt.SizeAllCursor
            onMoved: () => movePosition()
        }
    }
}
