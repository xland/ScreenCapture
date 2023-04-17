//拖拽创建截图区域
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

//修改截图区域大小
function resizeCutArea(mouse) {
    if (mouseInArea === 0) {
        Cutter.cutAreaLeft = mouse.x
        Cutter.cutAreaTop = mouse.y
    } else if (mouseInArea === 1) {
        Cutter.cutAreaTop = mouse.y
    } else if (mouseInArea === 2) {
        Cutter.cutAreaRight = mouse.x
        Cutter.cutAreaTop = mouse.y
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

//更新左上，右下坐标
function updatePoint() {
    if (Cutter.cutAreaLeft > Cutter.cutAreaRight) {
        let temp = Cutter.cutAreaRight
        Cutter.cutAreaRight = Cutter.cutAreaLeft
        Cutter.cutAreaLeft = temp
    }
    if (Cutter.cutAreaTop > Cutter.cutAreaBottom) {
        let temp = Cutter.cutAreaBottom
        Cutter.cutAreaBottom = Cutter.cutAreaTop
        Cutter.cutAreaTop = temp
    }
}

//拖拽移动截图区域
function moveCutAreaByDrag(mouse) {
    let spanX = mouse.x - mousePressX
    let spanY = mouse.y - mousePressY
    if (Cutter.cutAreaLeft + spanX < 0 && Cutter.cutAreaTop + spanY < 0) {
        //左上
        Cutter.cutAreaLeft = 0
        Cutter.cutAreaTop = 0
    } else if (Cutter.cutAreaLeft + spanX < 0) {
        Cutter.cutAreaLeft = 0
        if (Cutter.cutAreaBottom + spanY > Cutter.totalHeight) {
            //左下
            Cutter.cutAreaBottom = Cutter.totalHeight
        } else {
            //左
            Cutter.cutAreaTop += spanY
            Cutter.cutAreaBottom += spanY
        }
    } else if (Cutter.cutAreaTop + spanY < 0) {
        Cutter.cutAreaTop = 0
        if (Cutter.cutAreaRight + spanX > Cutter.totalWidth) {
            //右上
            Cutter.cutAreaRight = Cutter.totalWidth
        } else {
            //上
            Cutter.cutAreaLeft += spanX
            Cutter.cutAreaRight += spanX
        }
    } else if (Cutter.cutAreaRight + spanX > Cutter.totalWidth && Cutter.cutAreaBottom + spanY > Cutter.totalHeight) {
        //右下
        Cutter.cutAreaRight = Cutter.totalWidth
        Cutter.cutAreaBottom = Cutter.totalHeight
    } else if (Cutter.cutAreaRight + spanX > Cutter.totalWidth) {
        //右
        Cutter.cutAreaRight = Cutter.totalWidth
        Cutter.cutAreaTop += spanY
        Cutter.cutAreaBottom += spanY
    } else if (Cutter.cutAreaBottom + spanY > Cutter.totalHeight) {
        //下
        Cutter.cutAreaBottom = Cutter.totalHeight
        Cutter.cutAreaLeft += spanX
        Cutter.cutAreaRight += spanX
    } else {
        //其他区域
        Cutter.cutAreaTop += spanY
        Cutter.cutAreaLeft += spanX
        Cutter.cutAreaRight += spanX
        Cutter.cutAreaBottom += spanY
    }
    mousePressX = mouse.x
    mousePressY = mouse.y
}

//设置cursor样式，并记录鼠标所在区域
function setCursorByMousePosition(mouse) {
    if (mouse.x < Cutter.cutAreaLeft) {
        if (mouse.y < Cutter.cutAreaTop) {
            root.mouseInArea = 0 //左上
            root.cursorShape = Qt.SizeFDiagCursor
        } else if (mouse.y > Cutter.cutAreaBottom) {
            root.mouseInArea = 6 //左下
            root.cursorShape = Qt.SizeBDiagCursor
        } else {
            root.mouseInArea = 7 //左
            root.cursorShape = Qt.SizeHorCursor
        }
    } else if (mouse.x > Cutter.cutAreaRight) {
        if (mouse.y < Cutter.cutAreaTop) {
            root.mouseInArea = 2 //右上
            root.cursorShape = Qt.SizeBDiagCursor
        } else if (mouse.y > Cutter.cutAreaBottom) {
            root.mouseInArea = 4 //右下
            root.cursorShape = Qt.SizeFDiagCursor
        } else {
            root.mouseInArea = 3 //右
            root.cursorShape = Qt.SizeHorCursor
        }
    } else {
        if (mouse.y < Cutter.cutAreaTop) {
            root.mouseInArea = 1 //上
            root.cursorShape = Qt.SizeVerCursor
        } else if (mouse.y > Cutter.cutAreaBottom) {
            root.mouseInArea = 5 //下
            root.cursorShape = Qt.SizeVerCursor
        } else {
            root.mouseInArea = 8 //中
            root.cursorShape = Qt.SizeAllCursor
        }
    }
}

//设置cursor样式，并记录鼠标所在区域（绘图中）
function setCursorByMousePositionWhenDrawing(mouse) {
    let span = 4
    if (mouse.x < Cutter.cutAreaLeft && mouse.x > Cutter.cutAreaLeft - span && mouse.y > Cutter.cutAreaTop - span && mouse.y < Cutter.cutAreaTop) {
        root.mouseInArea = 0 //左上
        root.cursorShape = Qt.SizeFDiagCursor
    } else if (mouse.x < Cutter.cutAreaLeft && mouse.x > Cutter.cutAreaLeft - span && mouse.y > Cutter.cutAreaBottom && mouse.y < Cutter.cutAreaBottom + span) {
        root.mouseInArea = 6 //左下
        root.cursorShape = Qt.SizeBDiagCursor
    } else if (mouse.x < Cutter.cutAreaLeft && mouse.x > Cutter.cutAreaLeft - span && mouse.y > Cutter.cutAreaTop && mouse.y < Cutter.cutAreaBottom) {
        root.mouseInArea = 7 //左
        root.cursorShape = Qt.SizeHorCursor
    } else if (mouse.x < Cutter.cutAreaRight + span && mouse.x > Cutter.cutAreaRight && mouse.y > Cutter.cutAreaTop - span && mouse.y < Cutter.cutAreaTop) {
        root.mouseInArea = 2 //右上
        root.cursorShape = Qt.SizeBDiagCursor
    } else if (mouse.x < Cutter.cutAreaRight + span && mouse.x > Cutter.cutAreaRight && mouse.y > Cutter.cutAreaBottom && mouse.y < Cutter.cutAreaBottom + span) {
        root.mouseInArea = 4 //右下
        root.cursorShape = Qt.SizeFDiagCursor
    } else if (mouse.x < Cutter.cutAreaRight + span && mouse.x > Cutter.cutAreaRight && mouse.y > Cutter.cutAreaTop && mouse.y < Cutter.cutAreaBottom) {
        root.mouseInArea = 3 //右
        root.cursorShape = Qt.SizeHorCursor
    } else if (mouse.x > Cutter.cutAreaLeft && mouse.x < Cutter.cutAreaRight && mouse.y > Cutter.cutAreaTop - span && mouse.y < Cutter.cutAreaTop) {
        root.mouseInArea = 1 //上
        root.cursorShape = Qt.SizeVerCursor
    } else if (mouse.x > Cutter.cutAreaLeft && mouse.x < Cutter.cutAreaRight && mouse.y > Cutter.cutAreaBottom && mouse.y < Cutter.cutAreaBottom + span) {
        root.mouseInArea = 5 //下
        root.cursorShape = Qt.SizeVerCursor
    } else {
        root.mouseInArea = 9
        root.cursorShape = Qt.CrossCursor
    }
}
