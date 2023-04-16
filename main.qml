import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import ScreenCapture.Cutter 1.0
import "ShapeCreator.js" as ShapeCreator

ApplicationWindow {
    id: root
    visible: true
    x: Cutter.absoluteX
    y: Cutter.absoluteY
    width: Cutter.totalWidth
    height: Cutter.totalHeight
    flags: Qt.FramelessWindowHint
    property string maskColor: "#88000000"
    property string crossLineColor: "#330958d9"
    property int mousePressX: 0
    property int mousePressY: 0
    property int widthForMoving: 0
    property int heightForMoving: 0
    property int cutAreaState: 0 //0未开始，1创建中，2创建完成,3修改中,4移动中,5绘制中
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
            if (Cutter.cutAreaLeft > Cutter.cutAreaRight
                    && Cutter.cutAreaTop > Cutter.cutAreaBottom) {
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
            if (Cutter.cutAreaLeft > Cutter.cutAreaRight
                    && Cutter.cutAreaTop > Cutter.cutAreaBottom) {
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
            if (Cutter.cutAreaLeft > Cutter.cutAreaRight
                    && Cutter.cutAreaTop > Cutter.cutAreaBottom) {
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
            if (Cutter.cutAreaLeft > Cutter.cutAreaRight
                    && Cutter.cutAreaTop > Cutter.cutAreaBottom) {
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
        if (cutAreaState === 0 || (cutAreaState === 2 && mouseInArea === 8)) {
            mouseTipRect.x = mouse.x + 20
            mouseTipRect.y = mouse.y + 20
            if (mouseTipRect.x + mouseTipRect.width > Cutter.totalWidth) {
                mouseTipRect.x = mouse.x - 20 - mouseTipRect.width
            }
            if (mouseTipRect.y + mouseTipRect.height > Cutter.totalHeight) {
                mouseTipRect.y = mouse.y - 20 - mouseTipRect.height
            }
            mouseTipRect.imageSource = "image://ScreenImage/cursor?" + mouse.x + "," + mouse.y
            mouseTipRect.visible = true //todo 鼠标移动时，会忽闪一下
        } else {
            mouseTipRect.visible = false
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
    CutArea {
        id: cutArea
    }
    MouseArea {
        id: rootArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true
        focus: true
        cursorShape: Qt.CrossCursor
        onPositionChanged: mouse => {
                               if (cutAreaState === 0) {
                                   if (mouse.buttons === Qt.NoButton) {
                                       Cutter.createCutAreaByWindowRect()
                                   }
                               } else if (cutAreaState === 1) {
                                   if (mouse.buttons === Qt.LeftButton) {
                                       createCutAreaByDrag(mouse)
                                   }
                               } else if (cutAreaState === 2) {
                                   if (mouse.buttons === Qt.NoButton) {
                                       setMouseArea(mouse)
                                   }
                               } else if (cutAreaState === 3) {
                                   if (mouse.buttons === Qt.LeftButton) {
                                       resizeCutArea(mouse)
                                   }
                               } else if (cutAreaState === 4) {
                                   if (mouse.buttons === Qt.LeftButton) {
                                       moveCutAreaByMouse(mouse)
                                   }
                               }
                               setMouseTip(mouse)
                           }
        onPressed: mouse => {
                       if (mouse.button !== Qt.LeftButton) {
                           if (cutAreaState !== 0) {
                               cutAreaState = 0
                               cursorShape = Qt.CrossCursor
                               Cutter.createCutAreaByWindowRect()
                           } else {
                               root.close()
                           }
                       } else {
                           if (cutAreaState === 0) {
                               cutAreaState = 1
                               mousePressX = mouse.x
                               mousePressY = mouse.y
                           } else if (cutAreaState === 2) {
                               if (mouseInArea === 0) {
                                   cutAreaState = 3
                                   Cutter.cutAreaTop = mouse.y
                                   Cutter.cutAreaLeft = mouse.x
                               } else if (mouseInArea === 1) {
                                   cutAreaState = 3
                                   Cutter.cutAreaTop = mouse.y
                               } else if (mouseInArea === 2) {
                                   cutAreaState = 3
                                   Cutter.cutAreaTop = mouse.y
                                   Cutter.cutAreaRight = mouse.x
                               } else if (mouseInArea === 3) {
                                   cutAreaState = 3
                                   Cutter.cutAreaRight = mouse.x
                               } else if (mouseInArea === 4) {
                                   cutAreaState = 3
                                   Cutter.cutAreaRight = mouse.x
                                   Cutter.cutAreaBottom = mouse.y
                               } else if (mouseInArea === 5) {
                                   cutAreaState = 3
                                   Cutter.cutAreaBottom = mouse.y
                               } else if (mouseInArea === 6) {
                                   cutAreaState = 3
                                   Cutter.cutAreaLeft = mouse.x
                                   Cutter.cutAreaBottom = mouse.y
                               } else if (mouseInArea === 7) {
                                   cutAreaState = 3
                                   Cutter.cutAreaLeft = mouse.x
                               } else {
                                   cutAreaState = 4
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
                            if (cutAreaState === 1 || cutAreaState === 3
                                || cutAreaState === 4) {
                                cutAreaState = 2
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
                                if (cutAreaState === 2) {
                                    moveCutAreaByKey(0)
                                }
                            } else if (event.key === Qt.Key_Up) {
                                Cutter.moveMousePosition(1)
                                if (cutAreaState === 2) {
                                    moveCutAreaByKey(1)
                                }
                            } else if (event.key === Qt.Key_Right) {
                                Cutter.moveMousePosition(2)
                                if (cutAreaState === 2) {
                                    moveCutAreaByKey(2)
                                }
                            } else if (event.key === Qt.Key_Down) {
                                Cutter.moveMousePosition(3)
                                if (cutAreaState === 2) {
                                    moveCutAreaByKey(3)
                                }
                            }
                        }
    }
    DrawTool {
        id: drawTool
        visible: cutAreaState >= 2
        anchors.right: cutArea.right
        anchors.top: cutArea.bottom
        anchors.topMargin: 8
        anchors.rightMargin: 6
        onStartDraw: () => {
                         cutAreaState = 5
                         rootArea.cursorShape = Qt.CrossCursor
                     }
    }

    MouseTip {
        id: mouseTipRect
    }
    ShapeRect {
        id: shapeContainer
        visible: cutAreaState === 5
    }

    //  Component.onCompleted: {

    //  }
    //  sourceRectangle.grabToImage(function(result) {
    //             result.saveToFile("something.png")
    //          })
}
