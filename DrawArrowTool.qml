import QtQuick 2.15
import "Shapes.js" as Shapes
import ScreenCapture.Cutter 1.0

Rectangle {
    id: root
    height: 30
    color: "#F8F8F8"
    width: 100
    visible: App.drawToolState === 2 && App.cutAreaState === 2
    DrawBtn {
        id: empty
        code: "\ue61a"
        isSelected: !App.arrow.isFill
        onClick: () => {
                     App.arrow.isFill = false
                     App.arrow.bgColor = "transparent"
                     if (Shapes.instance && Shapes.instance.showDragger) {
                         Shapes.instance.bgColor = "transparent"
                     }
                 }
    }
    DrawBtn {
        id: fill
        code: "\ue62c"
        anchors.left: empty.right
        anchors.top: empty.top
        isSelected: App.arrow.isFill
        onClick: () => {
                     App.arrow.isFill = true
                     App.arrow.bgColor = "red"
                     if (Shapes.instance && Shapes.instance.showDragger) {
                         Shapes.instance.bgColor = "red"
                     }
                 }
    }
    ColorPicker {
        anchors.left: fill.right
        anchors.top: fill.top
    }
}
