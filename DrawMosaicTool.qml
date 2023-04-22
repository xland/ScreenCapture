import QtQuick 2.15
import "Shapes.js" as Shapes
import ScreenCapture.Cutter 1.0

Rectangle {
    id: root
    height: 30
    color: "#F8F8F8"
    width: 100
    visible: App.drawToolState === 4 && App.cutAreaState === 2
    DrawBtn {
        id: dot
        code: "\ue608"
        isSelected: false
        onClick: () => {
                     App.mosaic.isRect = !App.mosaic.isRect
                 }
    }
    DrawBtn {
        id: rect
        code: "\ueaa2"
        isSelected: true
        anchors.left: dot.right
        anchors.top: dot.top
        onClick: () => {
                     App.mosaic.isRect = !App.mosaic.isRect
                 }
    }
    ColorPicker {
        anchors.left: rect.right
        anchors.top: rect.top
    }
}
