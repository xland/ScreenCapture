let shapes = []
function createReact(mouse, isFill) {
  let component = Qt.createComponent("ShapeRect.qml")
  let shape = component.createObject(root, {
                                       "x": mouse.x,
                                       "y": mouse.y,
                                       "width": 100,
                                       "height": 200
                                     })
  shapes.push(shape)
}
