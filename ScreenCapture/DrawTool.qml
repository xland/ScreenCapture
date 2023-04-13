import QtQuick

Rectangle {
  width: 360
  height: 30
  FontLoader {
    source: "font.ttf"
  }
  component DrawBtn: Rectangle {
    width: 36
    height: 30
    color: "#F8F8F8"
    property alias code: iconText.text
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
                   parent.color = "#E0E0E0"
                 }
      onExited: () => {
                  parent.color = "#F8F8F8"
                }
    }
  }
  DrawBtn {
    id: rectBtn
    code: "\ue86d"
    anchors.left: parent.left
  }
  DrawBtn {
    id: arrawBtn
    code: "\ueb0c"
    anchors.left: rectBtn.right
  }
  DrawBtn {
    id: penBtn
    code: "\ue601"
    anchors.left: arrawBtn.right
  }
  DrawBtn {
    id: mosaicBtn
    code: "\ue82e"
    anchors.left: penBtn.right
  }
  DrawBtn {
    id: textBtn
    code: "\ue6ec"
    anchors.left: mosaicBtn.right
  }
  DrawBtn {
    id: eraserBtn
    code: "\ue6b8"
    anchors.left: textBtn.right
  }
  DrawBtn {
    id: undoBtn
    code: "\ued85"
    anchors.left: eraserBtn.right
  }
  DrawBtn {
    id: redoBtn
    code: "\ued8a"
    anchors.left: undoBtn.right
  }
  DrawBtn {
    id: clipboardBtrn
    code: "\ue87f"
    anchors.left: redoBtn.right
  }
  DrawBtn {
    id: saveBtn
    code: "\ue62f"
    anchors.left: clipboardBtrn.right
  }
}
