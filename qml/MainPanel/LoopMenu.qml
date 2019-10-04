import QtQuick 2.12
import QtQuick.Controls 2.12
import an.music 1.0

Menu {
    id: root
    focus: false
    x: -width / 2 + loopButton.width / 2
    y: -height - loopButton.height / 2
    background: Rectangle {
        radius: 5
        implicitWidth: 104
        implicitHeight: 106
        color: "#F9FCFE"
    }
    enter: Transition {
        NumberAnimation {
            alwaysRunToEnd: true
            property: "opacity"
            easing.type: Easing.Linear
            from: 0
            to: 1
            duration: 400
            onStopped: editor.focus = true;
        }
    }

    FocusScope {
        id: focusScope
        focus: false

        MenuItem {
            id: orderItem
            focus: false
            height: 30
            anchors.top: parent.top
            anchors.topMargin: 4
            background: Rectangle {
                width: root.width
                height: 30
                radius: 2
                border.color: "#DDD"
                color: hovered ? "#9EF2FA" : "#F9FCFE"
                property bool hovered: false

                Text {
                    anchors.centerIn: parent
                    text: qsTr("顺序循环")
                }

                MouseArea {
                    hoverEnabled: true
                    anchors.fill: parent
                    onEntered: parent.hovered = true;
                    onExited:  parent.hovered = false;
                    onClicked: {
                        root.close();
                        musicPlayer.playMode = MusicPlayer.Order;
                    }
                }
            }
        }

        MenuItem {
            id: singleItem
            focus: false
            height: 30
            anchors.top: orderItem.bottom
            anchors.topMargin: 4
            background: Rectangle {
                width: root.width
                height: 30
                radius: 2
                border.color: "#DDD"
                color: hovered ? "#9EF2FA" : "#F9FCFE"
                property bool hovered: false

                Text {
                    anchors.centerIn: parent
                    text: qsTr("单曲循环")
                }

                MouseArea {
                    hoverEnabled: true
                    anchors.fill: parent
                    onEntered: parent.hovered = true;
                    onExited:  parent.hovered = false;
                    onClicked: {
                        root.close();
                        musicPlayer.playMode = MusicPlayer.Single;
                    }
                }
            }
        }

        MenuItem {
            id: randomItem
            focus: false
            height: 30
            anchors.top: singleItem.bottom
            anchors.topMargin: 4
            background: Rectangle {
                width: root.width
                height: 30
                radius: 2
                border.color: "#DDD"
                color: hovered ? "#9EF2FA" : "#F9FCFE"
                property bool hovered: false

                Text {
                    anchors.centerIn: parent
                    text: qsTr("随机循环")
                }

                MouseArea {
                    hoverEnabled: true
                    anchors.fill: parent
                    onEntered: parent.hovered = true;
                    onExited:  parent.hovered = false;
                    onClicked: {
                        root.close();
                        musicPlayer.playMode = MusicPlayer.Random;
                    }
                }
            }
        }
    }
}
