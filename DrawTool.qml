import QtQuick 2.15

Rectangle {
    FontLoader {
        source: "font.ttf"
    }
    property int selectedIndex: -1
    signal startDraw
    function unSelectAllBtns() {
        for (var i = 0; i < 10; i++) {
            root.children[i].unSelect()
        }
    }

    id: root
    width: 360
    height: 30
    color: "transparent"
    DrawBtn {
        id: rectBtn
        code: "\ue86d"
        anchors.left: parent.left
        onBeforeSelected: () => {
                              unSelectAllBtns()
                              startDraw()
                          }
    }
    DrawBtn {
        id: arrawBtn
        code: "\ueb0c"
        anchors.left: rectBtn.right
        onBeforeSelected: () => {
                              unSelectAllBtns()
                          }
    }
    DrawBtn {
        id: penBtn
        code: "\ue601"
        anchors.left: arrawBtn.right
        onBeforeSelected: () => {
                              unSelectAllBtns()
                          }
    }
    DrawBtn {
        id: mosaicBtn
        code: "\ue82e"
        anchors.left: penBtn.right
        onBeforeSelected: () => {
                              unSelectAllBtns()
                          }
    }
    DrawBtn {
        id: textBtn
        code: "\ue6ec"
        anchors.left: mosaicBtn.right
        onBeforeSelected: () => {
                              unSelectAllBtns()
                          }
    }
    DrawBtn {
        id: eraserBtn
        code: "\ue6b8"
        anchors.left: textBtn.right
        onBeforeSelected: () => {
                              unSelectAllBtns()
                          }
    }
    DrawBtn {
        id: undoBtn
        code: "\ued85"
        anchors.left: eraserBtn.right
        onBeforeSelected: () => {
                              unSelectAllBtns()
                          }
    }
    DrawBtn {
        id: redoBtn
        code: "\ued8a"
        anchors.left: undoBtn.right
        onBeforeSelected: () => {
                              unSelectAllBtns()
                          }
    }
    DrawBtn {
        id: clipboardBtrn
        code: "\ue87f"
        anchors.left: redoBtn.right
        onBeforeSelected: () => {
                              unSelectAllBtns()
                          }
    }
    DrawBtn {
        id: saveBtn
        code: "\ue62f"
        anchors.left: clipboardBtrn.right
        onBeforeSelected: () => {
                              unSelectAllBtns()
                          }
    }

    DrawRectTool {
        id: drawRectTool
        visible: rectBtn.isSelected
        anchors.left: root.left
        anchors.top: root.top
        anchors.topMargin: 34
        z: 999
    }
}
