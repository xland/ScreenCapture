import QtQuick 2.15
import QtGraphicalEffects 1.15
import "Shapes.js" as Shapes

Item {
    id: root
    property alias source: blur.source
    anchors.fill: parent
    Canvas {
        id: mask
        anchors.fill: parent
        visible: false
        onPaint: {
            var ctx = getContext('2d')
            ctx.fillStyle = "transparent"
            ctx.fillRect(0, 0, width, height)
            ctx.lineWidth = 42
            ctx.strokeStyle = "#FFFFFF"
            ctx.beginPath()
            ctx.moveTo(0, 0)
            ctx.lineTo(600, 600)
            ctx.stroke()
        }
    }
    MaskedBlur {
        id: blur
        anchors.fill: parent
        maskSource: mask
        radius: 12
        samples: 28
    }
    Component.onCompleted: {
        Shapes.paintMask = root
    }
}
