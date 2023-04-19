import QtQuick 2.15

Canvas {
    id: root
    anchors.fill: parent
    property real lastX
    property real lastY
    property real curX
    property real curY
    property bool drawing: false
    property color color: "red"
    function startDraw(x, y) {
        curX = x
        curY = y
        drawing = true
        root.requestPaint()
    }
    function done() {
        drawing = false
    }

    onPaint: {
        var ctx = getContext('2d')
        ctx.lineWidth = 2
        ctx.strokeStyle = root.color
        ctx.beginPath()
        ctx.moveTo(lastX, lastY)
        lastX = curX
        lastY = curY
        ctx.lineTo(lastX, lastY)
        ctx.stroke()
    }
}
