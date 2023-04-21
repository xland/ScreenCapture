import QtQuick 2.15

Canvas {
    id: root
    anchors.fill: parent
    antialiasing: true
    layer.enabled: true
    layer.samples: 8
    renderTarget: Canvas.FramebufferObject
    property var operation: ['source-over', 'source-in', 'source-over', 'source-atop', 'destination-over', 'destination-in', 'destination-out', 'destination-atop', 'lighter', 'copy', 'xor', 'qt-clear', 'qt-destination', 'qt-multiply', 'qt-screen', 'qt-overlay', 'qt-darken', 'qt-lighten', 'qt-color-dodge', 'qt-color-burn', 'qt-hard-light', 'qt-soft-light', 'qt-difference', 'qt-exclusion']
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
