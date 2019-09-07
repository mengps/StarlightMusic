import QtQuick 2.12
import QtQuick.Controls 2.12

ToolTip
{
    id: root
    font.pointSize: 9
    font.family: "微软雅黑"
    opacity: 0
    background: Rectangle {
        radius: 4
        color: "#CCFFFFFF"
        border.color: "#888"
        border.width: 1
    }

    NumberAnimation {
        id: animation
        target: root
        running: false
        property: "opacity"
        from: 0
        to: 1
        duration: 700
        easing.type: Easing.InOutQuad
    }

    onVisibleChanged: if (visible) animation.start();
}
