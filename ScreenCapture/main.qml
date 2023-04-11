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
    property string maskColor: "#88000000"
    Image {
        id: backgroundImage
        source: "image://ScreenImage"
        sourceSize.width: root.width
        sourceSize.height: root.height
    }
//    Rectangle{
//        anchors.fill: parent
//        color:maskColor
//    }

    Rectangle {
        id:leftRect
        x:0
        y:0
        height: parent.height
        width: Cutter.cutAreaLeft
        color:maskColor
    }
    Rectangle {
        id:topRect
        x:Cutter.cutAreaLeft
        y:0
        height:Cutter.cutAreaTop
        width: parent.width - Cutter.cutAreaLeft
        color:maskColor
    }
    Rectangle {
        id:rightRect
        x:Cutter.cutAreaRight
        y:Cutter.cutAreaTop
        height:parent.height - Cutter.cutAreaTop
        width: parent.width - Cutter.cutAreaRight
        color:maskColor
    }
    Rectangle {
        id:bottomRect
        x:Cutter.cutAreaLeft
        y:Cutter.cutAreaBottom
        height:parent.height - Cutter.cutAreaBottom
        width: Cutter.cutAreaRight-Cutter.cutAreaLeft
        color:maskColor
    }
    MouseArea{
        id:rootArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled:true
        onClicked: {
            root.close();
        }
        onPositionChanged:{
            mouseTipRect.x = mouse.x+30;
            mouseTipRect.y = mouse.y+30;
            mouseTipRect.visible = true;
            Cutter.getHoveredWindowRect();
        }
    }
    Rectangle {
        id:mouseTipRect
        x:0
        y:0
        width:400
        height:600
        color:"#66000000"
        visible: false
    }
//    Component.onCompleted: {
//        console.log(Cutter.cutAreaLeft ,Cutter.cutAreaTop,Cutter.cutAreaRight,Cutter.cutAreaBottom);
//    }
}
