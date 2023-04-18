import QtQuick 2.15
import "Shapes.js" as Shapes
import ScreenCapture.Cutter 1.0

Rectangle {
    id: root
    height: 30
    color: "#F8F8F8"
    width: 100
    visible: App.drawToolState === 2 && App.cutAreaState === 2
    ColorPicker {
        anchors.left: root.left
        anchors.top: root.top
    }
    Component.onCompleted: () => {
                               let arr = Cutter.getArrowPoints(Qt.point(200, 200), Qt.point(600, 100), 10, 12)
                               console.log(JSON.stringify(arr))
                           }
}
