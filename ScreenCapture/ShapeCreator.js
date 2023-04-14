var shapes = []
var pressPos = {
  "x": 0,
  "y": 0
}
function createRect(mouse) {
  pressPos.x = mouse.x
  pressPos.y = mouse.y
  var component = Qt.createComponent("ShapeRect.qml")
  var shape = component.createObject(root, {})
  shapes.push(shape)
}
function resizeRect(mouse) {
  if (mouse.x < pressPos.x) {
    shapes[0].x = mouse.x
    shapes[0].width = pressPos.x - mouse.x
  } else {
    shapes[0].x = pressPos.x
    shapes[0].width = mouse.x - pressPos.x
  }
  if (mouse.y < pressPos.y) {
    shapes[0].y = mouse.y
    shapes[0].height = pressPos.y - mouse.y
  } else {
    shapes[0].y = pressPos.y
    shapes[0].height = mouse.y - pressPos.y
  }
}
