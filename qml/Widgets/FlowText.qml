import QtQuick 2.12

Item {
    id: root
    clip: true
    property alias text: text.text
    property alias color: text.color
    property alias font: text.font
    onWidthChanged: {
        text.flow();
    }

    function run(direction) {
        if (direction) {
            let diff = text.implicitWidth - width;
            animation.to = -diff;
            animation.restart();
        } else {
            animation.to = 0;
            animation.restart();
        }
    }

    function stop() {
        timer.stop();
        animation.stop();
    }

    Timer {
        id: timer
        running: false
        repeat: false
        onTriggered: root.run(animation.direction);
    }

    Text {
        id: text
        anchors.verticalCenter: parent.verticalCenter
        onTextChanged: {
            flow();
        }

        function flow() {
            root.stop();
            x = 0;
            if (implicitWidth > root.width) root.run(false);
        }
    }

    NumberAnimation {
        id: animation
        target: text
        property: "x"
        duration: 1500
        onStopped: {
            direction = !direction;
            timer.interval = 2000;
            timer.start();
        }
        property bool direction: false
    }
}
