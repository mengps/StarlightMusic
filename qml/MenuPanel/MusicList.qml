import QtQuick 2.12
import an.music.model 1.0
import "../Api/Api.js" as Api

ListView {
    id: musicListView
    anchors.fill: parent
    clip: true
    model: musicPlayer.music.model
    header: Row {
        width: parent.width
        height: 40

        Text {
            height: 40
            width: parent.width / 4
            font.pointSize: 12
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: qsTr("歌曲")

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                property int mode: MusicModel.Less
                onClicked: {
                    if (mode == MusicModel.Less) {
                        musicPlayer.music.sort(MusicModel.Title, MusicModel.Less);
                        mode = MusicModel.Greater;
                    } else {
                        musicPlayer.music.sort(MusicModel.Title, MusicModel.Greater);
                        mode = MusicModel.Less;
                    }
                }
                onEntered: cursorShape = Qt.PointingHandCursor;
                onExited: cursorShape = Qt.ArrowCursor;
            }
        }

        Text {
            height: 40
            width: parent.width / 4
            font.pointSize: 12
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: qsTr("歌手")

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                property int mode: MusicModel.Less
                onClicked: {
                    if (mode == MusicModel.Less) {
                        musicPlayer.music.sort(MusicModel.Singer, MusicModel.Less);
                        mode = MusicModel.Greater;
                    } else {
                        musicPlayer.music.sort(MusicModel.Singer, MusicModel.Greater);
                        mode = MusicModel.Less;
                    }
                }
                onEntered: cursorShape = Qt.PointingHandCursor;
                onExited: cursorShape = Qt.ArrowCursor;
            }
        }

        Text {
            height: 40
            width: parent.width / 4
            font.pointSize: 12
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: qsTr("专辑")

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                property int mode: MusicModel.Less
                onClicked: {
                    if (mode == MusicModel.Less) {
                        musicPlayer.music.sort(MusicModel.Album, MusicModel.Less);
                        mode = MusicModel.Greater;
                    } else {
                        musicPlayer.music.sort(MusicModel.Album, MusicModel.Greater);
                        mode = MusicModel.Less;
                    }
                }
                onEntered: cursorShape = Qt.PointingHandCursor;
                onExited: cursorShape = Qt.ArrowCursor;
            }
        }

        Text {
            height: 40
            width: parent.width / 4
            font.pointSize: 12
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: qsTr("时长")

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                property int mode: MusicModel.Less
                onClicked: {
                    if (mode == MusicModel.Less) {
                        musicPlayer.music.sort(MusicModel.Duration, MusicModel.Less);
                        mode = MusicModel.Greater;
                    } else {
                        musicPlayer.music.sort(MusicModel.Duration, MusicModel.Greater);
                        mode = MusicModel.Less;
                    }
                }
                onEntered: cursorShape = Qt.PointingHandCursor;
                onExited: cursorShape = Qt.ArrowCursor;
            }
        }
    }
    delegate: Item {
        width: musicListView.width
        height: 40

        Row {
            anchors.fill: parent

            Text {
                height: 40
                width: parent.width / 4
                font.pointSize: 12
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: qsTr(modelData.title)
            }

            Text {
                height: 40
                width: parent.width / 4
                font.pointSize: 12
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: qsTr(modelData.singer)
            }

            Text {
                height: 40
                width: parent.width / 4
                font.pointSize: 12
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: qsTr(modelData.album)
            }

            Text {
                height: 40
                width: parent.width / 4
                font.pointSize: 12
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                text: Api.time2string(modelData.duration)
            }
        }
    }
}
