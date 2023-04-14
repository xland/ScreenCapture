var shapes = []
var pressPos = {
  "x": 0,
  "y": 0
}
function createRect(mouse) {
  pressPos.x = mouse.x
  pressPos.y = mouse.y
  var component = Qt.createComponent("ShapeRect.qml")
  var shape = component.createObject(root, {
                                       "anchors": {
                                         "leftMargin": mouse.x,
                                         "topMargin": mouse.y,
                                         "rightMargin": mouse.x,
                                         "bottomMargin": mouse.y
                                       }
                                     })
  shapes.push(shape)
}
function resizeRect(mouse) {
  if (mouse.x < pressPos.x) {
    shapes[0].anchors.leftMargin = mouse.x
    shapes[0].anchors.rightMargin = pressPos.x
  } else {
    shapes[0].anchors.leftMargin = pressPos.x
    shapes[0].anchors.rightMargin = mouse.x
  }
  if (mouse.y < pressPos.y) {
    shapes[0].anchors.topMargin = mouse.y
    shapes[0].anchors.bottomMargin = pressPos.y
  } else {
    shapes[0].anchors.topMargin = pressPos.y
    shapes[0].anchors.bottomMargin = mouse.y
  }
}
