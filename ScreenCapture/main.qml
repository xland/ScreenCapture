import QtQuick
import QtQuick.Window
import QtQuick.Controls
import ScreenCapture.Cutter 1.0

ApplicationWindow {
    id:root
    visible: true
    x:Cutter.absoluteX
    y:Cutter.absoluteY
    width:Cutter.totalWidth
    height: Cutter.totalHeight
    flags: Qt.FramelessWindowHint
    property string borderColor: "#88000000"
    Image {
        id: backgroundImage
        source: "image://ScreenImage"
        sourceSize.width: root.width
        sourceSize.height: root.height
    }
    Rectangle {
        id:leftRect
        anchors.left: parent.left
        anchors.top:parent.top
        anchors.bottom: parent.bottom
        width: 100
        color:borderColor
        visible: false
    }
    Rectangle {
        id:topRect
        anchors.left: leftRect.right
        anchors.top:parent.top
        anchors.right: parent.right
        height: 100
        color:borderColor
        visible: false
    }
    Rectangle {
        id:rightRect
        anchors.right: parent.right
        anchors.top:topRect.bottom
        anchors.bottom: parent.bottom
        width: 100
        color:borderColor
        visible: false
    }
    Rectangle {
        id:bottomRect
        anchors.left: leftRect.right
        anchors.right: rightRect.left
        anchors.bottom: parent.bottom
        height: 100
        color:borderColor
        visible: false
    }
    MouseArea{
        id:rootArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: {
            root.close();
        }
//        onPress: {

//        }
    }
}
