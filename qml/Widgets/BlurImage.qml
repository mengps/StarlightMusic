import QtQuick 2.12
import QtGraphicalEffects 1.13

Item {

    property bool blur: false
    property alias blurRadius: fastBlur.radius
    property alias source: image.source
    property alias sourceSize: image.sourceSize
    property alias fillMode: image.fillMode

    Image {
        id: image
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        smooth: true
        visible: true
    }

    FastBlur {
        id: fastBlur
        anchors.fill: image
        visible: parent.blur
        source: image
        radius: 56
    }
}
