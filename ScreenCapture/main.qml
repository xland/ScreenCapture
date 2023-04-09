import QtQuick
import QtQuick.Window
import QtQuick.Controls

ApplicationWindow {
    visible: true
//    x:-100
//    y:-1000
//    width:totalWidth
//    height: totalHeight
    flags: Qt.FramelessWindowHint
//    visibility: "FullScreen"
    Image {
        id: backgroundImage
        source: "image://ScreenImage/1"
    }
}
