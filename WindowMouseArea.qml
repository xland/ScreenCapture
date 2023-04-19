import QtQuick 2.15
import ScreenCapture.Cutter 1.0
import "CutArea.js" as CutArea
import "MouseTip.js" as MouseTip
import "Shapes.js" as Shapes

MouseArea {
    property int mouseInArea: -1 //0左上,1上,2右上,3右,4右下,5下,6左下,7左,8中,9绘图区域
    property real mousePressX: 0
    property real mousePressY: 0
    function createComponent(name, obj) {
        var result
        var component = Qt.createComponent(name + ".qml")
        if (component.status === Component.Ready) {
            result = component.createObject(root, obj)
            if (result === null) {
                console.log("Error creating object")
            }
        } else if (component.status === Component.Error) {
            console.log("Error loading component:", component.errorString())
        }
        return result
    }

    function createShape(mouse) {
        if (App.drawToolState === 1) {
            let config = {
                "startX1": mouse.x,
                "startY1": mouse.y,
                "endX": mouse.x,
                "endY": mouse.y,
                "isRect": App.rectCircle.isRect,
                "isFill": App.rectCircle.isFill,
                "bgColor": App.rectCircle.bgColor,
                "borderWidth": App.rectCircle.borderWidth,
                "borderColor": App.rectCircle.borderColor
            }
            let shape = createComponent("ShapeRect", config)
            Shapes.shapes.splice(0, 0, shape)
        } else if (App.drawToolState === 2) {
            let config = {
                "isFill": App.arrow.isFill,
                "bgColor": App.arrow.bgColor,
                "borderWidth": App.arrow.borderWidth,
                "borderColor": App.arrow.borderColor
            }
            let shape = createComponent("ShapeArrow", config)
            Shapes.shapes.splice(0, 0, shape)
        } else if (App.drawToolState === 3) {
            let config = {
                "lastX": mousePressX,
                "lastY": mousePressY,
                "curX": mousePressX,
                "curY": mousePressY
            }
            console.log(JSON.stringify(config))
            let shape = createComponent("CanvasPen", config)
            Shapes.shapes.splice(0, 0, shape)
        }
    }
    id: root
    anchors.fill: parent
    visible: true //todo 绘图状态时要控制这里不显示
    acceptedButtons: Qt.LeftButton | Qt.RightButton
    hoverEnabled: true
    cursorShape: Qt.CrossCursor
    onPositionChanged: mouse => {
                           if (App.drawToolState === 0) {
                               if (App.cutAreaState === 0) {
                                   if (mouse.buttons === Qt.NoButton) {
                                       //高亮窗口区域
                                       Cutter.createCutAreaByWindowRect()
                                   }
                               } else if (App.cutAreaState === 1) {
                                   //todo 出现大小控制工具
                                   if (mouse.buttons === Qt.LeftButton) {
                                       CutArea.createCutAreaByDrag(mouse)
                                   }
                               } else if (App.cutAreaState === 2) {
                                   //设置Cursor
                                   if (mouse.buttons === Qt.NoButton) {
                                       CutArea.setCursorByMousePosition(mouse)
                                   }
                               } else if (App.cutAreaState === 3) {
                                   //改变大小
                                   if (mouse.buttons === Qt.LeftButton) {
                                       CutArea.resizeCutArea(mouse)
                                   }
                               } else if (App.cutAreaState === 4) {
                                   if (mouse.buttons === Qt.LeftButton) {
                                       //移动截图区域
                                       CutArea.moveCutAreaByDrag(mouse)
                                   }
                               }
                               //放大镜
                               MouseTip.setMouseTip(mouse)
                           } else {
                               root.cursorShape = Qt.CrossCursor
                               if (mouse.buttons === Qt.LeftButton) {
                                   if (App.drawToolState === 1) {
                                       Shapes.shapes[0].endX = mouse.x
                                       Shapes.shapes[0].endY = mouse.y
                                   } else if (App.drawToolState === 2) {
                                       let start = Qt.point(root.mousePressX, root.mousePressY)
                                       let end = Qt.point(mouse.x, mouse.y)
                                       Shapes.shapes[0].pathPointChanged(start, end)
                                   } else if (App.drawToolState === 3) {
                                       Shapes.shapes[0].color = "red"
                                       Shapes.shapes[0].startDraw(mouse.x, mouse.y)
                                   }
                               }
                           }
                       }
    onPressed: mouse => {
                   if (mouse.buttons === Qt.LeftButton) {
                       root.mousePressX = mouse.x
                       root.mousePressY = mouse.y
                       if (App.cutAreaState === 0) {
                           App.cutAreaState = 1
                       } else if (App.cutAreaState === 2) {
                           if (App.drawToolState === 0) {
                               if (mouseInArea != 8 && mouseInArea != -1) {
                                   //改变大小
                                   App.cutAreaState = 3
                                   CutArea.setCursorByMousePosition(mouse)
                                   CutArea.resizeCutArea(mouse)
                               } else {
                                   //移动位置
                                   App.cutAreaState = 4
                               }
                           } else {
                               if (App.drawToolState === 3 && Shapes.shapes[0] && Shapes.shapes[0].drawing) {
                                   Shapes.shapes[0].lastX = mouse.x
                                   Shapes.shapes[0].lastY = mouse.y
                               } else {
                                   Shapes.done()
                                   createShape(mouse)
                               }
                           }
                       }
                   } else {
                       Shapes.done()
                   }
               }
    onReleased: mouse => {
                    if (mouse.button === Qt.LeftButton) {
                        if (App.cutAreaState === 3) {
                            App.cutAreaState = 2
                            CutArea.updatePoint()
                        } else if (App.cutAreaState === 1 || App.cutAreaState === 4) {
                            App.cutAreaState = 2
                        }
                        if (App.drawToolState === 1) {
                            Shapes.shapes[0].showDragger = true
                        } else if (App.drawToolState === 2) {
                            Shapes.shapes[0].showDragger = true
                        } else if (App.drawToolState === 3) {

                        }
                    }
                }
}
