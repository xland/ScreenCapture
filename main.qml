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
    MouseTip {
        id: mouseTipRect
    }

    //    function moveCutAreaByKey(type) {
    //        if (type === 0) {
    //            if (Cutter.cutAreaLeft - 1 < 0) {
    //                return
    //            }
    //            Cutter.cutAreaLeft -= 1
    //            Cutter.cutAreaRight -= 1
    //        } else if (type === 1) {
    //            if (Cutter.cutAreaTop - 1 < 0) {
    //                return
    //            }
    //            Cutter.cutAreaTop -= 1
    //            Cutter.cutAreaBottom -= 1
    //        } else if (type === 2) {
    //            if (Cutter.cutAreaRight + 1 > Cutter.totalWidth) {
    //                return
    //            }
    //            Cutter.cutAreaLeft += 1
    //            Cutter.cutAreaRight += 1
    //        } else if (type === 3) {
    //            if (Cutter.cutAreaBottom + 1 > Cutter.totalHeight) {
    //                return
    //            }
    //            Cutter.cutAreaBottom += 1
    //            Cutter.cutAreaTop += 1
    //        }
    //    }
    //    Component.onCompleted: {
    //        console.log("main", App.cutAreaState)
    //    }

    //  Component.onCompleted: {

    //  }
    //  sourceRectangle.grabToImage(function(result) {
    //             result.saveToFile("something.png")
    //          })
}
