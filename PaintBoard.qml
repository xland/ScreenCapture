import QtQuick 2.15
import "Shapes.js" as Shapes

Item {
    id: root
    anchors.fill: parent
    function drawRect(param) {
        board.context.lineWidth = param.strokeWidth
        board.context.strokeStyle = param.strokeColor
        board.context.fillStyle = param.fillColor
        if (param.isFill) {
            board.context.fillRect(param.points[0].x, param.points[0].y, param.points[1].x, param.points[1].y)
        } else {
            board.context.strokeRect(param.points[0].x, param.points[0].y, param.points[1].x, param.points[1].y)
        }
        board.requestPaint()
    }
    function drawCircle(param) {
        board.context.lineWidth = param.strokeWidth
        board.context.beginPath()
        board.context.ellipse(param.points[0].x, param.points[0].y, param.points[1].x, param.points[1].y)
        if (param.isFill) {
            board.context.fillStyle = param.fillColor
            board.context.fill()
        } else {
            board.context.strokeStyle = param.strokeColor
            board.context.stroke()
        }
        board.context.closePath()
        board.requestPaint()
    }
    function drawArrow(param) {
        board.context.lineWidth = param.strokeWidth
        board.context.beginPath()
        board.context.moveTo(param.points[0].x, param.points[0].y)
        for (var i = 1; i < param.points.length; i++) {
            board.context.lineTo(param.points[i].x, param.points[i].y)
        }
        if (param.isFill) {
            board.context.fillStyle = param.fillColor
            board.context.fill()
        } else {
            board.context.strokeStyle = param.strokeColor
            board.context.stroke()
        }
        board.context.closePath()
        board.requestPaint()
    }
    function drawPen(param) {
        board.context.lineWidth = 2
        board.context.strokeStyle = param.strokeColor
        board.context.beginPath()
        for (var i = 0; i < param.points.length; i += 2) {
            board.context.moveTo(param.points[i].x, param.points[i].y)
            board.context.lineTo(param.points[i + 1].x, param.points[i + 1].y)
        }
        board.context.stroke()
        board.context.closePath()
        board.requestPaint()
    }
    Image {
        id: backgroundImage
        x: 0
        y: 0
        source: "image://ScreenImage/background"
        sourceSize.width: root.width
        sourceSize.height: root.height
    }
    Canvas {
        id: board
        anchors.fill: parent
        antialiasing: true
        layer.enabled: true
        layer.samples: 8
        smooth: true
        contextType: "2d"
        onPaint: {

        }
    }
    Component.onCompleted: {
        Shapes.paintBoard = root
    }
}
