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
    flags: Qt.FramelessWindowHint|Qt.WindowStaysOnTopHint
    property string maskColor: "#88000000"
    property string crossLineColor: "#330958d9"
    Image {
        id: backgroundImage
        source: "image://ScreenImage/background"
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
            cursorImage.source = "image://ScreenImage/cursor?"+mouse.x+","+mouse.y;
            mouseTipRect.visible = true;
            Cutter.getHoveredWindowRect();
        }
    }
    Rectangle {
        id:mouseTipRect
        x:0
        y:0
        width:232
        height:310
        color:"#AA000000"
        visible: false
        border.width: 1
        border.color:"#000000"
        Image {
            id: cursorImage
            x:1
            y:1
            Rectangle{
                x:0
                y:75
                height: 10
                width:110
                color:crossLineColor
            }
            Rectangle{
                x:120
                y:75
                height: 10
                width:110
                color:crossLineColor
            }
            Rectangle{
                x:110
                y:0
                height: 75
                width:10
                color:crossLineColor
            }
            Rectangle{
                x:110
                y:85
                height: 75
                width:10
                color:crossLineColor
            }
        }
        Text {
            id:mouseTipText1
            anchors.top: cursorImage.bottom
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 8
            text: "坐标：x: "+Cutter.mouseX+"  y: "+Cutter.mouseY
            color: "#ffffff"
            verticalAlignment: Text.AlignVCenter
        }
        Text {
            id:mouseTipText2
            anchors.top: mouseTipText1.bottom
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 8
            verticalAlignment: Text.AlignVCenter
            text: "RGB："+Cutter.colorRgb
            color: "#ffffff"
        }
        Text {
            id:mouseTipText3
            anchors.top: mouseTipText2.bottom
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 8
            verticalAlignment: Text.AlignVCenter
            text: "HEX："+Cutter.colorHex
            color: "#ffffff"
        }
        Text {
            id:mouseTipText4
            anchors.top: mouseTipText3.bottom
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 8
            font.pointSize: 7
            verticalAlignment: Text.AlignVCenter
            font.italic:true
            text: "Alt+X Copy RGB Color"
            color: "#dddddd"
        }
        Text {
            id:mouseTipText5
            anchors.top: mouseTipText4.bottom
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 8
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 7
            font.italic:true
            text: "Alt+C Copy HEX Color"
            color: "#dddddd"
        }
    }
//    Component.onCompleted: {
//        console.log(Cutter.cutAreaLeft ,Cutter.cutAreaTop,Cutter.cutAreaRight,Cutter.cutAreaBottom);
//    }
}
