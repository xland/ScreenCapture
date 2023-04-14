import QtQuick
import ScreenCapture.Cutter 1.0

Rectangle {
  component ResizeButton: Rectangle {
    border.width: 1
    color: "#4096ff"
    radius: 5
    width: 10
    height: 10
    border.color: "#ffffff"
  }
  x: Cutter.cutAreaLeft
  y: Cutter.cutAreaTop
  width: Cutter.cutAreaRight - Cutter.cutAreaLeft
  height: Cutter.cutAreaBottom - Cutter.cutAreaTop
  border.color: "#4096ff"
  border.width: 3
  color: "transparent"
  ResizeButton {
    x: -4
    y: -3
  }
  ResizeButton {
    x: -4
    y: parent.height / 2 - 4
  }
  ResizeButton {
    x: -4
    y: parent.height - 8
  }
  ResizeButton {
    x: parent.width - 8
    y: -3
  }
  ResizeButton {
    x: parent.width - 7
    y: parent.height / 2 - 4
  }
  ResizeButton {
    x: parent.width - 8
    y: parent.height - 8
  }
  ResizeButton {
    x: parent.width / 2 - 8
    y: -3
  }
  ResizeButton {
    x: parent.width / 2 - 8
    y: parent.height - 7
  }
}
