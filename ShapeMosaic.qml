import QtQuick 2.15

Canvas {
    id: root
    anchors.fill: parent
    antialiasing: true
    layer.enabled: true
    layer.samples: 8
    renderTarget: Canvas.FramebufferObject
    function getDrawInfo() {
        let result = {
            "type": "Mosaic",
            "strokeWidth": 2,
            "points": []
        }
        return result
    }
    onPaint: {
        var ctx = getContext('2d')
        ctx.globalCompositeOperation = 'xor'
        ctx.fillStyle = '#33a9ff'
        ctx.fillRect(10, 10, 600, 600)
        console.log(123)
    }
}
