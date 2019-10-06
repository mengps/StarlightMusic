import QtQuick 2.12
import "../Widgets" as Widgets

Widgets.ColorImage {
    id: root
    clip: false
    property bool hovered: false
    property alias toolTip: toolTip.text
    property alias backColor: background.color
    property alias backOpacity: background.opacity

    signal clicked();

    Widgets.ToolTip {
        id: toolTip
        visible: parent.hovered
    }

    Rectangle {
        id: background
        anchors.centerIn: parent
        width: Math.max(root.width, root.height) + 14
        height: width
        radius: height
        color: skinManager.buttonBackColor
        opacity: 0.4
        visible: root.hovered
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: parent
        onClicked: root.clicked(mouse);
        onEntered: {
            root.hovered = true;
            cursorShape = Qt.PointingHandCursor;
        }
        onExited: {
            root.hovered = false;
            cursorShape = Qt.ArrowCursor;
        }
    }
}
