import QtQuick

Rectangle {
  id: root
  property alias code: iconText.text
  property bool isSelected: false
  signal beforeSelected
  function unSelect() {
    if (root.isSelected) {
      root.isSelected = false
      root.color = "#F8F8F8"
      iconText.color = "#555555"
    }
  }
  width: 36
  height: 30
  color: "#F8F8F8"
  Text {
    id: iconText
    font.family: "iconfont"
    font.pixelSize: 16
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
                   root.isSelected = false
                   root.color = "#F8F8F8"
                   iconText.color = "#555555"
                 } else {
                   root.beforeSelected()
                   root.isSelected = true
                   root.color = "#1677ff"
                   iconText.color = "#FFFFFF"
                 }
               }
  }
  Component.onCompleted: {
    if (root.isSelected) {
      root.color = "#1677ff"
      iconText.color = "#FFFFFF"
    }
  }
}
