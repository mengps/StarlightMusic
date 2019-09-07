import QtQuick 2.12
import "Api/Api.js" as Api
import "Effects" as Effects
import "Widgets" as Widgets

Item {
    id: root
    height: 30
    property bool pressed: false
    property real value: 0.0
    property bool hovered: false
    property alias backColor: progressBack.color
    property alias handle: handle
    property alias progressBack: progressBack
    readonly property real minValue: 0.0
    readonly property real maxValue: 1.0
    onValueChanged: effect.system.start();
    onWidthChanged: handle.x = value * width - handle.width / 2;

    function setValue(v) {
        if (!pressed) {
            handle.x = v * root.width - handle.width / 2;
        }
    }

    Rectangle {
        id: progressBack
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        height: 2
        radius: height / 2
        color: "#CCC"

        Rectangle {
            id: progress
            height: parent.height
            radius: height / 2
            color: "#4dffd0"
        }
    }

    /*Rectangle {
        id: handle
        anchors.verticalCenter: parent.verticalCenter
        border.color: "red"
        width: 24
        height: 24
        radius: height / 2
        onXChanged: {
            progress.width = x / root.width * root.width + handle.width / 2;
            root.value = (x + handle.width / 2) / root.width;
        }

        DragHandler {
            id: dragHandle
            target: handle
            xAxis.minimum: - handle.width / 2
            xAxis.maximum: root.width - handle.width / 2
            onActiveChanged: {
                if (active) root.pressed = true;
                else root.pressed = false;
            }
        }
    }*/

    Effects.RiseEffect {
        id: effect
        width: progress.width
        height: parent.height
        source: "qrc:/image/ParticleImage/star.png"
        emitRate: root.value * 120
    }

    Image {
        id: handle
        x: -handle.width / 2
        source: "qrc:/image/handle.png"
        anchors.verticalCenter: parent.verticalCenter
        sourceSize: Qt.size(height, height)
        visible: false
        width: 24
        height: 24
        onXChanged: {
            progress.width = x / root.width * root.width + handle.width / 2;
            root.value = (x + handle.width / 2) / root.width;
            if (pressed) {
                toolTip.text = Api.time2string(root.value * musicPlayer.duration);
                toolTip.visible = true;
            }
        }

        DragHandler {
            id: dragHandle
            target: handle
            xAxis.minimum: - handle.width / 2
            xAxis.maximum: root.width - handle.width / 2
            onActiveChanged: {
                if (active) root.pressed = true;
                else {
                    toolTip.visible = false;
                    root.pressed = false;
                }
            }
        }

        Widgets.ToolTip {
            id: toolTip
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            handle.x = mouse.x - handle.width / 2;
            root.pressedChanged();
        }
        onEntered: handle.visible = true;
        onExited: if (!root.pressed) handle.visible = false;
    }
}
