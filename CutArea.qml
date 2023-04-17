import QtQuick 2.15
import QtQuick.Shapes 1.15
import ScreenCapture.Cutter 1.0

Shape {
    component ResizeButton: Rectangle {
        border.width: 1
        color: "#52c41a"
        radius: 5
        width: 10
        height: 10
        border.color: "#ffffff"
    }
    id: root
    anchors.fill: parent
    ShapePath {
        id: path
        strokeWidth: 3
        strokeColor: "#52c41a"
        fillColor: "#88000000"
        startX: Cutter.cutAreaLeft
        startY: Cutter.cutAreaTop
        PathLine {
            x: Cutter.cutAreaRight
            y: Cutter.cutAreaTop
        }
        PathLine {
            x: Cutter.cutAreaRight
            y: Cutter.cutAreaBottom
        }
        PathLine {
            x: Cutter.cutAreaLeft
            y: Cutter.cutAreaBottom
        }
        PathLine {
            x: Cutter.cutAreaLeft
            y: Cutter.cutAreaTop
        }
        PathMove {
            x: -1 - path.strokeWidth
            y: -1 - path.strokeWidth
        }
        PathLine {
            x: root.width + path.strokeWidth
            y: -1 - path.strokeWidth
        }
        PathLine {
            x: root.width + path.strokeWidth + 1
            y: root.height + path.strokeWidth + 1
        }
        PathLine {
            x: -1 - path.strokeWidth
            y: root.height + path.strokeWidth + 1
        }
        PathLine {
            x: -1 - path.strokeWidth
            y: -1 - path.strokeWidth
        }
    }
    ResizeButton {
        x: Cutter.cutAreaLeft - path.strokeWidth * 1.5
        y: Cutter.cutAreaTop - path.strokeWidth * 1.5
    }
    ResizeButton {
        x: Cutter.cutAreaLeft + (Cutter.cutAreaRight - Cutter.cutAreaLeft) / 2
        y: Cutter.cutAreaTop - path.strokeWidth * 1.5
    }
    ResizeButton {
        x: Cutter.cutAreaRight - path.strokeWidth * 1.5
        y: Cutter.cutAreaTop - path.strokeWidth * 1.5
    }
    ResizeButton {
        x: Cutter.cutAreaRight - path.strokeWidth * 1.5
        y: Cutter.cutAreaTop + (Cutter.cutAreaBottom - Cutter.cutAreaTop) / 2
    }
    ResizeButton {
        x: Cutter.cutAreaRight - path.strokeWidth * 1.5
        y: Cutter.cutAreaBottom - path.strokeWidth * 1.5
    }
    ResizeButton {
        x: Cutter.cutAreaLeft + (Cutter.cutAreaRight - Cutter.cutAreaLeft) / 2
        y: Cutter.cutAreaBottom - path.strokeWidth * 1.5
    }
    ResizeButton {
        x: Cutter.cutAreaLeft - path.strokeWidth * 1.5
        y: Cutter.cutAreaBottom - path.strokeWidth * 1.5
    }
    ResizeButton {
        x: Cutter.cutAreaLeft - path.strokeWidth * 1.5
        y: Cutter.cutAreaTop + (Cutter.cutAreaBottom - Cutter.cutAreaTop) / 2
    }
    //    Component.onCompleted: {
    //        console.log("cutarea", App.cutAreaState)
    //    }
}
