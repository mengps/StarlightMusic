import QtQuick 2.12
import an.lyrics.model 1.0
import "../Widgets" as Widgets

Rectangle {
    id: root
    clip: true
    color: skinManager.detailColor
    property bool isVisiable: false

    function display() {
        isVisiable = true;
        animation.needStop = true;
        animation.to = parent.height;
        animation.restart();
    }

    function hide() {
        isVisiable = false;
        animation.needStop = false;
        animation.to = 0;
        animation.restart();
    }

    NumberAnimation on height {
        id: animation
        duration: 400
        running: false
        onStopped: binder.when = needStop;
        property bool needStop: false
    }

    Binding {
        id: binder
        when: false
        target: root
        property: "height"
        value: root.parent.height
    }

    Widgets.Button {
        id: shrinkButton
        anchors.left: parent.left
        anchors.leftMargin: 40
        anchors.top: parent.top
        anchors.topMargin: 40
        source: "qrc:/image/Player/shrink.png"
        width: 30
        height: 30
        color: skinManager.buttonColor
        toolTip: qsTr("收起详情")
        visible: root.isVisiable
        onClicked: root.hide();
    }

    Image {
        id: playbill
        width: parent.width / 3.5
        height: width
        mipmap: true
        sourceSize: Qt.size(250, 250)
        source: "qrc:/image/music.png"
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: -40
        anchors.left: parent.left
        anchors.leftMargin: 100

        Connections {
            target: musicPlayer
            onPlaybillChanged: {
                playbill.source = "image://playbill/" + Date.now();
            }
        }
    }

    Text {
        id: title
        height: 35
        anchors.left: playbill.right
        anchors.leftMargin: 50
        anchors.right: parent.right
        anchors.rightMargin: 50
        anchors.top: parent.top
        anchors.topMargin: 60
        color: skinManager.textColor
        text: musicPlayer.title
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.bold: true
        font.pointSize: 15
    }

    Text {
        id: singer
        height: 25
        anchors.left: title.left
        anchors.right: title.right
        anchors.top: title.bottom
        anchors.topMargin: 8
        color: Qt.darker(title.color, 1.2)
        text: musicPlayer.singer == "" ? "" : qsTr("歌手: ") + musicPlayer.singer
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 11
    }

    Text {
        id: album
        height: 25
        anchors.left: singer.left
        anchors.right: singer.right
        anchors.top: singer.bottom
        anchors.topMargin: 2
        color:singer.color
        text: musicPlayer.album == "" ? "" : qsTr("专辑: ") + musicPlayer.album
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 11
    }

    Component {
        id: delegate

        Item {
            width: lyricsView.width
            height: 30

            Text {
                text: lyric
                font.bold: lyricsView.currentIndex == index ? true : false
                font.pointSize: lyricsView.currentIndex == index ? 12 : 10
                color: lyricsView.currentIndex == index ? title.color : Qt.lighter(title.color, 1.8)
                anchors.centerIn: parent
            }
        }
    }

    ListView {
        id: lyricsView
        model: musicPlayer.lyrics.model
        clip: true
        width: 400
        topMargin: 80
        spacing: 5
        bottomMargin: 150
        contentY: currentIndex * 35 - 80
        anchors.horizontalCenter: singer.horizontalCenter
        anchors.top: album.bottom
        anchors.topMargin: 30
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 110
        delegate: delegate
        currentIndex: musicPlayer.lyricIndex
        highlight: Rectangle{
            height: 30
            color: skinManager.textColor
            width: lyricsView.width
            opacity: 0.1
            radius: 15
            Behavior on y {
                NumberAnimation {
                    duration: 600
                }
            }
        }
        highlightFollowsCurrentItem: true
    }
}
