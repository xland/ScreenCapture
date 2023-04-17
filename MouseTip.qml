import QtQuick 2.15
import ScreenCapture.Cutter 1.0

Rectangle {
    property string crossLineColor: "#330958d9"
    id: root
    x: App.mouseTip.x
    y: App.mouseTip.y
    width: App.mouseTip.width
    height: App.mouseTip.height
    color: "#CC000000"
    border.width: 1
    border.color: "#000000"
    visible: App.mouseTip.visible
    antialiasing: true
    Image {
        y:1
        id: cursorImage
        source: App.mouseTip.src
        anchors.horizontalCenter: parent.horizontalCenter
        Rectangle {
            x: 0
            y: 75
            height: 10
            width: 110
            color: crossLineColor
        }
        Rectangle {
            x: 120
            y: 75
            height: 10
            width: 110
            color: crossLineColor
        }
        Rectangle {
            x: 110
            y: 0
            height: 75
            width: 10
            color: crossLineColor
        }
        Rectangle {
            x: 110
            y: 85
            height: 75
            width: 10
            color: crossLineColor
        }
    }
    Text {
        id: mouseTipText1
        anchors.top: cursorImage.bottom
        anchors.topMargin: 6
        anchors.left: parent.left
        anchors.leftMargin: 8
        text: "坐标：x: " + Cutter.mouseX + "  y: " + Cutter.mouseY
        color: "#ffffff"
        verticalAlignment: Text.AlignVCenter
    }
    Text {
        id: mouseTipText2
        anchors.top: mouseTipText1.bottom
        anchors.topMargin: 6
        anchors.left: parent.left
        anchors.leftMargin: 8
        verticalAlignment: Text.AlignVCenter
        text: "颜色：" + Cutter.colorRgb + "  " + Cutter.colorHex
        color: "#ffffff"
    }
    Text {
        id: mouseTipText3
        anchors.top: mouseTipText2.bottom
        anchors.topMargin: 6
        anchors.left: parent.left
        anchors.leftMargin: 8
        verticalAlignment: Text.AlignVCenter
        text: "复制：rgb: Alt+X  hex: Alt+C"
        color: "#cccccc"
    }
}
