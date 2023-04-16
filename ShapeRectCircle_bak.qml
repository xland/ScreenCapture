import QtQuick 2.15

MouseArea {
    anchors.fill: parent
    acceptedButtons: Qt.LeftButton | Qt.RightButton
    hoverEnabled: true
    cursorShape: Qt.CrossCursor
    onPressed: mouse => {
                   if (mouse.buttons === Qt.LeftButton) {
                       ShapeCreator.createRect(mouse)
                   }
               }
    onPositionChanged: mouse => {
                           if (mouse.buttons === Qt.LeftButton) {
                               ShapeCreator.resizeRect(mouse)
                           }
                       }
    onReleased: () => {
                    ShapeCreator.createDragger()
                }
}
