import QtQuick

Rectangle {
  FontLoader {
    source: "font.ttf"
  }
  property int selectedIndex: -1
  function selecteBase() {}
  component DrawBtn: Rectangle {
    width: 36
    height: 30
    color: "#F8F8F8"
    property alias code: iconText.text
    property alias codeColor: iconText.color
    property int index: -1
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
                   if (selectedIndex === index) {
                     return
                   }
                   parent.color = "#E0E0E0"
                 }
      onExited: () => {
                  if (selectedIndex === index) {
                    return
                  }
                  parent.color = "#F8F8F8"
                }
      onClicked: () => {
                   if (selectedIndex != -1) {
                     root.children[selectedIndex].color = "#F8F8F8"
                     root.children[selectedIndex].codeColor = "#555555"
                   }
                   if (selectedIndex === index) {
                     selectedIndex = -1
                   } else {
                     selectedIndex = index
                     parent.color = "#1677ff"
                     parent.codeColor = "#FFFFFF"
                     root.height = 66
                   }
                 }
    }
  }
  id: root
  width: 360
  height: 30
  color: "transparent"
  DrawBtn {
    id: rectBtn
    code: "\ue86d"
    index: 0
    anchors.left: parent.left
  }
  DrawBtn {
    id: arrawBtn
    code: "\ueb0c"
    index: 1
    anchors.left: rectBtn.right
  }
  DrawBtn {
    id: penBtn
    code: "\ue601"
    index: 2
    anchors.left: arrawBtn.right
  }
  DrawBtn {
    id: mosaicBtn
    code: "\ue82e"
    index: 3
    anchors.left: penBtn.right
  }
  DrawBtn {
    id: textBtn
    code: "\ue6ec"
    index: 4
    anchors.left: mosaicBtn.right
  }
  DrawBtn {
    id: eraserBtn
    code: "\ue6b8"
    index: 5
    anchors.left: textBtn.right
  }
  DrawBtn {
    id: undoBtn
    code: "\ued85"
    index: 6
    anchors.left: eraserBtn.right
  }
  DrawBtn {
    id: redoBtn
    code: "\ued8a"
    index: 7
    anchors.left: undoBtn.right
  }
  DrawBtn {
    id: clipboardBtrn
    code: "\ue87f"
    index: 8
    anchors.left: redoBtn.right
  }
  DrawBtn {
    id: saveBtn
    code: "\ue62f"
    index: 9
    anchors.left: clipboardBtrn.right
  }

  //开始新的一行
  DrawBtn {
    id: emptyRect
    code: "\ue790"
    anchors.left: parent.left
    anchors.top: rectBtn.bottom
    anchors.topMargin: 3
  }
  DrawBtn {
    id: fillRect
    code: "\ueaa2"
    anchors.left: emptyRect.right
    anchors.top: emptyRect.top
  }
  DrawBtn {
    id: emptyCircle
    code: "\ue610"
    anchors.left: fillRect.right
    anchors.top: emptyRect.top
  }
  DrawBtn {
    id: fillCircle
    code: "\ue6bb"
    anchors.left: emptyCircle.right
    anchors.top: emptyRect.top
  }
  ColorPicker {
    anchors.left: fillCircle.right
    anchors.top: emptyRect.top
  }
}
