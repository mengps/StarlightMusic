import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import an.music 1.0
import "Api/Api.js" as Api
import "Effects" as Effects
import "Widgets" as Widgets
import "DetailPanel"
import "MenuPanel"
import "MainPanel"

Window {
    id: mainPanel
    visible: true
    width: 750
    height: 500
    minimumWidth: 750
    minimumHeight: 500
    title: qsTr("StarlightMusic")
    color: skinManager.globalColor
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
        blur: detailPanel.isVisiable || menuPanel.content.isVisible
        fillMode: Image.PreserveAspectCrop
        source: skinManager.backgroundImage
        sourceSize: Qt.size(900, 600)
        focus: true
        Keys.enabled: true
        Keys.onPressed: {
            event.accepted = true;
            switch (event.key) {
            case Qt.Key_Space:
                playButton.clicked();
                break;
            case Qt.Key_Left:
                if (event.modifiers & Qt.ControlModifier) {
                    prevButton.clicked();
                } else {
                    let v1 = progressControl.value - 0.05 > 0.0000001 ? progressControl.value - 0.05 : 0.0;
                    progressControl.setProgress(v1);
                }
                break;
            case Qt.Key_Right:
                if (event.modifiers & Qt.ControlModifier) {
                    nextButton.clicked();
                } else {
                    let v2 = progressControl.value + 0.05 < 0.9999999 ? progressControl.value + 0.05 : 1.0;
                    progressControl.setProgress(v2);
                }
                break;
            case Qt.Key_Tab:
                menuPanel.listMenuClick();
                break;
            case Qt.Key_QuoteLeft:
                if (detailPanel.isVisiable) {
                    detailPanel.hide();
                } else {
                    detailPanel.display();
                }
                break;
            default:
                event.accepted = false;
                break;
            }
        }
    }

    MenuPanel {
        id: menuPanel
        anchors.fill: parent
        contentHeight: progressControl.y
    }

    DetailPanel {
        id: detailPanel
        anchors.bottom: parent.bottom
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
                musicPlayer.addMusicList(drop.urls);
            }
        }
    }

    Image {
        id: playbill
        x: menuPanel.menuBack.width
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
            color: skinManager.buttonColor
            visible: parent.hovered && !detailPanel.isVisiable
        }

        Widgets.ColorImage {
            anchors.centerIn: parent
            width: 25
            height: 25
            color: skinManager.buttonColor
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
        anchors.left: playbill.right
        anchors.leftMargin: 12
        anchors.right: timeText.left
        anchors.rightMargin: 12
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 60
        onPressedChanged: {
            if (!pressed) {
                setProgress(value);
            }
        }

        function setProgress(v) {
            musicPlayer.volume = 0;
            musicPlayer.progress = v;
            if (!volumeButton.mute) {
                //音量缓冲
                buffAnimation.to = volumeBar.value;
                buffAnimation.needStop = false;
                buffAnimation.restart();
            }
        }

        Connections {
            target: musicPlayer
            onProgressChanged: {
                progressControl.setValue(musicPlayer.progress);
                timeText.update();
            }
        }
    }

    Widgets.FlowText {
        id: musicTitle
        height: 30
        anchors.left: progressControl.left
        anchors.leftMargin: 4
        anchors.right: prevButton.left
        anchors.rightMargin: 20
        anchors.top: progressControl.bottom
        anchors.topMargin: 10
        color: skinManager.textColor
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
        color: skinManager.textColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 12
        Component.onCompleted: text = "00:00 / " + duration;
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
        color: skinManager.buttonColor
        state: "pausing"
        states: [
            State {
                name: "playing"
                StateChangeScript {
                    script: {
                        playButton.source = "qrc:/image/Player/pause.png";
                        playingEffect.running = true;
                        progressControl.effect.running = true;
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
                        progressControl.effect.running = false;
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
            else state = "playing";
        }

        Connections {
            target: musicPlayer
            onPlayingChanged: {
                if (playButton.state == "pausing" && musicPlayer.playing)
                    playButton.state = "playing";
            }
            onFinished: {
                playButton.state = "pausing";
                musicPlayer.playNext();
            }
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
            source: skinManager.buttonParticleImage
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
        color: skinManager.buttonColor
        toolTip: qsTr("上一首")
        onClicked: musicPlayer.playPrev();
    }

    Widgets.Button {
        id: nextButton
        anchors.verticalCenter: playButton.verticalCenter
        anchors.left: playButton.right
        anchors.leftMargin: 24
        source: "qrc:/image/Player/next.png"
        width: 22
        height: 22
        color: skinManager.buttonColor
        toolTip: qsTr("下一首")
        onClicked: musicPlayer.playNext();
    }

    Widgets.Button {
        id: loopButton
        anchors.verticalCenter: playButton.verticalCenter
        anchors.left: nextButton.right
        anchors.leftMargin: 24
        source: {
            if (musicPlayer.playMode == MusicPlayer.Single)
                return "qrc:/image/Player/single.png";
            else if (musicPlayer.playMode == MusicPlayer.Random)
                return "qrc:/image/Player/random.png";
            else return "qrc:/image/Player/order.png";
        }
        width: 22
        height: 22
        color: skinManager.buttonColor
        toolTip: {
            if (musicPlayer.playMode == MusicPlayer.Single)
                return "单曲循环";
            else if (musicPlayer.playMode == MusicPlayer.Random)
                return "随机循环";
            else return "顺序循环";
        }
        onClicked: loopMenu.open();

        LoopMenu {
            id: loopMenu
        }
    }

    Widgets.Button {
        id: volumeButton
        anchors.verticalCenter: playButton.verticalCenter
        anchors.left: loopButton.right
        anchors.leftMargin: 24
        source: "qrc:/image/Player/volume.png"
        width: 22
        height: 22
        color: skinManager.buttonColor
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
        text: qsTr("当前音量 : ") + value
        onValueChanged: {
            if (value == 0) volumeButton.setMute();
            else volumeButton.setUnmute();
        }
        Component.onCompleted: value = musicPlayer.volume;
    }
}
