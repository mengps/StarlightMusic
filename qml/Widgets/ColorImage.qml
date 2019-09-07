import QtQuick 2.12
import QtGraphicalEffects 1.13

Item {
    property alias color: rect.color
    property alias source: mask.source

    Rectangle {
        id: rect
        anchors.fill: parent
        visible: false
    }

    Image {
        id: mask
        anchors.fill: parent
        sourceSize: Qt.size(parent.width, parent.height)
        fillMode: Image.PreserveAspectFit
        smooth: true
        mipmap: true
        visible: true
    }

    OpacityMask {
        anchors.fill: parent
        source: rect
        maskSource: mask
    }
}
