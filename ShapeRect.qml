import QtQuick 2.15
import QtQuick.Shapes 1.15
import "Resizer.js" as Resizer

Shape {
    property string customColor: "red"
    property int x1: 0
    property int y1: 0
    property int x2: 0
    property int y2: 0
    ShapePath {
        strokeWidth: 3
        strokeColor: customColor
        fillColor: "transparent"
        startX: x1
        startY: y1
        PathLine {
            x: x2
            y: y1
        }
        PathLine {
            x: x2
            y: y2
        }
        PathLine {
            x: x1
            y: y2
        }
        PathLine {
            x: x1
            y: y1
        }
    }
}
