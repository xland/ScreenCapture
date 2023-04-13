import QtQuick

Rectangle {
  component SmallColor: Rectangle {
    width: 11
    height: 11
    border.width: 0.5
    border.color: "#000000"
  }
  color: "#F5F5F5"
  width: 126
  height: 30
  Rectangle {
    id: firstColor
    anchors.left: parent.left
    anchors.leftMargin: 3
    anchors.top: parent.top
    anchors.topMargin: 3
    border.width: 0.5
    border.color: "#000000"
    width: 24
    height: 24
    color: "red"
  }
  Row {
    id: firstRow
    anchors.left: firstColor.right
    anchors.top: firstColor.top
    anchors.leftMargin: 2
    spacing: 2
    SmallColor {
      color: "black"
    }
    SmallColor {
      color: "#f5222d"
    }
    SmallColor {
      color: "#fa541c"
    }
    SmallColor {
      color: "#fa8c16"
    }
    SmallColor {
      color: "#faad14"
    }
    SmallColor {
      color: "#d4b106"
    }
    SmallColor {
      color: "#7cb305"
    }
  }
  Row {
    anchors.left: firstColor.right
    anchors.top: firstRow.bottom
    anchors.leftMargin: 2
    anchors.topMargin: 2
    spacing: 2
    SmallColor {
      color: "white"
    }
    SmallColor {
      color: "#52c41a"
    }
    SmallColor {
      color: "#13c2c2"
    }
    SmallColor {
      color: "#1677ff"
    }
    SmallColor {
      color: "#2f54eb"
    }
    SmallColor {
      color: "#722ed1"
    }
    SmallColor {
      color: "#eb2f96"
    }
  }
}
