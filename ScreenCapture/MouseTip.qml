import QtQuick
import ScreenCapture.Cutter 1.0

Rectangle {
  width: 232
  height: 234
  color: "#CC000000"
  visible: false
  border.width: 1
  border.color: "#000000"
  property alias imageSource: cursorImage.source
  Image {
    id: cursorImage
    x: 1
    y: 1
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
