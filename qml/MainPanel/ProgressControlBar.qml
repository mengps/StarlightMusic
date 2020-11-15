import QtQuick 2.12
import "../Api/Api.js" as Api
import "../Effects" as Effects
import "../Widgets" as Widgets

Item {
    id: root

    height: 30
    property bool pressed: false
    property real value: 0.0
    property bool hovered: false
    property alias handle: handle
    property alias effect: effect
    property alias progressBack: progressBack
    readonly property real minValue: 0.0
    readonly property real maxValue: 1.0
    onWidthChanged: handle.x = value * width - handle.width / 2;

    function setValue(v) {
        if (!pressed) {
            handle.x = v * root.width - handle.width / 2;
        }
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            handle.x = mouse.x - handle.width / 2;
            root.pressedChanged();
            effectTimer.triggered();
        }
        onEntered: {
            cursorShape = Qt.PointingHandCursor;
            handle.visible = true;
        }
        onExited: {
            cursorShape = Qt.ArrowCursor;
            if (!root.pressed) handle.visible = false;
        }
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
            height: parent.height
            radius: height / 2
            color: skinManager.progressColor
        }
    }

    Effects.RiseEffect {
        id: effect
        width: 0
        height: parent.height
        source: skinManager.progressParticleImage
        emitRate: 0

        Timer {
            id: effectTimer
            interval: 1000
            running: effect.running
            repeat: true
            onTriggered: {
                effect.width = progress.width;
                effect.emitRate = progress.width / 12;
            }
        }
    }

    Widgets.ColorImage {
        id: handle
        x: -width / 2
        color: skinManager.progressHandleColor
        source: skinManager.progressHandleImage
        anchors.verticalCenter: parent.verticalCenter
        visible: false
        width: 24
        height: 24
        onXChanged: {
            progress.width = x / root.width * root.width + handle.width / 2;
            root.value = (x + handle.width / 2) / root.width;
            if (pressed) {
                effectTimer.triggered();
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
}
