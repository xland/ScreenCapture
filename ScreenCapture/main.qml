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
        x:0
        y:0
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
        focus: true
        onClicked: ()=>{
            root.close();
        }
        onPositionChanged:(mouse)=>{
            mouseTipRect.x = mouse.x+20;
            mouseTipRect.y = mouse.y+20;
            if(mouseTipRect.x + mouseTipRect.width > Cutter.totalWidth){
                mouseTipRect.x = mouse.x-20-mouseTipRect.width
            }
            if(mouseTipRect.y + mouseTipRect.height > Cutter.totalHeight){
                mouseTipRect.y = mouse.y-20-mouseTipRect.height
            }
            cursorImage.source = "image://ScreenImage/cursor?"+mouse.x+","+mouse.y;
            mouseTipRect.visible = true;
            Cutter.getHoveredWindowRect();
        }
        Keys.onPressed: (event) => {
            if (event.modifiers & Qt.AltModifier){
                if(event.key === Qt.Key_X){
                    Cutter.copyColor(true);
                }else if(event.key === Qt.Key_C){
                    Cutter.copyColor(false);
                }
            }
        }
    }
    Rectangle {
        id:mouseTipRect
        width:231.5
        height:234
        color:"#CC000000"
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
            text: "颜色："+Cutter.colorRgb+"  "+Cutter.colorHex
            color: "#ffffff"
        }
        Text {
            id:mouseTipText3
            anchors.top: mouseTipText2.bottom
            anchors.topMargin: 6
            anchors.left: parent.left
            anchors.leftMargin: 8
            verticalAlignment: Text.AlignVCenter
            text: "复制：rgb: Alt+X  hex: Alt+C"
            color: "#cccccc"
        }
    }
    Component.onCompleted: {
        console.log(Cutter.absoluteX ,Cutter.absoluteY,Cutter.totalWidth,Cutter.totalHeight);
    }
}
