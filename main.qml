import QtQuick 2.15
import QtQuick.Controls 2.15
import ScreenCapture.Cutter 1.0
import QtQuick.Shapes 1.15

ApplicationWindow {
    FontLoader {
        source: "font.ttf"
    }
    id: root
    visible: true
    x: Cutter.absoluteX
    y: Cutter.absoluteY
    width: Cutter.totalWidth
    height: Cutter.totalHeight
    //    width: 300
    //    height: 300
    //    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    flags: Qt.FramelessWindowHint
    Item {
        id: board
        anchors.fill: parent
        Image {
            x: 0
            y: 0
            id: backgroundImage
            source: "image://ScreenImage/background"
            sourceSize.width: root.width
            sourceSize.height: root.height
        }
        PaintBoard {
            id: paintBoard
        }
    }

    WindowMouseArea {
        id: windowMouseArea
    }
    CutArea {
        id: cutArea
    }
    DrawTool {
        id: drawTool
    }
    DrawRectTool {
        id: drawRectTool
        anchors.left: drawTool.left
        anchors.top: drawTool.top
        anchors.topMargin: 34
    }
    DrawArrowTool {
        id: drawArrowTool
        anchors.left: drawTool.left
        anchors.top: drawTool.top
        anchors.topMargin: 34
    }
    DrawPenTool {
        id: drawPenTool
        anchors.left: drawTool.left
        anchors.top: drawTool.top
        anchors.topMargin: 34
    }
    DrawMosaicTool {
        id: drawMosaicTool
        anchors.left: drawTool.left
        anchors.top: drawTool.top
        anchors.topMargin: 34
    }
    DrawEraserTool {
        id: drawEraserTool
        anchors.left: drawTool.left
        anchors.top: drawTool.top
        anchors.topMargin: 34
    }
    MouseTip {
        id: mouseTipRect
    }

    //  Component.onCompleted: {

    //  }
    //  sourceRectangle.grabToImage(function(result) {
    //             result.saveToFile("something.png")
    //          })
}
