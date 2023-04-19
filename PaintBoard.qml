import QtQuick 2.15
import "Shapes.js" as Shapes

Canvas {
    id: root
    anchors.fill: parent
    antialiasing: true
    layer.enabled: true
    layer.samples: 8
    function getPaintContext(param) {
        var ctx = getContext('2d')
        ctx.lineWidth = param.strokeWidth
        ctx.strokeStyle = param.strokeColor
        ctx.fillStyle = param.fillColor
        return ctx
    }
    function endPaint(param, ctx) {
        ctx.stroke()
        if (param.isFill) {
            ctx.fill()
        }
        ctx.save()
        root.requestPaint()
    }
    function drawRect(param) {
        let ctx = getPaintContext(param)
        ctx.rect(param.points[0].x, param.points[0].y, param.points[1].x, param.points[1].y)
        endPaint(param, ctx)
    }
    function drawCircle(param) {
        let ctx = getPaintContext(param)
        ctx.ellipse(param.points[0].x, param.points[0].y, param.points[1].x, param.points[1].y)
        endPaint(param, ctx)
    }
    function drawArrow(param) {
        let ctx = getPaintContext(param)
        ctx.beginPath()
        ctx.moveTo(param.points[0].x, param.points[0].y)
        for (var i = 1; i < param.points.length; i++) {
            ctx.lineTo(param.points[i].x, param.points[i].y)
        }
        endPaint(param, ctx)
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
        ctx.save()
        root.requestPaint()
    }

    Component.onCompleted: {
        Shapes.paintBoard = root
    }
}
