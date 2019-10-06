import QtQuick 2.12
import QtQuick.Controls 2.12
import Qt.labs.platform 1.1
import "../Widgets" as Widgets

Item {
    id: root

    FileDialog {
        id: fileDialog
        fileMode: FileDialog.OpenFiles
        nameFilters: ["皮肤文件 (*.skin)", "XML文件 (*.xml *.XML)"]
        onAccepted: {
            for (let i = 0; i < files.length; i++) {
                let baseName = fileApi.baseName(files[i]);
                let fileName = fileApi.toFileString(files[i]);
                model.insert(model.count - 1, {"skinTitle": baseName, "skinName": fileName})
            }


        }
    }

    Component {
        id: delegate

        Item {
            id: wrapper
            width: 120
            height: 120

            Rectangle {
                anchors.fill: parent
                anchors.margins: 12
                clip: true
                radius: 4
                color: hovered ? skinManager.menuBackColor : skinManager.menuHoverColor
                border.color: skinManager.borderColor
                property bool hovered: false

                Widgets.ColorImage {
                    width: 24
                    height: 24
                    source: "qrc:/image/skinSelected.png"
                    visible: skinName == musicPlayer.skinName
                    color: skinManager.borderColor
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                }

                Widgets.ColorImage {
                    width: 32
                    height: 32
                    source: "qrc:/image/Player/add.png"
                    visible: skinName == "##custom_skin"
                    color: skinManager.borderColor
                    anchors.centerIn: parent
                }

                Text {
                    anchors.centerIn: parent
                    color: skinManager.textColor
                    text: skinTitle
                    visible: skinName != "##custom_skin"
                    font.pointSize: 11
                }

                Widgets.ToolTip {
                    visible: parent.hovered
                    text: skinTitle
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: parent.hovered = true;
                    onExited: parent.hovered = false;
                    onClicked: {
                        if(skinName == "##custom_skin") {
                            fileDialog.open();
                        }
                        else musicPlayer.skinName = skinName;
                    }
                }
            }
        }
    }

    ListModel {
        id: model
        ListElement {
            skinTitle: qsTr("经典黑")
            skinName: ":/skin/default_black.skin"
        }
        ListElement {
            skinTitle: qsTr("经典白")
            skinName: ":/skin/default_white.skin"
        }
        ListElement {
            skinTitle: qsTr("经典蓝")
            skinName: ":/skin/default_blue.skin"
        }
        ListElement {
            skinTitle: qsTr("自定义皮肤")
            skinName: "##custom_skin"
        }
    }

    GridView {
        anchors.fill: parent   
        cellHeight: 120
        cellWidth: 120
        model: model
        delegate: delegate
        ScrollBar.vertical: ScrollBar { }
    }
}

