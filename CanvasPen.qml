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
    property variant points: []
    function startDraw(x, y) {
        curX = x
        curY = y
        drawing = true
        root.requestPaint()
    }
    function getDrawInfo() {
        let result = {
            "type": "Pen",
            "strokeWidth": 2,
            "strokeColor": color,
            "points": points
        }
        return result
    }
    onPaint: {
        var ctx = getContext('2d')
        ctx.lineWidth = 2
        ctx.strokeStyle = root.color
        ctx.beginPath()
        ctx.moveTo(lastX, lastY)
        points.push({
                        "x": lastX,
                        "y": lastY
                    })
        lastX = curX
        lastY = curY
        ctx.lineTo(lastX, lastY)
        points.push({
                        "x": lastX,
                        "y": lastY
                    })
        ctx.stroke()
    }
}
