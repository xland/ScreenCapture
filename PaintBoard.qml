import QtQuick 2.15
import "Shapes.js" as Shapes

Canvas {
    id: root
    anchors.fill: parent
    //    antialiasing: true
    //    layer.enabled: true
    //    layer.samples: 8
    //    renderTarget: Canvas.FramebufferObject
    property var operation: ['source-over', 'source-in', 'source-over', 'source-atop', 'destination-over', 'destination-in', 'destination-out', 'destination-atop', 'lighter', 'copy', 'xor', 'qt-clear', 'qt-destination', 'qt-multiply', 'qt-screen', 'qt-overlay', 'qt-darken', 'qt-lighten', 'qt-color-dodge', 'qt-color-burn', 'qt-hard-light', 'qt-soft-light', 'qt-difference', 'qt-exclusion']
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

        //        var ctx = getContext('2d')
        //        console.log(1212121)
        //        for (var i = 0; i < operation.length; i++) {
        //            var dx = Math.floor(i % 6) * 100
        //            var dy = Math.floor(i / 6) * 100
        //            ctx.save()
        //            ctx.fillStyle = '#33a9ff'
        //            ctx.fillRect(10 + dx, 10 + dy, 60, 60)
        //            ctx.globalCompositeOperation = root.operation[i]
        //            ctx.fillStyle = '#ff33a9'
        //            ctx.globalAlpha = 0.75
        //            ctx.beginPath()
        //            ctx.arc(60 + dx, 60 + dy, 30, 0, 2 * Math.PI)
        //            ctx.closePath()
        //            ctx.fill()
        //            ctx.restore()
        //        }
        //        root.requestPaint()
        let ctx = getPaintContext(param)
        ctx.rect(param.points[0].x, param.points[0].y, param.points[1].x, param.points[1].y)
        ctx.globalCompositeOperation = 'xor'
        ctx.fillStyle = '#33a9ff'
        ctx.fillRect(10, 10, 600, 600)
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
    onPaint: {

    }

    Component.onCompleted: {
        Shapes.paintBoard = root
    }
}
