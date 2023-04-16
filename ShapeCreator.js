var shapes = []
var draggers = []

function createComponent(name, num) {
    var result = []
    var component = Qt.createComponent(name + ".qml")
    if (component.status === Component.Ready) {
        for (var i = 0; i < num; i++) {
            var shape = component.createObject(shapeContainer, {})
            if (shape === null) {
                console.log("Error creating object")
            }
            result.push(shape)
        }
    } else if (component.status === Component.Error) {
        console.log("Error loading component:", component.errorString())
    }
    return result
}

function createRect(mouse) {
    let rect = createComponent("ShapeRect", 1)[0]
    rect.x1 = mouse.x
    rect.y1 = mouse.y
    rect.x2 = mouse.x
    rect.y2 = mouse.y
    //    for (let item in rect) {
    //        console.log(item)
    //    }
    shapes.splice(0, 0, rect)
}
function resizeRect(mouse) {
    shapes[0].x2 = mouse.x
    shapes[0].y2 = mouse.y
}
function createDragger() {

    //    console.log(shapes[0].objectName)
    //    if (shapes[0].objectName === "ShapeRect") {

    //    }
    let objs = createComponent("Dragger", 8)
    let span = objs[0].width / 2
    //左上
    objs[0].x = shapes[0].x1 - span
    objs[0].y = shapes[0].y1 - span
    objs[0].onXChanged = () => {}

    //上
    objs[1].cursor = Qt.SizeVerCursor
    objs[1].axis = Drag.YAxis
    objs[1].x = shapes[0].x1 + (shapes[0].x2 - shapes[0].x1) / 2
    objs[1].y = shapes[0].y1 - span

    //右上
    objs[2].cursor = Qt.SizeBDiagCursor
    objs[2].x = shapes[0].x2 - span
    objs[2].y = shapes[0].y1 - span
    //右
    objs[3].axis = Drag.XAxis
    objs[3].cursor = Qt.SizeHorCursor
    objs[3].x = shapes[0].x2 - span
    objs[3].y = shapes[0].y1 + (shapes[0].y2 - shapes[0].y1) / 2
    //右下
    objs[4].x = shapes[0].x2 - span
    objs[4].y = shapes[0].y2 - span
    //下
    objs[5].axis = Drag.YAxis
    objs[5].cursor = Qt.SizeVerCursor
    objs[5].x = shapes[0].x1 + (shapes[0].x2 - shapes[0].x1) / 2
    objs[5].y = shapes[0].y2 - span
    //左下
    objs[6].cursor = Qt.SizeBDiagCursor
    objs[6].x = shapes[0].x1 - span
    objs[6].y = shapes[0].y2 - span
    //左
    objs[7].axis = Drag.XAxis
    objs[7].cursor = Qt.SizeHorCursor
    objs[7].x = shapes[0].x1 - span
    objs[7].y = shapes[0].y1 + (shapes[0].y2 - shapes[0].y1) / 2

    draggers = draggers.concat(objs)

    //    shapes.push(rect)
}
