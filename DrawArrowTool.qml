import QtQuick 2.15
import "Shapes.js" as Shapes

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
}
