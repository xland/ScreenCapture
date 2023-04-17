import QtQuick 2.15

Rectangle {
    id: root
    property alias code: iconText.text
    property bool isSelected: false
    signal click
    width: 36
    height: 30
    color: "#F8F8F8"
    Text {
        id: iconText
        font.family: "iconfont"
        font.pixelSize: 16
        antialiasing: true
        color: "#555555"
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor
        onEntered: () => {
                       if (root.isSelected) {
                           return
                       }
                       root.color = "#E0E0E0"
                   }
        onExited: () => {
                      if (root.isSelected) {
                          return
                      }
                      root.color = "#F8F8F8"
                  }
        onClicked: () => {
                       if (root.isSelected) {
                           root.color = "#F8F8F8"
                           iconText.color = "#555555"
                       } else {
                           root.color = "#1677ff"
                           iconText.color = "#FFFFFF"
                       }
                       click()
                   }
    }
    onIsSelectedChanged: (val1, val2) => {
                             if (!root.isSelected) {
                                 root.color = "#F8F8F8"
                                 iconText.color = "#555"
                             }
                         }

    Component.onCompleted: {
        if (root.isSelected) {
            root.color = "#1677ff"
            iconText.color = "#FFFFFF"
        }
    }
}
