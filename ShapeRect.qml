import QtQuick 2.15
import QtQuick.Shapes 1.15
import "Resizer.js" as Resizer

Shape {
    property string customColor: "red"
    property int x1: 0
    property int y1: 0
    property int x2: 0
    property int y2: 0
    component Dragger: Rectangle {
        width: 8
        height: 8
        border.width: 1
        border.color: "#555555"
        color: "transparent"
    }
    ShapePath {
        strokeWidth: 3
        strokeColor: customColor
        fillColor: customColor
        startX: x1
        startY: y1
        PathLine {
            x: x2
            y: y1
        }
        PathLine {
            x: x2
            y: y2
        }
        PathLine {
            x: x1
            y: y2
        }
        PathLine {
            x: x1
            y: y1
        }
    }
} //Rectangle {//  id: root//  property string customColor: "red"//  property int pressX: 0//  property int pressY: 0//  border.width: 6//  border.color: customColor//  color: "transparent"//  MouseArea {//    anchors.fill: parent//    anchors.margins: -2//    hoverEnabled: true//    cursorShape: Qt.SizeAllCursor//    onPressed: mouse => {//                 pressX = mouse.x//                 pressY = mouse.y//               }//    onPositionChanged: mouse => {//                         if (mouse.buttons === Qt.LeftButton) {//                           let spanX = mouse.x - pressX//                           let spanY = mouse.y - pressY//                           root.x = root.x + spanX//                           root.y = root.y + spanY
//                         }
//                       }

//    MouseArea {
//      anchors.fill: parent
//      anchors.margins: 4
//    }
//  }
//  Dragger {
//    anchors.left: root.left
//    anchors.top: root.top
//    MouseArea {
//      id: areaLeftTop
//      anchors.fill: parent
//      hoverEnabled: true
//      cursorShape: Qt.SizeFDiagCursor
//      onPressed: mouse => {
//                   if (mouse.buttons === Qt.LeftButton) {
//                     Resizer.start(mouse)
//                   }
//                 }
//      onPositionChanged: mouse => {
//                           if (mouse.buttons === Qt.LeftButton) {
//                             Resizer.leftTop(mouse)
//                           }
//                         }
//    }
//  }
//  Dragger {
//    anchors.left: root.left
//    anchors.top: root.top
//    anchors.topMargin: root.height / 2
//    MouseArea {
//      id: areaLeft
//      anchors.fill: parent
//      hoverEnabled: true
//      cursorShape: Qt.SizeHorCursor
//      onPressed: mouse => {
//                   if (mouse.buttons === Qt.LeftButton) {
//                     Resizer.start(mouse)
//                   }
//                 }
//      onPositionChanged: mouse => {
//                           if (mouse.buttons === Qt.LeftButton) {
//                             Resizer.left(mouse)
//                           }
//                         }
//    }
//  }
//  Dragger {
//    anchors.left: root.left
//    anchors.bottom: root.bottom
//    MouseArea {
//      anchors.fill: parent
//      hoverEnabled: true
//      cursorShape: Qt.SizeBDiagCursor
//    }
//  }
//  Dragger {
//    anchors.left: root.left
//    anchors.top: root.top
//    anchors.leftMargin: root.width / 2
//    MouseArea {
//      anchors.fill: parent
//      hoverEnabled: true
//      cursorShape: Qt.SizeVerCursor
//    }
//  }
//  Dragger {
//    anchors.left: root.left
//    anchors.bottom: root.bottom
//    anchors.leftMargin: root.width / 2
//    MouseArea {
//      anchors.fill: parent
//      hoverEnabled: true
//      cursorShape: Qt.SizeVerCursor
//    }
//  }
//  Dragger {
//    anchors.right: root.right
//    anchors.top: root.top
//    MouseArea {
//      anchors.fill: parent
//      hoverEnabled: true
//      cursorShape: Qt.SizeBDiagCursor
//    }
//  }
//  Dragger {
//    anchors.right: root.right
//    anchors.top: root.top
//    anchors.topMargin: root.height / 2
//    MouseArea {
//      id: areaRight
//      anchors.fill: parent
//      hoverEnabled: true
//      cursorShape: Qt.SizeHorCursor
//      onPressed: mouse => {
//                   if (mouse.buttons === Qt.LeftButton) {
//                     Resizer.start(mouse)
//                   }
//                 }
//      onPositionChanged: mouse => {
//                           if (mouse.buttons === Qt.LeftButton) {
//                             Resizer.right(mouse)
//                           }
//                         }
//    }
//  }
//  Dragger {
//    anchors.right: root.right
//    anchors.bottom: root.bottom
//    MouseArea {
//      id: areaRightBottom
//      anchors.fill: parent
//      hoverEnabled: true
//      cursorShape: Qt.SizeFDiagCursor
//      onPressed: mouse => {
//                   if (mouse.buttons === Qt.LeftButton) {
//                     Resizer.start(mouse)
//                   }
//                 }
//      onPositionChanged: mouse => {
//                           if (mouse.buttons === Qt.LeftButton) {
//                             Resizer.rightBottom(mouse)
//                           }
//                         }
//    }
//  }
//}

