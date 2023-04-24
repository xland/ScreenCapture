import QtQuick 2.15
import "Shapes.js" as Shapes
import ScreenCapture.Cutter 1.0

Rectangle {
    id: root
    height: 30
    color: "#F8F8F8"
    width: 70
    visible: App.drawToolState === 6 && App.cutAreaState === 2
    DrawBtn {
        id: dot
        code: "\ue608"
        isSelected: !App.eraser.isRect
        onClick: () => {
                     App.eraser.isRect = !App.eraser.isRect
                 }
    }
    DrawBtn {
        id: rect
        code: "\ueaa2"
        isSelected: App.eraser.isRect
        anchors.left: dot.right
        anchors.top: dot.top
        onClick: () => {
                     App.eraser.isRect = !App.eraser.isRect
                 }
    }
}
