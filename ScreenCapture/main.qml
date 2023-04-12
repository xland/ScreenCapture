import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Dialogs
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
  property int widthForMoving: 0
  property int heightForMoving: 0
  property int cutAreaCreateState: 0 //0未开始，1创建中，2创建完成,3修改中,4移动中
  property int mouseInArea: -1 //0左上,1上,2右上,3右,4右下,5下,6左下,7左,8中
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
  function moveCutAreaByMouse(mouse) {
    if (mouse.x - mousePressX < 0) {
      Cutter.cutAreaLeft = 0
      mousePressX = mouse.x - Cutter.cutAreaLeft
      Cutter.cutAreaRight = widthForMoving
    } else if (mouse.x - mousePressX + widthForMoving > Cutter.totalWidth) {
      Cutter.cutAreaLeft = Cutter.totalWidth - widthForMoving
      Cutter.cutAreaRight = Cutter.totalWidth
      mousePressX = mouse.x - Cutter.cutAreaLeft
    } else {
      Cutter.cutAreaLeft = mouse.x - mousePressX
      Cutter.cutAreaRight = widthForMoving + Cutter.cutAreaLeft
    }
    if (mouse.y - mousePressY < 0) {
      Cutter.cutAreaTop = 0
      Cutter.cutAreaBottom = heightForMoving
      mousePressY = mouse.y - Cutter.cutAreaTop
    } else if (mouse.y - mousePressY + heightForMoving > Cutter.totalHeight) {
      Cutter.cutAreaTop = Cutter.totalHeight - heightForMoving
      Cutter.cutAreaBottom = Cutter.totalHeight
      mousePressY = mouse.y - Cutter.cutAreaTop
    } else {
      Cutter.cutAreaTop = mouse.y - mousePressY
      Cutter.cutAreaBottom = heightForMoving + Cutter.cutAreaTop
    }
  }
  function moveCutAreaByKey(type) {
    if (type === 0) {
      if (Cutter.cutAreaLeft - 1 < 0) {
        return
      }
      Cutter.cutAreaLeft -= 1
      Cutter.cutAreaRight -= 1
    } else if (type === 1) {
      if (Cutter.cutAreaTop - 1 < 0) {
        return
      }
      Cutter.cutAreaTop -= 1
      Cutter.cutAreaBottom -= 1
    } else if (type === 2) {
      if (Cutter.cutAreaRight + 1 > Cutter.totalWidth) {
        return
      }
      Cutter.cutAreaLeft += 1
      Cutter.cutAreaRight += 1
    } else if (type === 3) {
      if (Cutter.cutAreaBottom + 1 > Cutter.totalHeight) {
        return
      }
      Cutter.cutAreaBottom += 1
      Cutter.cutAreaTop += 1
    }
  }
  function setMouseTip(mouse) {
    mouseTipRect.x = mouse.x + 20
    mouseTipRect.y = mouse.y + 20
    if (mouseTipRect.x + mouseTipRect.width > Cutter.totalWidth) {
      mouseTipRect.x = mouse.x - 20 - mouseTipRect.width
    }
    if (mouseTipRect.y + mouseTipRect.height > Cutter.totalHeight) {
      mouseTipRect.y = mouse.y - 20 - mouseTipRect.height
    }
    mouseTipRect.imageSource = "image://ScreenImage/cursor?" + mouse.x + "," + mouse.y
    mouseTipRect.visible = true
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
    id: cutArea
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
  MouseTip {
    id: mouseTipRect
  }
  Popup {
    id: dialog
    modal: true
    Text {
      text: "你确定要退出截图吗？"
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
                         } else if (cutAreaCreateState === 4) {
                           if (mouse.buttons === Qt.LeftButton) {
                             moveCutAreaByMouse(mouse)
                           }
                         }
                         setMouseTip(mouse)
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
                     if (mouseInArea === 0) {
                       cutAreaCreateState = 3
                       Cutter.cutAreaTop = mouse.y
                       Cutter.cutAreaLeft = mouse.x
                     } else if (mouseInArea === 1) {
                       cutAreaCreateState = 3
                       Cutter.cutAreaTop = mouse.y
                     } else if (mouseInArea === 2) {
                       cutAreaCreateState = 3
                       Cutter.cutAreaTop = mouse.y
                       Cutter.cutAreaRight = mouse.x
                     } else if (mouseInArea === 3) {
                       cutAreaCreateState = 3
                       Cutter.cutAreaRight = mouse.x
                     } else if (mouseInArea === 4) {
                       cutAreaCreateState = 3
                       Cutter.cutAreaRight = mouse.x
                       Cutter.cutAreaBottom = mouse.y
                     } else if (mouseInArea === 5) {
                       cutAreaCreateState = 3
                       Cutter.cutAreaBottom = mouse.y
                     } else if (mouseInArea === 6) {
                       cutAreaCreateState = 3
                       Cutter.cutAreaLeft = mouse.x
                       Cutter.cutAreaBottom = mouse.y
                     } else if (mouseInArea === 7) {
                       cutAreaCreateState = 3
                       Cutter.cutAreaLeft = mouse.x
                     } else {
                       cutAreaCreateState = 4
                       mousePressX = mouse.x - Cutter.cutAreaLeft
                       mousePressY = mouse.y - Cutter.cutAreaTop
                       heightForMoving = cutArea.height
                       widthForMoving = cutArea.width
                     }
                   }
                 }
               }
    onReleased: mouse => {
                  if (mouse.button === Qt.LeftButton) {
                    if (cutAreaCreateState === 1 || cutAreaCreateState === 3 || cutAreaCreateState === 4) {
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
                        root.flags = Qt.FramelessWindowHint
                        Cutter.askForQuit()
                        root.flags = Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
                      } else if (event.key === Qt.Key_Left) {
                        Cutter.moveMousePosition(0)
                        if (cutAreaCreateState === 2) {
                          moveCutAreaByKey(0)
                        }
                      } else if (event.key === Qt.Key_Up) {
                        Cutter.moveMousePosition(1)
                        if (cutAreaCreateState === 2) {
                          moveCutAreaByKey(1)
                        }
                      } else if (event.key === Qt.Key_Right) {
                        Cutter.moveMousePosition(2)
                        if (cutAreaCreateState === 2) {
                          moveCutAreaByKey(2)
                        }
                      } else if (event.key === Qt.Key_Down) {
                        Cutter.moveMousePosition(3)
                        if (cutAreaCreateState === 2) {
                          moveCutAreaByKey(3)
                        }
                      }
                    }
  }

  //    Component.onCompleted: {
  //        console.log(Cutter.absoluteX ,Cutter.absoluteY,Cutter.totalWidth,Cutter.totalHeight);
  //    }
}
