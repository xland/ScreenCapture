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
    flags: Qt.FramelessWindowHint
    Image {
        x: 0
        y: 0
        id: backgroundImage
        source: "image://ScreenImage/background"
        sourceSize.width: root.width
        sourceSize.height: root.height
    }

    //    ShapeRect {
    //        id: shapeContainer
    //        visible: cutAreaState === 5
    //    }
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
    //    function setMouseTip(mouse) {
    //        if (cutAreaState === 0 || (cutAreaState === 2 && mouseInArea === 8)) {
    //            mouseTipRect.x = mouse.x + 20
    //            mouseTipRect.y = mouse.y + 20
    //            if (mouseTipRect.x + mouseTipRect.width > Cutter.totalWidth) {
    //                mouseTipRect.x = mouse.x - 20 - mouseTipRect.width
    //            }
    //            if (mouseTipRect.y + mouseTipRect.height > Cutter.totalHeight) {
    //                mouseTipRect.y = mouse.y - 20 - mouseTipRect.height
    //            }
    //            mouseTipRect.imageSource = "image://ScreenImage/cursor?" + mouse.x + "," + mouse.y
    //            mouseTipRect.visible = true //todo 鼠标移动时，会忽闪一下
    //        } else {
    //            mouseTipRect.visible = false
    //        }
    //    }
    //    Component.onCompleted: {
    //        console.log("main", App.cutAreaState)
    //    }
    //    Rectangle {
    //        id: leftRect
    //        x: 0
    //        y: 0
    //        height: parent.height
    //        width: Cutter.cutAreaLeft
    //        color: maskColor
    //    }
    //    Rectangle {
    //        id: topRect
    //        x: Cutter.cutAreaLeft
    //        y: 0
    //        height: Cutter.cutAreaTop
    //        width: parent.width - Cutter.cutAreaLeft
    //        color: maskColor
    //    }
    //    Rectangle {
    //        id: rightRect
    //        x: Cutter.cutAreaRight
    //        y: Cutter.cutAreaTop
    //        height: parent.height - Cutter.cutAreaTop
    //        width: parent.width - Cutter.cutAreaRight
    //        color: maskColor
    //    }
    //    Rectangle {
    //        id: bottomRect
    //        x: Cutter.cutAreaLeft
    //        y: Cutter.cutAreaBottom
    //        height: parent.height - Cutter.cutAreaBottom
    //        width: Cutter.cutAreaRight - Cutter.cutAreaLeft
    //        color: maskColor
    //    }
    //    MouseArea {
    //        id: rootArea
    //        visible: false
    //        anchors.fill: parent
    //        acceptedButtons: Qt.LeftButton | Qt.RightButton
    //        hoverEnabled: true
    //        focus: true
    //        cursorShape: Qt.CrossCursor
    //        onPositionChanged: mouse => {
    //                               if (cutAreaState === 0) {
    //                                   if (mouse.buttons === Qt.NoButton) {
    //                                       Cutter.createCutAreaByWindowRect()
    //                                   }
    //                               } else if (cutAreaState === 1) {
    //                                   if (mouse.buttons === Qt.LeftButton) {
    //                                       createCutAreaByDrag(mouse)
    //                                   }
    //                               } else if (cutAreaState === 2) {
    //                                   if (mouse.buttons === Qt.NoButton) {
    //                                       setMouseArea(mouse)
    //                                   }
    //                               } else if (cutAreaState === 3) {
    //                                   if (mouse.buttons === Qt.LeftButton) {

    //                                       //                                       resizeCutArea(mouse)
    //                                   }
    //                               } else if (cutAreaState === 4) {
    //                                   if (mouse.buttons === Qt.LeftButton) {
    //                                       moveCutAreaByMouse(mouse)
    //                                   }
    //                               }
    //                               setMouseTip(mouse)
    //                           }
    //        onPressed: mouse => {
    //                       if (mouse.button !== Qt.LeftButton) {
    //                           if (cutAreaState !== 0) {
    //                               cutAreaState = 0
    //                               cursorShape = Qt.CrossCursor
    //                               Cutter.createCutAreaByWindowRect()
    //                           } else {
    //                               root.close()
    //                           }
    //                       } else {
    //                           if (cutAreaState === 0) {
    //                               cutAreaState = 1
    //                               mousePressX = mouse.x
    //                               mousePressY = mouse.y
    //                           } else if (cutAreaState === 2) {
    //                               if (mouseInArea === 0) {
    //                                   cutAreaState = 3
    //                                   Cutter.cutAreaTop = mouse.y
    //                                   Cutter.cutAreaLeft = mouse.x
    //                               } else if (mouseInArea === 1) {
    //                                   cutAreaState = 3
    //                                   Cutter.cutAreaTop = mouse.y
    //                               } else if (mouseInArea === 2) {
    //                                   cutAreaState = 3
    //                                   Cutter.cutAreaTop = mouse.y
    //                                   Cutter.cutAreaRight = mouse.x
    //                               } else if (mouseInArea === 3) {
    //                                   cutAreaState = 3
    //                                   Cutter.cutAreaRight = mouse.x
    //                               } else if (mouseInArea === 4) {
    //                                   cutAreaState = 3
    //                                   Cutter.cutAreaRight = mouse.x
    //                                   Cutter.cutAreaBottom = mouse.y
    //                               } else if (mouseInArea === 5) {
    //                                   cutAreaState = 3
    //                                   Cutter.cutAreaBottom = mouse.y
    //                               } else if (mouseInArea === 6) {
    //                                   cutAreaState = 3
    //                                   Cutter.cutAreaLeft = mouse.x
    //                                   Cutter.cutAreaBottom = mouse.y
    //                               } else if (mouseInArea === 7) {
    //                                   cutAreaState = 3
    //                                   Cutter.cutAreaLeft = mouse.x
    //                               } else {
    //                                   cutAreaState = 4
    //                                   mousePressX = mouse.x - Cutter.cutAreaLeft
    //                                   mousePressY = mouse.y - Cutter.cutAreaTop
    //                                   heightForMoving = cutArea.height
    //                                   widthForMoving = cutArea.width
    //                               }
    //                           }
    //                       }
    //                   }
    //        onReleased: mouse => {
    //                        if (mouse.button === Qt.LeftButton) {
    //                            if (cutAreaState === 1 || cutAreaState === 3 || cutAreaState === 4) {
    //                                cutAreaState = 2
    //                            }
    //                        }
    //                    }
    //        Keys.onPressed: event => {
    //                            if (event.modifiers & Qt.AltModifier) {
    //                                if (event.key === Qt.Key_X) {
    //                                    Cutter.copyColor(true)
    //                                } else if (event.key === Qt.Key_C) {
    //                                    Cutter.copyColor(false)
    //                                }
    //                            } else if (event.key === Qt.Key_Escape) {
    //                                root.flags = Qt.FramelessWindowHint
    //                                Cutter.askForQuit()
    //                                root.flags = Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    //                            } else if (event.key === Qt.Key_Left) {
    //                                Cutter.moveMousePosition(0)
    //                                if (cutAreaState === 2) {
    //                                    moveCutAreaByKey(0)
    //                                }
    //                            } else if (event.key === Qt.Key_Up) {
    //                                Cutter.moveMousePosition(1)
    //                                if (cutAreaState === 2) {
    //                                    moveCutAreaByKey(1)
    //                                }
    //                            } else if (event.key === Qt.Key_Right) {
    //                                Cutter.moveMousePosition(2)
    //                                if (cutAreaState === 2) {
    //                                    moveCutAreaByKey(2)
    //                                }
    //                            } else if (event.key === Qt.Key_Down) {
    //                                Cutter.moveMousePosition(3)
    //                                if (cutAreaState === 2) {
    //                                    moveCutAreaByKey(3)
    //                                }
    //                            }
    //                        }
    //    }

    //  Component.onCompleted: {

    //  }
    //  sourceRectangle.grabToImage(function(result) {
    //             result.saveToFile("something.png")
    //          })
}
