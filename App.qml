pragma Singleton

import QtQuick 2.15

QtObject {
    //0未开始,1创建中,2创建完成,3改变大小中,4移动位置中
    property int cutAreaState: 0
    //0未选择，1画方画圆,2画箭头,3画线,4马赛克,5写字,6橡皮
    property int drawToolState: 0
    property QtObject rectCircle: QtObject {
        property bool isRect: true
        property bool isFill: false
        property string fillColor: "transparent"
        property real strokeWidth: 3
        property string strokeColor: "red"
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
