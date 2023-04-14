let startX
let startY
function start(mouse) {
  startX = mouse.x
  startY = mouse.y
}
function leftTop(mouse) {
  let spanX = mouse.x - startX
  let spanY = mouse.y - startY
  if (root.width - spanX <= 0 && root.height - spanY <= 0) {
    console.log(456)
    areaLeftTop.released(mouse)
    areaRightBottom.pressed(mouse)
    areaRightBottom.positionChanged(mouse)
    //    areaLeftTop.enable = false
    return
  } else if (root.width - spanX <= 0) {
    mouse.accepted = false
    return
  } else if (root.height - spanY <= 0) {
    mouse.accepted = false
    return
  } else {
    root.x = root.x + spanX
    root.y = root.y + spanY
    root.width = root.width - spanX
    root.height = root.height - spanY
  }
}

function left(mouse) {
  let spanX = mouse.x - startX
  root.x = root.x + spanX
  root.width = root.width - spanX
}
function right(mouse) {
  let spanX = mouse.x - startX
  root.width = root.width + spanX
}

function rightBottom(mouse) {
  console.log(123)
  let spanX = mouse.x - startX
  let spanY = mouse.y - startY
  root.width = root.width + spanX
  root.height = root.height + spanY
}
