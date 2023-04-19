.pragma library
var shapes = []
function done() {
    if (shapes.length > 0) {
        shapes[0].done()
    }
}
