.pragma library
var shapes = []
var paintBoard
function createComponent(name, config, parent) {
    var result
    var component = Qt.createComponent(name + ".qml")
    if (component.status === 1) {
        result = component.createObject(parent, config)
        if (result === null) {
            console.log("Error creating object")
        }
    } else {
        console.log("Error loading component:", component.errorString())
    }
    shapes.splice(0, 0, result)
    return result
}
function done() {
    if (shapes.length <= 0) {
        return
    }
    let result = shapes[0].getDrawInfo()
    paintBoard["draw" + result.type](result)
    shapes[0].destroy()
    shapes.splice(0, 1)
    console.log("destroy", result.type)
}
