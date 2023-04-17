//设置放大镜窗口
function setMouseTip(mouse) {
    if (App.cutAreaState === 0 || (App.cutAreaState === 2 && mouseInArea === 8)) {
        App.mouseTip.x = mouse.x + 20
        App.mouseTip.y = mouse.y + 20
        if (App.mouseTip.x + App.mouseTip.width > Cutter.totalWidth) {
            App.mouseTip.x = mouse.x - 20 - App.mouseTip.width
        }
        if (App.mouseTip.y + App.mouseTip.height > Cutter.totalHeight) {
            App.mouseTip.y = mouse.y - 20 - App.mouseTip.height
        }
        App.mouseTip.src = "image://ScreenImage/cursor?" + mouse.x + "," + mouse.y
        App.mouseTip.visible = true //todo 鼠标移动时，会忽闪一下
    } else {
        App.mouseTip.visible = false
    }
}
