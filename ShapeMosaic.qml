import QtQuick 2.15
import QtGraphicalEffects 1.15
import QtQuick.Shapes 1.15

Item {
    property real startX1: 0
    property real startY1: 0
    property real endX: 0
    property real endY: 0
    property bool showDragger: false
    //    property real span: draggerLeftTop.width / 2
    function getDrawInfo() {
        let result = {//            "type": "Mosaic",
            //            "strokeWidth": borderWidth,
            //            "strokeColor": borderColor,
            //            "fillColor": bgColor,
            //            "isFill": isFill,
            //            "points": []
        }
        return result
    }
    function setDraggerPosition() {
        draggerLeftTop.x = startX1 - span
        draggerLeftTop.y = startY1 - span

        draggerTop.x = startX1 + (endX - startX1) / 2 - span
        draggerTop.y = startY1 - span

        draggerRightTop.x = endX - span
        draggerRightTop.y = startY1 - span

        draggerRight.x = endX - span
        draggerRight.y = startY1 + (endY - startY1) / 2 - span

        draggerRightBottom.x = endX - span
        draggerRightBottom.y = endY - span

        draggerBottom.x = startX1 + (endX - startX1) / 2 - span
        draggerBottom.y = endY - span

        draggerLeftBottom.x = startX1 - span
        draggerLeftBottom.y = endY - span

        draggerLeft.x = startX1 - span
        draggerLeft.y = startY1 + (endY - startY1) / 2 - span
    }
    function updatePosition() {
        shader.width = endX - startX1
        shader.height = endY - startY1
        shader.x = startX1
        shader.y = startY1
        setDraggerPosition()
    }

    function positionChanged(x1, y1) {//        if (!shaderImg.source) {
        //            root.parent.parent.children[0].grabToImage(function (result) {
        //                shaderImg.source = result.url
        //                console.log(123)
        //            })
        //        }

        //        var ctx = mask.getContext('2d')
        //        ctx.lineWidth = 32
        //        ctx.strokeStyle = "#000000"
        //        ctx.beginPath()
        //        ctx.moveTo(startX1, startY1)
        //        ctx.lineTo(x1, y1)
        //        startX1 = x1
        //        startY1 = y1
        //        ctx.stroke()
        //        mask.requestPaint()
    }

    id: root
    anchors.fill: parent
    Canvas {
        id: mask
        anchors.fill: parent
        onPaint: {
            var ctx = getContext('2d')
            ctx.fillStyle = "transparent"
            ctx.fillRect(0, 0, width, height)
            ctx.lineWidth = 42
            ctx.strokeStyle = "#000000"
            ctx.beginPath()
            ctx.moveTo(0, 0)
            ctx.lineTo(600, 600)
            ctx.stroke()
        }
    }
    MaskedBlur {
        id: blur
        anchors.fill: parent
        source: board
        maskSource: mask
        radius: 12
        samples: 28
    }
}
