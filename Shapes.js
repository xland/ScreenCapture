.pragma library
var shapes = []
var paintBoard
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
