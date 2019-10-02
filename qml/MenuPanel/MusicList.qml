import QtQuick 2.12
import an.music.model 1.0
import "../Api/Api.js" as Api

ListView {
    id: musicListView

    property color headerColor: "#55FFFFFF"
    property color headerBorderColor: "#FFF"

    anchors.fill: parent
    clip: true
    spacing: 4
    model: musicPlayer.music.model
    header: Row {
        width: parent.width
        height: 54
        spacing: 2

        Rectangle {
            clip: true
            height: 40
            width: parent.width / 4 - parent.spacing
            color:  musicListView.headerColor
            border.color: musicListView.headerBorderColor
            radius: 4

            Text {
                anchors.centerIn: parent
                font.pointSize: 12
                text: qsTr("歌曲")
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                property int mode: MusicModel.Less
                onClicked: {
                    if (mode === MusicModel.Less) {
                        musicPlayer.music.sort(MusicModel.Title, MusicModel.Less);
                        mode = MusicModel.Greater;
                    } else {
                        musicPlayer.music.sort(MusicModel.Title, MusicModel.Greater);
                        mode = MusicModel.Less;
                    }
                }
                onEntered: {
                    parent.color = Qt.darker(musicListView.headerColor);
                    cursorShape = Qt.PointingHandCursor;
                }
                onExited: {
                    parent.color = musicListView.headerColor;
                    cursorShape = Qt.ArrowCursor;
                }
            }
        }

        Rectangle {
            clip: true
            height: 40
            width: parent.width / 4 - parent.spacing
            color: musicListView.headerColor
            border.color: musicListView.headerBorderColor
            radius: 4

            Text {
                anchors.centerIn: parent
                font.pointSize: 12
                text: qsTr("歌手")
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                property int mode: MusicModel.Less
                onClicked: {
                    if (mode === MusicModel.Less) {
                        musicPlayer.music.sort(MusicModel.Singer, MusicModel.Less);
                        mode = MusicModel.Greater;
                    } else {
                        musicPlayer.music.sort(MusicModel.Singer, MusicModel.Greater);
                        mode = MusicModel.Less;
                    }
                }
                onEntered: {
                    parent.color = Qt.darker(musicListView.headerColor);
                    cursorShape = Qt.PointingHandCursor;
                }
                onExited: {
                    parent.color = musicListView.headerColor;
                    cursorShape = Qt.ArrowCursor;
                }
            }
        }

        Rectangle {
            clip: true
            height: 40
            width: parent.width / 4 - parent.spacing
            color:  musicListView.headerColor
            border.color: musicListView.headerBorderColor
            radius: 4

            Text {
                anchors.centerIn: parent
                font.pointSize: 12
                text: qsTr("专辑")
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                property int mode: MusicModel.Less
                onClicked: {
                    if (mode === MusicModel.Less) {
                        musicPlayer.music.sort(MusicModel.Album, MusicModel.Less);
                        mode = MusicModel.Greater;
                    } else {
                        musicPlayer.music.sort(MusicModel.Album, MusicModel.Greater);
                        mode = MusicModel.Less;
                    }
                }
                onEntered: {
                    parent.color = Qt.darker(musicListView.headerColor);
                    cursorShape = Qt.PointingHandCursor;
                }
                onExited: {
                    parent.color = musicListView.headerColor;
                    cursorShape = Qt.ArrowCursor;
                }
            }
        }

        Rectangle {
            clip: true
            height: 40
            width: parent.width / 4 - parent.spacing
            color:  musicListView.headerColor
            border.color: musicListView.headerBorderColor
            radius: 4

            Text {
                anchors.centerIn: parent
                font.pointSize: 12
                text: qsTr("时长")
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                property int mode: MusicModel.Less
                onClicked: {
                    if (mode === MusicModel.Less) {
                        musicPlayer.music.sort(MusicModel.Duration, MusicModel.Less);
                        mode = MusicModel.Greater;
                    } else {
                        musicPlayer.music.sort(MusicModel.Duration, MusicModel.Greater);
                        mode = MusicModel.Less;
                    }
                }
                onEntered: {
                    parent.color = Qt.darker(musicListView.headerColor);
                    cursorShape = Qt.PointingHandCursor;
                }
                onExited: {
                    parent.color = musicListView.headerColor;
                    cursorShape = Qt.ArrowCursor;
                }
            }
        }
    }
    delegate: Rectangle {
        clip: true
        height: 30
        width: parent.width
        color: "transparent"
        radius: 2

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onDoubleClicked: musicPlayer.play(modelData.filename)
            onEntered: {
                parent.color = musicListView.headerColor;
            }
            onExited: {
                parent.color = "transparent";
            }
        }

        FontMetrics {
            id: fontMetrics
            font.pointSize: 11
        }

        Row {
            width: parent.width
            height: 30
            spacing: 2

            Item {
                height: 30
                width: parent.width / 4 - parent.spacing

                Text {
                    clip: true
                    height: 30
                    width: parent.width - 20
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pointSize: 11
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: fontMetrics.advanceWidth(text) > width ? Text.AlignLeft : Text.AlignHCenter
                    text: qsTr(modelData.title)
                }
            }

            Item {
                height: 30
                width: parent.width / 4 - parent.spacing

                Text {
                    clip: true
                    height: 30
                    width: parent.width - 20
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pointSize: 11
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: fontMetrics.advanceWidth(text) > width ? Text.AlignLeft : Text.AlignHCenter
                    text: qsTr(modelData.singer)
                }
            }

            Item {
                height: 30
                width: parent.width / 4 - parent.spacing

                Text {
                    clip: true
                    height: 30
                    width: parent.width - 20
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pointSize: 11
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: fontMetrics.advanceWidth(text) > width ? Text.AlignLeft : Text.AlignHCenter
                    text: qsTr(modelData.album)
                }
            }

            Item {
                height: 30
                width: parent.width / 4

                Text {
                    clip: true
                    height: 30
                    width: parent.width - 20
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pointSize: 11
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: Api.time2string(modelData.duration)
                }
            }
        }
    }
}
