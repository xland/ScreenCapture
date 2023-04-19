import QtQuick 2.15
import "Shapes.js" as Shapes
import ScreenCapture.Cutter 1.0

Rectangle {
    id: root
    height: 30
    color: "#F8F8F8"
    width: 100
    visible: App.drawToolState === 3 && App.cutAreaState === 2
    DrawBtn {
        id: dot
        code: "\ue608"
        isSelected: true
        onClick: () => {
                     App.rectCircle.isFill = !App.rectCircle.isFill
                     App.rectCircle.bgColor = "transparent"
                     if (Shapes.shapes.length > 0 && Shapes.shapes[0].showDragger) {
                         Shapes.shapes[0].bgColor = "transparent"
                     }
                 }
    }
    ColorPicker {
        anchors.left: dot.right
        anchors.top: dot.top
    }
}
