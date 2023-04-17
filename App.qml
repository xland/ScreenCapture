pragma Singleton

import QtQuick 2.15

QtObject {
    //0未开始,1创建中,2创建完成,3改变大小中,4移动位置中,5绘制中
    property int cutAreaState: 0
    //1画方画圆,2画箭头,3画线,4马赛克,5写字,6橡皮
    property int drawToolState: 1
    property QtObject rectCircle: QtObject {
        property bool isRect: true
        property bool isFill: false
        property real borderWidth: 3
        property string color: "red"
    }

    property QtObject mouseTip: QtObject {
        property real x: 0
        property real y: 0
        property real width: 232
        property real height: 234
        property bool visible: false
        property string src: ""
    }
}
