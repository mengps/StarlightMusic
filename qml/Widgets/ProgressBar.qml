import QtQuick 2.12
import "../Widgets" as Widgets

Item {
    id: root
    property int value: 0
    property int minValue: 0
    property int maxValue: 100
    property alias text: toolTip.text

    MouseArea {
        anchors.fill: parent
        onClicked: parent.value = mouse.x / parent.width * parent.maxValue;
    }

    Rectangle {
        id: progressBack
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        height: 2
        radius: height / 2
        color: skinManager.progressBackColor

        Rectangle {
            id: progress
            width: root.value / root.maxValue * parent.width
            height: parent.height
            radius: height / 2
            color: skinManager.progressColor
        }
    }

    Rectangle {
        id: handle
        anchors.verticalCenter: parent.verticalCenter
        x: root.value / root.maxValue * root.width - width / 2
        width: 14
        height: width
        radius: width / 2
        color: skinManager.progressHandleColor
        property bool hovered: false

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            property real startX: 0.0
            property real offsetX: 0.0
            onPressed: startX = mouse.x;
            onPositionChanged: {
                if(pressed) {
                    offsetX = mouse.x - startX;
                    let test = root.value + offsetX;
                    if (test < root.minValue) root.value = root.minValue;
                    else if (test > root.maxValue) root.value = root.maxValue;
                    else if (test <= root.maxValue && test >= root.minValue) root.value += offsetX;
                }
            }
            onEntered: {
                handle.width += 4;
                parent.hovered = true;
                cursorShape = Qt.PointingHandCursor;
            }
            onExited: {
                handle.width -= 4;
                parent.hovered = false
                cursorShape = Qt.ArrowCursor;
            }
        }

        Widgets.ToolTip {
            id: toolTip
            visible: parent.hovered
        }
    } 
}
