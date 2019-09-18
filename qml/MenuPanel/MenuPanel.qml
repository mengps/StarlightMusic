import QtQuick 2.12
import "../Widgets" as Widgets

Item {
    id: root

    property bool isVisible: true
    property bool menuVisible: false
    property real menuWidth: 45
    property real menuHeight: 50
    property color menuColor: "#fff"
    property alias back: menuBack

    onMenuVisibleChanged: {
        if(!menuVisible) {
            content.hide();
        }
    }

    function display() {
        menuVisible = true;
        animation.index = 0;
        animation.isHide = false;
        animation.start();
    }

    function hide() {
        menuVisible = false;
        animation.index = 6;
        animation.isHide = true;
        animation.start();
    }

    NumberAnimation {
        id: animation
        property: "width"
        duration: 150
        running: false
        property int index: 0
        property bool isHide: false
        onStopped: {
            if (!isHide) {
                switch (++index) {
                    case 1: to = root.menuWidth; target = menuBack; start(); break;
                    case 2: to = root.menuWidth; target = closeMenu; start(); break;
                    case 3: to = root.menuWidth; target = listMenu; start(); break;
                    case 4: to = root.menuWidth; target = historyMenu; start(); break;
                    case 5: to = root.menuWidth; target = skinMenu; start(); break;
                    default: break;
                }
            } else {
                switch (--index) {
                    case 0: if (root.isVisible && openButton.needVisible) {
                            openButton.needVisible = false;
                            openButton.visible = true;
                        } break;
                    case 1: to = 0; target = menuBack; start(); break;
                    case 2: to = 0; target = closeMenu; start(); break;
                    case 3: to = 0; target = listMenu; start(); break;
                    case 4: to = 0; target = historyMenu; start(); break;
                    case 5: if (!openButton.needVisible) {
                            openButton.visible = false;
                        } to = 0; target = skinMenu; start(); break;
                    default: break;
                }
            }
        }
    }

    Rectangle {
        id: menuBack
        color: "#33888888"
        width: 0
        height: parent.height
    }

    Widgets.Button {
        id: openButton
        visible: false
        color: root.menuColor
        x: 20
        anchors.top: closeMenu.top
        width: 30
        height: 30
        source: "qrc:/image/Player/open.png"
        toolTip: qsTr("打开菜单")
        property bool needVisible: false
        onClicked: {
            visible = false;
            root.display();
        }
    }

    MenuButton {
        id: closeMenu
        source: "qrc:/image/Player/close.png"
        color: "#aaaaaaaa"
        imageColor: root.menuColor
        width: 0
        height: parent.menuHeight
        anchors.top: parent.top
        anchors.topMargin: 100
        toolTip: qsTr("关闭菜单")
        onClicked: {
            root.hide();
            if (root.isVisible) {
                openButton.needVisible = true;
            }
        }
    }

    Rectangle {
        id: content
        anchors.left: menuBack.right
        width: 0
        height: root.height
        color: "#aaaaaaaa"
        property bool isVisible: false

        function display() {
            if (root.isVisible) {
                isVisible = true;
                contentAnimation.to = root.width - root.menuWidth;
                contentAnimation.restart();
            }
        }
        function hide() {
            isVisible = false;
            contentAnimation.to = 0;
            contentAnimation.restart();
        }

        NumberAnimation on width{
            id: contentAnimation
            running: false
            duration: 500
            easing.type: Easing.InOutQuad
        }
    }

    MenuButton {
        id: listMenu
        source: "qrc:/image/Player/content.png"
        color: "#aaaaaaaa"
        imageColor: root.menuColor
        toolTip: qsTr("播放列表")
        width: 0
        height: parent.menuHeight
        anchors.top: closeMenu.bottom
        onClicked: {
            if (content.isVisible) {
                content.hide();
            } else {
                content.display();
            }
        }
    }

    MenuButton {
        id: historyMenu
        source: "qrc:/image/Player/history.png"
        color: "#aaaaaaaa"
        imageColor: root.menuColor
        toolTip: qsTr("历史播放记录")
        width: 0
        height: parent.menuHeight
        anchors.top: listMenu.bottom
    }

    MenuButton {
        id: skinMenu
        source: "qrc:/image/Player/skin.png"
        color: "#aaaaaaaa"
        imageColor: root.menuColor
        toolTip: qsTr("更换皮肤")
        width: 0
        height: parent.menuHeight
        anchors.top: historyMenu.bottom
    }
}
