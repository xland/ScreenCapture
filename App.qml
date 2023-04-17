pragma Singleton

import QtQuick 2.15

QtObject {
    //0未开始,1创建中,2创建完成,3改变大小中,4移动位置中,5绘制中
    property int cutAreaState: 0
    property QtObject mouseTip: QtObject {
        property real x: 0
        property real y: 0
        property real width: 232
        property real height: 234
        property bool visible: false
        property string src: ""
    }
}
