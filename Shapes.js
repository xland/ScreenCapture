.pragma library
var instance
var paintBoard
var paintMask
function createComponent(name, config, parent) {
    var component = Qt.createComponent(name + ".qml")
    if (component.status === 1) {
        instance = component.createObject(parent, config)
        if (instance === null) {
            console.log("Error creating object")
        }
    } else {
        console.log("Error loading component:", component.errorString())
    }
}
function done() {
    if (!instance) {
        return
    }
    let result = instance.getDrawInfo()
    paintBoard["draw" + result.type](result)
    instance.destroy()
    instance = null
    console.log("destroy", result.type)
}
