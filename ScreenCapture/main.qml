import QtQuick
import QtQuick.Window
import QtQuick.Controls

ApplicationWindow {
    id:root
    visible: true
    x:absoluteX
    y:absoluteY
    width:totalWidth
    height: totalHeight
    flags: Qt.FramelessWindowHint
    Image {
        id: backgroundImage
        source: "image://ScreenImage"
        sourceSize.width: root.width
        sourceSize.height: root.height
    }
    MouseArea{
        id:rootArea
        anchors.fill: parent
        onClicked: {
            root.close();
        }
    }
}
