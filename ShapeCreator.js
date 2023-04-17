var shapes = []

function createComponent(name, obj) {
    var result
    var component = Qt.createComponent(name + ".qml")
    if (component.status === Component.Ready) {
        result = component.createObject(root, obj)
        if (result === null) {
            console.log("Error creating object")
        }
    } else if (component.status === Component.Error) {
        console.log("Error loading component:", component.errorString())
    }
    return result
}

function createShape(mouse) {
    if (App.drawToolState === 1) {
        let shape = createComponent("ShapeRect", {
                                        "startX": mouse.x,
                                        "startY": mouse.y,
                                        "endX": mouse.x,
                                        "endY": mouse.y
                                    })
        shapes.splice(0, 0, shape)
    }
}
