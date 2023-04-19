import QtQuick 2.15
import "Shapes.js" as Shapes
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
                     Shapes.done()
                     if (App.drawToolState === 1) {
                         App.drawToolState = 0
                     } else {
                         App.drawToolState = 1
                     }
                 }
    }
    DrawBtn {
        id: arrawBtn
        code: "\ueb0c"
        isSelected: App.drawToolState === 2
        onClick: () => {
                     Shapes.done()
                     if (App.drawToolState === 2) {
                         App.drawToolState = 0
                     } else {
                         App.drawToolState = 2
                     }
                 }
    }
    DrawBtn {
        id: penBtn
        code: "\ue601"
        isSelected: App.drawToolState === 3
        onClick: () => {
                     Shapes.done()
                     if (App.drawToolState === 3) {
                         App.drawToolState = 0
                     } else {
                         App.drawToolState = 3
                     }
                 }
    }
    DrawBtn {
        id: mosaicBtn
        code: "\ue82e"
        isSelected: App.drawToolState === 4
        onClick: () => {
                     Shapes.done()
                     if (App.drawToolState === 4) {
                         App.drawToolState = 0
                     } else {
                         App.drawToolState = 4
                     }
                 }
    }
    DrawBtn {
        id: textBtn
        code: "\ue6ec"
        isSelected: App.drawToolState === 5
        onClick: () => {
                     Shapes.done()
                     if (App.drawToolState === 5) {
                         App.drawToolState = 0
                     } else {
                         App.drawToolState = 5
                     }
                 }
    }
    DrawBtn {
        id: eraserBtn
        code: "\ue6b8"
        isSelected: App.drawToolState === 6
        onClick: () => {
                     Shapes.done()
                     if (App.drawToolState === 6) {
                         App.drawToolState = 0
                     } else {
                         App.drawToolState = 6
                     }
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
