import QtQuick 2.15
import "Shapes.js" as Shapes

Canvas {
    id: root
    anchors.fill: parent
    antialiasing: true
    layer.enabled: true
    layer.samples: 8
    smooth: true
    function getPaintContext(param) {

        return ctx
    }
    function endPaint(param, ctx) {}
    function drawRect(param) {
        var ctx = getContext('2d')
        ctx.lineWidth = param.strokeWidth
        ctx.strokeStyle = param.strokeColor
        ctx.fillStyle = param.fillColor
        if (param.isFill) {
            ctx.fillRect(param.points[0].x, param.points[0].y, param.points[1].x, param.points[1].y)
        } else {
            ctx.strokeRect(param.points[0].x, param.points[0].y, param.points[1].x, param.points[1].y)
        }
        root.requestPaint()
    }
    function drawCircle(param) {
        var ctx = getContext('2d')
        ctx.lineWidth = param.strokeWidth
        ctx.beginPath()
        ctx.ellipse(param.points[0].x, param.points[0].y, param.points[1].x, param.points[1].y)
        if (param.isFill) {
            ctx.fillStyle = param.fillColor
            ctx.fill()
        } else {
            ctx.strokeStyle = param.strokeColor
            ctx.stroke()
        }
        ctx.closePath()
        root.requestPaint()
    }
    function drawArrow(param) {
        var ctx = getContext('2d')
        ctx.lineWidth = param.strokeWidth
        ctx.beginPath()
        ctx.moveTo(param.points[0].x, param.points[0].y)
        for (var i = 1; i < param.points.length; i++) {
            ctx.lineTo(param.points[i].x, param.points[i].y)
        }
        if (param.isFill) {
            ctx.fillStyle = param.fillColor
            ctx.fill()
        } else {
            ctx.strokeStyle = param.strokeColor
            ctx.stroke()
        }
        ctx.closePath()
        root.requestPaint()
    }
    function drawPen(param) {
        var ctx = getContext('2d')
        ctx.lineWidth = 2
        ctx.strokeStyle = param.strokeColor
        ctx.beginPath()
        for (var i = 0; i < param.points.length; i += 2) {
            ctx.moveTo(param.points[i].x, param.points[i].y)
            ctx.lineTo(param.points[i + 1].x, param.points[i + 1].y)
        }
        ctx.stroke()
        ctx.closePath()
        root.requestPaint()
    }
    onPaint: {

    }

    Component.onCompleted: {
        Shapes.paintBoard = root
    }
}
