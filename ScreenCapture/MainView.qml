import QtQuick 2.15

Rectangle
{
    id: root
    color: "#dd88ff"
    Image {
        id: backgroundImage
        source: "image://ScreenImage"
        sourceSize.width: root.width
        sourceSize.height: root.height
    }
}

