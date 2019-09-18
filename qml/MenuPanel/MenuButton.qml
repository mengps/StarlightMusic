import QtQuick 2.12
import "../Widgets" as Widgets

Item {
    id: root
    clip: true
    property color color: "white"
    property bool hovered: false
    property alias toolTip: toolTip.text
    property alias source: image.source
    property alias imageColor: image.color

    signal clicked();

    Rectangle {
        id: hoveredRect
        width: root.width
        height: root.height
        color: root.color
        visible: root.hovered
    }

    Widgets.ColorImage {
        id: image
        width: 25
        height: 25
        anchors.centerIn: parent
    }

    Widgets.ToolTip {
        id: toolTip
        visible: root.hovered
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked();
        onEntered: root.hovered = true;
        onExited: root.hovered = false;
    }
}
