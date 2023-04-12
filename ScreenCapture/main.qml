import QtQuick
import QtQuick.Window
import QtQuick.Controls
import ScreenCapture.Cutter 1.0

ApplicationWindow {
  id: root
  visible: true
  x: Cutter.absoluteX
  y: Cutter.absoluteY
  width: Cutter.totalWidth
  height: Cutter.totalHeight
  flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
  property string maskColor: "#88000000"
  property string crossLineColor: "#330958d9"
  property int mousePressX: 0
  property int mousePressY: 0
  property bool isCutAreaSet: false
  property int mouseInArea: -1
  Image {
    x: 0
    y: 0
    id: backgroundImage
    source: "image://ScreenImage/background"
    sourceSize.width: root.width
    sourceSize.height: root.height
  }
  //    Rectangle{
  //        anchors.fill: parent
  //        color:maskColor
  //    }
  Rectangle {
    id: leftRect
    x: 0
    y: 0
    height: parent.height
    width: Cutter.cutAreaLeft
    color: maskColor
  }
  Rectangle {
    id: topRect
    x: Cutter.cutAreaLeft
    y: 0
    height: Cutter.cutAreaTop
    width: parent.width - Cutter.cutAreaLeft
    color: maskColor
  }
  Rectangle {
    id: rightRect
    x: Cutter.cutAreaRight
    y: Cutter.cutAreaTop
    height: parent.height - Cutter.cutAreaTop
    width: parent.width - Cutter.cutAreaRight
    color: maskColor
  }
  Rectangle {
    id: bottomRect
    x: Cutter.cutAreaLeft
    y: Cutter.cutAreaBottom
    height: parent.height - Cutter.cutAreaBottom
    width: Cutter.cutAreaRight - Cutter.cutAreaLeft
    color: maskColor
  }
  Rectangle {
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
      y: parent.height - 6
    }
  }

  MouseArea {
    id: rootArea
    anchors.fill: parent
    acceptedButtons: Qt.LeftButton | Qt.RightButton
    hoverEnabled: true
    focus: true
    cursorShape: Qt.CrossCursor
    onPositionChanged: mouse => {
                         if (mouse.buttons === Qt.NoButton && !isCutAreaSet) {
                           Cutter.getHoveredWindowRect()
                         } else if (mouse.buttons === Qt.LeftButton) {
                           Cutter.cutAreaLeft = mousePressX
                           Cutter.cutAreaTop = mousePressY
                           Cutter.cutAreaRight = mouse.x
                           Cutter.cutAreaBottom = mouse.y
                           isCutAreaSet = true
                         }

                         if (isCutAreaSet) {
                           let span = 5
                           if (mouse.x < Cutter.cutAreaLeft + span) {
                             if (mouse.y < Cutter.cutAreaTop) {
                               mouseInArea = 0 //左上
                               cursorShape = Qt.SizeFDiagCursor
                             } else if (mouse.y > Cutter.cutAreaBottom) {
                               mouseInArea = 6 //左下
                               cursorShape = Qt.SizeBDiagCursor
                             } else {
                               mouseInArea = 7 //左
                               cursorShape = Qt.SizeHorCursor
                             }
                           } else if (mouse.x > Cutter.cutAreaRight - span) {
                             if (mouse.y < Cutter.cutAreaTop) {
                               mouseInArea = 2 //右上
                               cursorShape = Qt.SizeBDiagCursor
                             } else if (mouse.y > Cutter.cutAreaBottom) {
                               mouseInArea = 4 //右下
                               cursorShape = Qt.SizeFDiagCursor
                             } else {
                               mouseInArea = 3 //右
                               cursorShape = Qt.SizeHorCursor
                             }
                           } else {
                             if (mouse.y < Cutter.cutAreaTop + span) {
                               mouseInArea = 1 //上
                               cursorShape = Qt.SizeVerCursor
                             } else if (mouse.y > Cutter.cutAreaBottom - span) {
                               mouseInArea = 5 //下
                               cursorShape = Qt.SizeVerCursor
                             } else {
                               mouseInArea = 8 //中
                               cursorShape = Qt.SizeAllCursor
                             }
                           }
                         }

                         mouseTipRect.x = mouse.x + 20
                         mouseTipRect.y = mouse.y + 20
                         if (mouseTipRect.x + mouseTipRect.width > Cutter.totalWidth) {
                           mouseTipRect.x = mouse.x - 20 - mouseTipRect.width
                         }
                         if (mouseTipRect.y + mouseTipRect.height > Cutter.totalHeight) {
                           mouseTipRect.y = mouse.y - 20 - mouseTipRect.height
                         }
                         cursorImage.source = "image://ScreenImage/cursor?" + mouse.x + "," + mouse.y
                         mouseTipRect.visible = true
                       }
    onPressed: mouse => {
                 if (mouse.button !== Qt.LeftButton) {
                   if (isCutAreaSet) {
                     isCutAreaSet = false
                     cursorShape = Qt.CrossCursor
                     Cutter.getHoveredWindowRect()
                     return
                   }
                   root.close()
                   return
                 }
                 if (isCutAreaSet) {
                   if (mouseInArea === 0) {
                     Cutter.cutAreaTop = mouse.y
                     Cutter.cutAreaLeft = mouse.x
                   } else if (mouseInArea === 1) {
                     Cutter.cutAreaTop = mouse.y
                   } else if (mouseInArea === 2) {
                     Cutter.cutAreaTop = mouse.y
                     Cutter.cutAreaRight = mouse.x
                   } else if (mouseInArea === 3) {
                     Cutter.cutAreaRight = mouse.x
                   } else if (mouseInArea === 4) {
                     Cutter.cutAreaRight = mouse.x
                     Cutter.cutAreaBottom = mouse.y
                   } else if (mouseInArea === 5) {
                     Cutter.cutAreaBottom = mouse.y
                   } else if (mouseInArea === 6) {
                     Cutter.cutAreaLeft = mouse.x
                     Cutter.cutAreaBottom = mouse.y
                   } else if (mouseInArea === 7) {
                     Cutter.cutAreaLeft = mouse.x
                   }
                 }

                 mousePressX = mouse.x
                 mousePressY = mouse.y
               }
    onReleased: mouse => {
                  if (mouse.button === Qt.LeftButton) {
                    isCutAreaSet = true
                  }
                }

    Keys.onPressed: event => {
                      if (event.modifiers & Qt.AltModifier) {
                        if (event.key === Qt.Key_X) {
                          Cutter.copyColor(true)
                        } else if (event.key === Qt.Key_C) {
                          Cutter.copyColor(false)
                        }
                      }
                    }
  }
  Rectangle {
    id: mouseTipRect
    width: 232
    height: 234
    color: "#CC000000"
    visible: false
    border.width: 1
    border.color: "#000000"
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
  //    Component.onCompleted: {
  //        console.log(Cutter.absoluteX ,Cutter.absoluteY,Cutter.totalWidth,Cutter.totalHeight);
  //    }
}
