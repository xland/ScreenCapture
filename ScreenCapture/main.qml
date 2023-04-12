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
  property int cutAreaCreateState: 0 //0未开始，1创建中，2创建完成,3修改中
  property int mouseInArea: -1
  function setMouseArea(mouse) {
    let span = 5
    if (mouse.x < Cutter.cutAreaLeft + span) {
      if (mouse.y < Cutter.cutAreaTop) {
        mouseInArea = 0 //左上
        rootArea.cursorShape = Qt.SizeFDiagCursor
      } else if (mouse.y > Cutter.cutAreaBottom) {
        mouseInArea = 6 //左下
        rootArea.cursorShape = Qt.SizeBDiagCursor
      } else {
        mouseInArea = 7 //左
        rootArea.cursorShape = Qt.SizeHorCursor
      }
    } else if (mouse.x > Cutter.cutAreaRight - span) {
      if (mouse.y < Cutter.cutAreaTop) {
        mouseInArea = 2 //右上
        rootArea.cursorShape = Qt.SizeBDiagCursor
      } else if (mouse.y > Cutter.cutAreaBottom) {
        mouseInArea = 4 //右下
        rootArea.cursorShape = Qt.SizeFDiagCursor
      } else {
        mouseInArea = 3 //右
        rootArea.cursorShape = Qt.SizeHorCursor
      }
    } else {
      if (mouse.y < Cutter.cutAreaTop + span) {
        mouseInArea = 1 //上
        rootArea.cursorShape = Qt.SizeVerCursor
      } else if (mouse.y > Cutter.cutAreaBottom - span) {
        mouseInArea = 5 //下
        rootArea.cursorShape = Qt.SizeVerCursor
      } else {
        mouseInArea = 8 //中
        rootArea.cursorShape = Qt.SizeAllCursor
      }
    }
  }

  function createCutAreaByDrag(mouse) {
    let left = mousePressX, top = mousePressY, right = mouse.x, bottom = mouse.y
    if (mouse.x < left) {
      left = mouse.x
      right = mousePressX
    }
    if (mouse.y < top) {
      top = mouse.y
      bottom = mousePressY
    }
    Cutter.cutAreaLeft = left
    Cutter.cutAreaTop = top
    Cutter.cutAreaRight = right
    Cutter.cutAreaBottom = bottom
  }
  function resizeCutArea(mouse) {
    if (mouseInArea === 0) {
      Cutter.cutAreaLeft = mouse.x
      Cutter.cutAreaTop = mouse.y
      if (Cutter.cutAreaLeft > Cutter.cutAreaRight && Cutter.cutAreaTop > Cutter.cutAreaBottom) {
        mouseInArea = 4
        Cutter.cutAreaLeft = Cutter.cutAreaRight
        Cutter.cutAreaTop = Cutter.cutAreaBottom
      } else if (Cutter.cutAreaLeft > Cutter.cutAreaRight) {
        mouseInArea = 2
        rootArea.cursorShape = Qt.SizeBDiagCursor
        Cutter.cutAreaLeft = Cutter.cutAreaRight
      } else if (Cutter.cutAreaTop > Cutter.cutAreaBottom) {
        mouseInArea = 6
        rootArea.cursorShape = Qt.SizeBDiagCursor
        Cutter.cutAreaTop = Cutter.cutAreaBottom
      }
    } else if (mouseInArea === 1) {
      Cutter.cutAreaTop = mouse.y
      if (Cutter.cutAreaTop > Cutter.cutAreaBottom) {
        mouseInArea = 5
        Cutter.cutAreaTop = Cutter.cutAreaBottom
      }
    } else if (mouseInArea === 2) {
      Cutter.cutAreaRight = mouse.x
      Cutter.cutAreaTop = mouse.y
      if (Cutter.cutAreaLeft > Cutter.cutAreaRight && Cutter.cutAreaTop > Cutter.cutAreaBottom) {
        mouseInArea = 6
        Cutter.cutAreaLeft = Cutter.cutAreaRight
        Cutter.cutAreaTop = Cutter.cutAreaBottom
      } else if (Cutter.cutAreaLeft > Cutter.cutAreaRight) {
        mouseInArea = 0
        rootArea.cursorShape = Qt.SizeFDiagCursor
        Cutter.cutAreaLeft = Cutter.cutAreaRight
      } else if (Cutter.cutAreaTop > Cutter.cutAreaBottom) {
        mouseInArea = 4
        rootArea.cursorShape = Qt.SizeFDiagCursor
        Cutter.cutAreaBottom = Cutter.cutAreaTop
      }
    } else if (mouseInArea === 3) {
      Cutter.cutAreaRight = mouse.x
      if (Cutter.cutAreaLeft > Cutter.cutAreaRight) {
        mouseInArea = 7
        Cutter.cutAreaLeft = Cutter.cutAreaRight
      }
    } else if (mouseInArea === 4) {
      Cutter.cutAreaRight = mouse.x
      Cutter.cutAreaBottom = mouse.y
      if (Cutter.cutAreaLeft > Cutter.cutAreaRight && Cutter.cutAreaTop > Cutter.cutAreaBottom) {
        mouseInArea = 0
        Cutter.cutAreaLeft = Cutter.cutAreaRight
        Cutter.cutAreaTop = Cutter.cutAreaBottom
      } else if (Cutter.cutAreaLeft > Cutter.cutAreaRight) {
        mouseInArea = 6
        rootArea.cursorShape = Qt.SizeBDiagCursor
        Cutter.cutAreaLeft = Cutter.cutAreaRight
      } else if (Cutter.cutAreaTop > Cutter.cutAreaBottom) {
        mouseInArea = 2
        rootArea.cursorShape = Qt.SizeBDiagCursor
        Cutter.cutAreaTop = Cutter.cutAreaBottom
      }
    } else if (mouseInArea === 5) {
      Cutter.cutAreaBottom = mouse.y
      if (Cutter.cutAreaTop > Cutter.cutAreaBottom) {
        mouseInArea = 1
        Cutter.cutAreaTop = Cutter.cutAreaBottom
      }
    } else if (mouseInArea === 6) {
      Cutter.cutAreaLeft = mouse.x
      Cutter.cutAreaBottom = mouse.y
      if (Cutter.cutAreaLeft > Cutter.cutAreaRight && Cutter.cutAreaTop > Cutter.cutAreaBottom) {
        mouseInArea = 2
        Cutter.cutAreaLeft = Cutter.cutAreaRight
        Cutter.cutAreaTop = Cutter.cutAreaBottom
      } else if (Cutter.cutAreaLeft > Cutter.cutAreaRight) {
        mouseInArea = 4
        rootArea.cursorShape = Qt.SizeFDiagCursor
        Cutter.cutAreaRight = Cutter.cutAreaLeft
      } else if (Cutter.cutAreaTop > Cutter.cutAreaBottom) {
        mouseInArea = 0
        rootArea.cursorShape = Qt.SizeFDiagCursor
        Cutter.cutAreaBottom = Cutter.cutAreaTop
      }
    } else if (mouseInArea === 7) {
      Cutter.cutAreaLeft = mouse.x
      if (Cutter.cutAreaLeft > Cutter.cutAreaRight) {
        mouseInArea = 3
        Cutter.cutAreaLeft = Cutter.cutAreaRight
      }
    }
  }

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
                         if (cutAreaCreateState === 0) {
                           if (mouse.buttons === Qt.NoButton) {
                             Cutter.createCutAreaByWindowRect()
                           }
                         } else if (cutAreaCreateState === 1) {
                           if (mouse.buttons === Qt.LeftButton) {
                             createCutAreaByDrag(mouse)
                           }
                         } else if (cutAreaCreateState === 2) {
                           if (mouse.buttons === Qt.NoButton) {
                             setMouseArea(mouse)
                           }
                         } else if (cutAreaCreateState === 3) {
                           if (mouse.buttons === Qt.LeftButton) {
                             resizeCutArea(mouse)
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
                   if (cutAreaCreateState !== 0) {
                     cutAreaCreateState = 0
                     cursorShape = Qt.CrossCursor
                     Cutter.createCutAreaByWindowRect()
                   } else {
                     root.close()
                   }
                 } else {
                   if (cutAreaCreateState === 0) {
                     cutAreaCreateState = 1
                     mousePressX = mouse.x
                     mousePressY = mouse.y
                   } else if (cutAreaCreateState === 2) {
                     cutAreaCreateState = 3
                     mousePressX = mouse.x
                     mousePressY = mouse.y
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
                     } else {

                     }
                   }
                 }
               }
    onReleased: mouse => {
                  if (mouse.button === Qt.LeftButton) {
                    if (cutAreaCreateState === 1 || cutAreaCreateState === 3) {
                      cutAreaCreateState = 2
                    }
                  }
                }
    Keys.onPressed: event => {
                      if (event.modifiers & Qt.AltModifier) {
                        if (event.key === Qt.Key_X) {
                          Cutter.copyColor(true)
                        } else if (event.key === Qt.Key_C) {
                          Cutter.copyColor(false)
                        }
                      } else if (event.key === Qt.Key_Escape) {
                        //todo confirm
                        //https://doc-snapshots.qt.io/qt6-dev/qml-qtquick-dialogs-messagedialog.html#details
                        Qt.quit()
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
