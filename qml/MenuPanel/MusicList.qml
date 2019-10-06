import QtQuick 2.12
import QtQuick.Controls 2.12
import an.music.model 1.0
import "../Api/Api.js" as Api
import "../Widgets" as Widgets

Item {
    id: root

    Row {
        id: headerItem
        width: parent.width
        height: 54
        spacing: 2

        Rectangle {
            clip: true
            height: 40
            width: parent.width / 4 - parent.spacing
            color: hovered ? skinManager.headerHoverColor : skinManager.headerColor
            border.color: skinManager.headerBorderColor
            radius: 4
            property bool hovered: false

            Text {
                anchors.centerIn: parent
                font.pointSize: 12
                color: skinManager.musicTextColor
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
                    parent.hovered = true;
                    cursorShape = Qt.PointingHandCursor;
                }
                onExited: {
                    parent.hovered = false;
                    cursorShape = Qt.ArrowCursor;
                }
            }
        }

        Rectangle {
            clip: true
            height: 40
            width: parent.width / 4 - parent.spacing
            color: hovered ? skinManager.headerHoverColor : skinManager.headerColor
            border.color: skinManager.headerBorderColor
            radius: 4
            property bool hovered: false

            Text {
                anchors.centerIn: parent
                font.pointSize: 12
                color: skinManager.musicTextColor
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
                    parent.hovered = true;
                    cursorShape = Qt.PointingHandCursor;
                }
                onExited: {
                    parent.hovered = false;
                    cursorShape = Qt.ArrowCursor;
                }
            }
        }

        Rectangle {
            clip: true
            height: 40
            width: parent.width / 4 - parent.spacing
            color: hovered ? skinManager.headerHoverColor : skinManager.headerColor
            border.color: skinManager.headerBorderColor
            radius: 4
            property bool hovered: false

            Text {
                anchors.centerIn: parent
                font.pointSize: 12
                color: skinManager.musicTextColor
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
                    parent.hovered = true;
                    cursorShape = Qt.PointingHandCursor;
                }
                onExited: {
                    parent.hovered = false;
                    cursorShape = Qt.ArrowCursor;
                }
            }
        }

        Rectangle {
            clip: true
            height: 40
            width: parent.width / 4 - parent.spacing
            color: hovered ? skinManager.headerHoverColor : skinManager.headerColor
            border.color: skinManager.headerBorderColor
            radius: 4
            property bool hovered: false

            Text {
                anchors.centerIn: parent
                font.pointSize: 12
                color: skinManager.musicTextColor
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

    Component {
        id: delegate

        Rectangle {
            clip: true
            height: 30
            width: parent.width
            color:  "transparent"
            radius: 2
            property bool hovered: false

            Rectangle {
                id: indicator
                width: 10
                height: 10
                radius: 5
                anchors.verticalCenter: parent.verticalCenter
                color: modelData == musicPlayer.curMusic ? skinManager.musicTextSelectedColor : "transparent"
            }

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true

                function contains(item, x, y) {
                    return x > item.x && x < (item.x + item.width) &&
                           y > item.y && y < (item.y + item.height);
                }

                onDoubleClicked: {
                    musicPlayer.curMusic = modelData;
                    musicPlayer.play(modelData.filename)
                }
                onEntered: {
                    parent.hovered = true;
                    parent.color = skinManager.headerColor;
                }
                onExited: {
                    parent.hovered = false;
                    parent.color = "transparent";
                }
                onPositionChanged: {
                    let pos = Qt.point(mouse.x, mouse.y);

                    if (contains(titleItem, mouse.x, mouse.y)) {
                        musicTooltip.text = musicTitle.text;
                    } else if (contains(singerItem, mouse.x, mouse.y)) {
                        musicTooltip.text = musicSinger.text;
                    } else if (contains(albumItem, mouse.x, mouse.y)) {
                        musicTooltip.text = musicAlbum.text;
                    } else if (contains(durationItem, mouse.x, mouse.y)) {
                        musicTooltip.text = musicDuration.text;
                    } else return;

                    musicTooltip.x = (mouse.x + musicTooltip.width) > parent.width ?
                                     parent.width - musicTooltip.width : mouse.x;
                    musicTooltip.visibleChanged();
                }
            }

            Widgets.ToolTip {
                id: musicTooltip
                visible: parent.hovered
            }

            FontMetrics {
                id: fontMetrics
                font.pointSize: 11
                font.bold: modelData == musicPlayer.curMusic ? true : false
            }

            Item {
                id: titleItem
                anchors.left: parent.left
                height: 30
                width: parent.width / 4

                Text {
                    id: musicTitle
                    clip: true
                    height: 30
                    width: parent.width - 30
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: modelData == musicPlayer.curMusic ? skinManager.musicTextSelectedColor : skinManager.musicTextColor
                    font.pointSize: 11
                    font.bold: modelData == musicPlayer.curMusic ? true : false
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: fontMetrics.advanceWidth(text) > width ? Text.AlignLeft : Text.AlignHCenter
                    text: qsTr(modelData.title)
                }
            }

            Item {
                id: singerItem
                anchors.left: titleItem.right
                height: 30
                width: parent.width / 4

                Text {
                    id: musicSinger
                    clip: true
                    height: 30
                    width: parent.width - 30
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: modelData == musicPlayer.curMusic ? skinManager.musicTextSelectedColor : skinManager.musicTextColor
                    font.pointSize: 11
                    font.bold: modelData == musicPlayer.curMusic ? true : false
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: fontMetrics.advanceWidth(text) > width ? Text.AlignLeft : Text.AlignHCenter
                    text: qsTr(modelData.singer)
                }
            }

            Item {
                id: albumItem
                anchors.left: singerItem.right
                height: 30
                width: parent.width / 4

                Text {
                    id: musicAlbum
                    clip: true
                    height: 30
                    width: parent.width - 30
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: modelData == musicPlayer.curMusic ? skinManager.musicTextSelectedColor : skinManager.musicTextColor
                    font.pointSize: 11
                    font.bold: modelData == musicPlayer.curMusic ? true : false
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: fontMetrics.advanceWidth(text) > width ? Text.AlignLeft : Text.AlignHCenter
                    text: qsTr(modelData.album)
                }
            }

            Item {
                id: durationItem
                anchors.left: albumItem.right
                height: 30
                width: parent.width / 4

                Text {
                    id: musicDuration
                    clip: true
                    height: 30
                    width: parent.width - 30
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: modelData == musicPlayer.curMusic ? skinManager.musicTextSelectedColor : skinManager.musicTextColor
                    font.pointSize: 11
                    font.bold: modelData == musicPlayer.curMusic ? true : false
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: Api.time2string(modelData.duration)
                }
            }
        }
    }

    ListView {
        id: musicListView
        anchors.top: headerItem.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        clip: true
        spacing: 4
        model: musicPlayer.music.model
        delegate: delegate
        ScrollBar.vertical: ScrollBar { }
    }
}

