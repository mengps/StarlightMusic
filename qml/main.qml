import QtQuick 2.12
import QtQuick.Controls 2.12
import an.window 1.0
import "Api/Api.js" as Api
import "Effects" as Effects
import "Widgets" as Widgets
import "DetailPanel"
import "MenuPanel"

FramelessWindow {
    id: mainPanel
    visible: true
    width: 900
    height: 600/*
    opacity: 0
    minimumWidth: 900
    minimumHeight: 600
    title: qsTr("StarlightMusic")*/
    //需要增加一个全局的SetttingManager
    property color globalColor: "white"
    Component.onCompleted: {
        startAnimation.start();
        menuPanel.display();
    }

    NumberAnimation {
        id: startAnimation
        running: false
        target: mainPanel
        property: "opacity"
        from: 0
        to: 1
        duration: 600
        easing.type: Easing.InQuad
        onStopped: mainPanel.show();
    }

    Widgets.BlurImage {
        id: background
        anchors.fill: parent
        blur: detailPanel.isVisiable
        fillMode: Image.PreserveAspectCrop
        source: "qrc:/image/background.jpg"
        sourceSize: Qt.size(900, 600)
    }

    MenuPanel {
        id: menuPanel
        anchors.left: parent.left
        height: parent.height
        width: parent.width
    }

    DetailPanel {
        id: detailPanel
        anchors.bottom: parent.bottom
        color: "#88000000"
        width: parent.width
        height: 0
        onIsVisiableChanged: {
            if (isVisiable) {
                menuPanel.isVisible = false;
                menuPanel.hide();
            } else {
                menuPanel.isVisible = true;
                menuPanel.display();
            }
        }
    }

    DropArea {
        anchors.fill: parent
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

    Image {
        id: playbill
        x: menuPanel.back.width
        anchors.bottom: menuPanel.bottom
        sourceSize: Qt.size(100, 100)
        source: "qrc:/image/music.png"
        width: 80
        height: 80
        mipmap: true
        property bool hovered: false
        Connections {
            target: musicPlayer
            onPlaybillChanged: {
                playbill.source = "image://playbill/" + Date.now();
            }
        }

        Widgets.ToolTip {
            id: toolTip
            text: qsTr("展开详情")
            visible: parent.hovered && !detailPanel.isVisiable
        }

        Rectangle {
            anchors.fill: parent
            opacity: 0.5
            color: "#aaa"
            visible: parent.hovered && !detailPanel.isVisiable
        }

        Widgets.ColorImage {
            anchors.centerIn: parent
            width: 25
            height: 25
            color: mainPanel.globalColor
            source: "qrc:/image/Player/expand.png"
            visible: parent.hovered && !detailPanel.isVisiable
        }

        MouseArea {
            anchors.fill: parent
            visible: !detailPanel.isVisiable
            hoverEnabled: true
            onClicked: detailPanel.display();
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

    ProgressControlBar {
        id: progressControl
        backColor: "#aaa"
        anchors.left: playbill.right
        anchors.leftMargin: 12
        anchors.right: timeText.left
        anchors.rightMargin: 12
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 60
        onPressedChanged: {
            if (!pressed) {
                musicPlayer.volume = 0;
                musicPlayer.progress = value;
                if (!volumeButton.mute) {
                    //音量缓冲
                    buffAnimation.to = volumeBar.value;
                    buffAnimation.needStop = false;
                    buffAnimation.restart();
                }
            }
        }

        Connections {
            target: musicPlayer
            onProgressChanged: {
                progressControl.setValue(musicPlayer.progress);
                timeText.update();
            }
            onFinished: {
                //musicPlayer.play(musicPlayer.music);
                playButton.state = "pausing";
            }
            onError: print(errorString);
        }
    }

    Widgets.FlowText {
        id: musicInfo
        width: 180
        height: 30
        anchors.left: progressControl.left
        anchors.leftMargin: 4
        anchors.top: progressControl.bottom
        anchors.topMargin: 10
        color: mainPanel.globalColor
        text: musicPlayer.title == "" ? "" :  musicPlayer.title + " - " + musicPlayer.singer
        font.bold: true
        font.pointSize: 11
    }

    Text {
        id: timeText
        anchors.verticalCenter: progressControl.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 12
        width: 120
        color: mainPanel.globalColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 12
        text: "00:00 / 00:00"
        property string duration: Api.time2string(musicPlayer.duration)

        function update() {
            text = Api.time2string(musicPlayer.progress * musicPlayer.duration) + " / " + duration;
        }
    }

    Widgets.Button {
        id: playButton
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: progressControl.bottom
        anchors.topMargin: 8
        width: 34
        height: 34
        color: mainPanel.globalColor
        state: "pausing"
        states: [
            State {
                name: "playing"
                StateChangeScript {
                    script: {
                        playButton.source = "qrc:/image/Player/pause.png";
                        playingEffect.running = true;
                        musicPlayer.volume = 0;
                        musicPlayer.resume();
                        buffAnimation.to = volumeBar.value;
                        buffAnimation.needStop = false;
                        buffAnimation.restart();
                    }
                }
            },
            State {
                name: "pausing"
                StateChangeScript {
                    script: {
                        playButton.source = "qrc:/image/Player/play.png";
                        playingEffect.running = false;
                        buffAnimation.to = 0;
                        buffAnimation.needStop = true;
                        buffAnimation.restart();
                    }
                }
            }
        ]
        toolTip: state == "playing" ? qsTr("暂停") : qsTr("播放")
        onClicked: {
            if (state == "playing") state = "pausing";
            else if (state == "pausing") state = "playing";
        }

        NumberAnimation {
            //音量缓沖,不会瞬间降至0,也不会瞬间拉高
            id: buffAnimation
            target: musicPlayer
            property: "volume"
            duration: 800
            running: false
            easing.type: Easing.Linear
            property bool needStop: false
            onStopped: if (needStop) musicPlayer.suspend();
        }

        Effects.RippleEffect {
            id: playingEffect
            anchors.fill: parent
            z: -1
            size: 70
            emitRate: 4
            source: "qrc:/image/ParticleImage/circle.png"
        }
    }

    Widgets.Button {
        id: prevButton
        anchors.verticalCenter: playButton.verticalCenter
        anchors.right: playButton.left
        anchors.rightMargin: 24
        source: "qrc:/image/Player/prev.png"
        width: 22
        height: 22
        color: mainPanel.globalColor
        toolTip: qsTr("上一首")
        onClicked: print("prev")
    }

    Widgets.Button {
        id: nextButton
        anchors.verticalCenter: playButton.verticalCenter
        anchors.left: playButton.right
        anchors.leftMargin: 24
        source: "qrc:/image/Player/next.png"
        width: 22
        height: 22
        color: mainPanel.globalColor
        toolTip: qsTr("下一首")
        onClicked: print("next")
    }

    Widgets.Button {
        id: volumeButton
        anchors.verticalCenter: playButton.verticalCenter
        anchors.left: nextButton.right
        anchors.leftMargin: 24
        source: "qrc:/image/Player/volume.png"
        width: 22
        height: 22
        color: mainPanel.globalColor
        toolTip: qsTr("音量调节")
        onClicked: {
            if (!mute) {
                volumeButton.setMute();
            } else {
                volumeButton.setUnmute();
            }
        }

        property bool mute: false
        property bool hovered: false

        function setMute() {
            volumeButton.source = "qrc:/image/Player/mute.png";
            volumeButton.mute = true;
            musicPlayer.volume = 0;
        }

        function setUnmute() {
            volumeButton.source = "qrc:/image/Player/volume.png";
            volumeButton.mute = false;
            musicPlayer.volume = volumeBar.value;
        }
    }

    Widgets.ProgressBar {
        id: volumeBar
        anchors.verticalCenter: volumeButton.verticalCenter
        anchors.left: volumeButton.right
        anchors.leftMargin: 12
        width: 100
        height: 20
        value: 100
        handleColor: mainPanel.globalColor
        text: qsTr("当前音量 : ") + value
        onValueChanged: {
            if (value == 0) volumeButton.setMute();
            else volumeButton.setUnmute();
        }
    }
}
