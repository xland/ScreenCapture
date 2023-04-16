var shapes = []
function createRect(mouse) {
    var component = Qt.createComponent("ShapeRect.qml")
    if (component.status === Component.Ready) {
        var shape = component.createObject(root, {
                                               "x1": mouse.x,
                                               "y1": mouse.y,
                                               "x2": mouse.x,
                                               "y2": mouse.y
                                           })
        if (shape === null) {
            console.log("Error creating object")
        }
        shapes.push(shape)
    } else if (component.status === Component.Error) {
        console.log("Error loading component:", component.errorString())
    }
}
function resizeRect(mouse) {
    shapes[0].x2 = mouse.x
    shapes[0].y2 = mouse.y
}
