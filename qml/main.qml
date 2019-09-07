import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import "Api/Api.js" as Api
import "Effects" as Effects
import "Widgets" as Widgets

Window {
    id: root
    visible: true
    width: 1024
    height: 680
    title: qsTr("StarlightMusic")
    //需要增加一个全局的SetttingManager
    property color globalColor: "white"

    Image {
        id: background
        anchors.fill: parent
        antialiasing: true
        mipmap: true
        fillMode: Image.PreserveAspectCrop
        source: "qrc:/image/background.jpg"
    }

    DropArea {
        anchors.fill: parent;
        onDropped: {
            if(drop.hasUrls) {
                for(var i = 0; i < drop.urls.length; i++) {
                    //加入到播放列表
                }
                musicPlayer.play(drop.urls[0]);
                playButton.state = "playing";
            }
        }
    }

    Text {
        id: title
        anchors.centerIn: parent
        text: musicPlayer.title
        font.bold: true
        font.pointSize: 20
    }

    ProgressControlBar {
        id: progressControl
        backColor: "#aaa"
        anchors.left: parent.left
        anchors.leftMargin: 40
        anchors.right: timeText.left
        anchors.rightMargin: 20
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 60
        onPressedChanged: if (!pressed) musicPlayer.progress = value;

        Connections {
            target: musicPlayer
            onProgressChanged: {
                progressControl.setValue(musicPlayer.progress);
                timeText.update();
            }
            onFinished: playButton.state = "pausing";
            onError: print(errorString);
        }
    }

    Text {
        id: timeText
        anchors.verticalCenter: progressControl.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 20
        width: 120
        color: root.globalColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 12
        text: "00:00 / 00:00"
        property string duration: Api.time2string(musicPlayer.duration)

        function update() {
            text = Api.time2string(musicPlayer.progress * musicPlayer.duration) + " / " + duration;
        }
    }

    Widgets.ColorImage {
        id: playButton
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: progressControl.bottom
        anchors.topMargin: 8
        source: "qrc:/image/Player/pause.png"
        width: 32
        height: 32
        color: root.globalColor
        state: "pausing"
        states: [
            State {
                name: "playing"
                StateChangeScript {
                    script: {
                        musicPlayer.resume();
                        playButton.source = "qrc:/image/Player/pause.png";
                        playingEffect.running = true;
                    }
                }
            },
            State {
                name: "pausing"
                StateChangeScript {
                    script: {
                        musicPlayer.suspend();
                        playButton.source = "qrc:/image/Player/play.png";
                        playingEffect.running = false;
                    }
                }
            }
        ]

        property bool hovered: false

        Effects.RippleEffect {
            id: playingEffect
            anchors.fill: parent
            z: -1
            size: 70
            emitRate: 4
            source: "qrc:/image/ParticleImage/circle.png"
        }

        Widgets.ToolTip {
            text: qsTr(parent.state == "playing" ? "暂停" : "播放")
            visible: parent.hovered
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: parent
            onClicked: {
                if (playButton.state == "playing")
                    playButton.state = "pausing";
                else if (playButton.state == "pausing")
                    playButton.state = "playing";
            }
            onEntered: {
                parent.hovered = true;
                cursorShape = Qt.PointingHandCursor;
            }
            onExited: {
                parent.hovered = false;
                cursorShape = Qt.ArrowCursor;
            }
        }
    }

    Widgets.ColorImage {
        id: nextButton
        anchors.verticalCenter: playButton.verticalCenter
        anchors.left: playButton.right
        anchors.leftMargin: 24
        source: "qrc:/image/Player/next.png"
        width: 22
        height: 22
        color: root.globalColor
        property bool hovered: false

        Widgets.ToolTip {
            text: qsTr("下一首")
            visible: parent.hovered
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: parent
            onClicked: print("next")
            onEntered: {
                parent.hovered = true;
                cursorShape = Qt.PointingHandCursor;
            }
            onExited: {
                parent.hovered = false;
                cursorShape = Qt.ArrowCursor;
            }
        }
    }

    Widgets.ColorImage {
        id: volumeButton
        anchors.verticalCenter: playButton.verticalCenter
        anchors.left: nextButton.right
        anchors.leftMargin: 24
        source: "qrc:/image/Player/volume.png"
        width: 22
        height: 22
        color: root.globalColor
        property bool hovered: false

        Widgets.ToolTip {
            text: qsTr("音量调节")
            visible: parent.hovered
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: parent
            onClicked: print("volume")
            onEntered: {
                parent.hovered = true;
                cursorShape = Qt.PointingHandCursor;
            }
            onExited: {
                parent.hovered = false;
                cursorShape = Qt.ArrowCursor;
            }
        }
    }

    Widgets.ColorImage {
        id: prevButton
        anchors.verticalCenter: playButton.verticalCenter
        anchors.right: playButton.left
        anchors.rightMargin: 24
        source: "qrc:/image/Player/prev.png"
        width: 22
        height: 22
        color: root.globalColor
        property bool hovered: false

        Widgets.ToolTip {
            text: qsTr("上一首")
            visible: parent.hovered
        }

        MouseArea {
            anchors.fill: parent
            hoverEnabled: parent
            onClicked: print("prev")
            onEntered: {
                parent.hovered = true;
                cursorShape = Qt.PointingHandCursor;
            }
            onExited: {
                parent.hovered = false;
                cursorShape = Qt.ArrowCursor;
            }
        }
    }
}
