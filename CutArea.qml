import QtQuick 2.15
import QtQuick.Shapes 1.15
import ScreenCapture.Cutter 1.0

Shape {
    id: root
    anchors.fill: parent
    ShapePath {
        id: path
        strokeWidth: 2
        strokeColor: "#4096ff"
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
    MouseArea {
        id: mouseArea
        property int mouseInArea: -1 //0左上,1上,2右上,3右,4右下,5下,6左下,7左,8中
        property int mousePressX: 0
        property int mousePressY: 0
        function setMouseArea(mouse) {
            if (mouse.x < Cutter.cutAreaLeft) {
                if (mouse.y < Cutter.cutAreaTop) {
                    mouseArea.mouseInArea = 0 //左上
                    mouseArea.cursorShape = Qt.SizeFDiagCursor
                } else if (mouse.y > Cutter.cutAreaBottom) {
                    mouseArea.mouseInArea = 6 //左下
                    mouseArea.cursorShape = Qt.SizeBDiagCursor
                } else {
                    mouseArea.mouseInArea = 7 //左
                    mouseArea.cursorShape = Qt.SizeHorCursor
                }
            } else if (mouse.x > Cutter.cutAreaRight - span) {
                if (mouse.y < Cutter.cutAreaTop) {
                    mouseArea.mouseInArea = 2 //右上
                    mouseArea.cursorShape = Qt.SizeBDiagCursor
                } else if (mouse.y > Cutter.cutAreaBottom) {
                    mouseArea.mouseInArea = 4 //右下
                    mouseArea.cursorShape = Qt.SizeFDiagCursor
                } else {
                    mouseArea.mouseInArea = 3 //右
                    mouseArea.cursorShape = Qt.SizeHorCursor
                }
            } else {
                if (mouse.y < Cutter.cutAreaTop + span) {
                    mouseArea.mouseInArea = 1 //上
                    mouseArea.cursorShape = Qt.SizeVerCursor
                } else if (mouse.y > Cutter.cutAreaBottom - span) {
                    mouseArea.mouseInArea = 5 //下
                    mouseArea.cursorShape = Qt.SizeVerCursor
                } else {
                    mouseArea.mouseInArea = 8 //中
                    mouseArea.cursorShape = Qt.SizeAllCursor
                }
            }
        }
        anchors.fill: parent
        visible: true //todo 绘图状态时要控制这里不显示
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true
        cursorShape: Qt.CrossCursor
        onPositionChanged: mouse => {
                               if (App.cutAreaState === 0) {
                                   if (mouse.buttons === Qt.NoButton) {
                                       //高亮窗口区域
                                       Cutter.createCutAreaByWindowRect()
                                   }
                               } else if (App.cutAreaState === 1) {
                                   if (mouse.buttons === Qt.LeftButton) {

                                       //                                       createCutAreaByDrag(mouse)
                                   }
                               } else if (App.cutAreaState === 2) {
                                   if (mouse.buttons === Qt.NoButton) {
                                       setMouseArea(mouse)
                                   }
                               } else if (App.cutAreaState === 3) {
                                   if (mouse.buttons === Qt.LeftButton) {

                                       //                                       resizeCutArea(mouse)
                                   }
                               } else if (App.cutAreaState === 4) {
                                   if (mouse.buttons === Qt.LeftButton) {
                                       moveCutAreaByMouse(mouse)
                                   }
                               }
                               //                               setMouseTip(mouse)
                           }
        onPressed: mouse => {
                       if (mouse.buttons === Qt.LeftButton) {
                           if (App.cutAreaState === 0) {
                               App.cutAreaState = 1
                               mouseArea.mousePressX = mouse.x
                               mouseArea.mousePressY = mouse.y
                           }
                       }
                   }
        onReleased: mouse => {
                        if (mouse.button === Qt.LeftButton) {
                            if (App.cutAreaState === 1 || App.cutAreaState === 3 || App.cutAreaState === 4) {
                                App.cutAreaState = 2
                            }
                        }
                    }
    }

    //    Component.onCompleted: {
    //        console.log(Cutter.cutAreaLeft, Cutter.cutAreaTop, Cutter.cutAreaRight,
    //                    Cutter.cutAreaBottom)
    //    }
}
