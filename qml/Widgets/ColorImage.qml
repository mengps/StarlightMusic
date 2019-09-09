import QtQuick 2.12
import QtGraphicalEffects 1.13

Item {
    property alias color: rect.color
    property alias source: mask.source

    Component.onCompleted: mask.sourceSize = Qt.size(width, height)

    Rectangle {
        id: rect
        anchors.fill: parent
        visible: false
    }

    Image {
        id: mask
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        mipmap: true
        visible: true
    }

    OpacityMask {
        anchors.fill: parent
        cached: true
        source: rect
        maskSource: mask
    }
}
