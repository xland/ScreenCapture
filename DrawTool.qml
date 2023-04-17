import QtQuick 2.15
import ScreenCapture.Cutter 1.0

Row {
    id: root
    visible: App.cutAreaState === 2
    anchors.right: parent.right
    anchors.rightMargin: Cutter.totalWidth - Cutter.cutAreaRight
    anchors.top: parent.top
    anchors.topMargin: Cutter.cutAreaBottom + 8
    width: 360
    height: 30
    DrawBtn {
        id: rectBtn
        code: "\ue86d"
        isSelected: App.drawToolState === 1
        onClick: () => {
                     App.drawToolState = 1
                 }
    }
    DrawBtn {
        id: arrawBtn
        code: "\ueb0c"
        isSelected: App.drawToolState === 2
        onClick: () => {
                     App.drawToolState = 2
                 }
    }
    DrawBtn {
        id: penBtn
        code: "\ue601"
        isSelected: App.drawToolState === 3
        onClick: () => {
                     App.drawToolState = 3
                 }
    }
    DrawBtn {
        id: mosaicBtn
        code: "\ue82e"
        isSelected: App.drawToolState === 4
        onClick: () => {
                     App.drawToolState = 4
                 }
    }
    DrawBtn {
        id: textBtn
        code: "\ue6ec"
        isSelected: App.drawToolState === 5
        onClick: () => {
                     App.drawToolState = 5
                 }
    }
    DrawBtn {
        id: eraserBtn
        code: "\ue6b8"
        isSelected: App.drawToolState === 6
        onClick: () => {
                     App.drawToolState = 6
                 }
    }
    DrawBtn {
        id: undoBtn
        code: "\ued85"
        onClick: () => {}
    }
    DrawBtn {
        id: redoBtn
        code: "\ued8a"
        onClick: () => {}
    }
    DrawBtn {
        id: clipboardBtrn
        code: "\ue87f"
        onClick: () => {}
    }
    DrawBtn {
        id: saveBtn
        code: "\ue62f"
        onClick: () => {}
    }
}
