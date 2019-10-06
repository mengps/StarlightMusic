import QtQuick 2.12
import QtQuick.Controls 2.12
import "../Widgets" as Widgets

Item {
    id: root

    property bool isVisible: true
    property bool menuVisible: false
    property real menuWidth: 45
    property real menuHeight: 50
    property real contentHeight: 400
    property alias menuBack: menuBack
    property alias content: content

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

    function listMenuClick() {
        listMenu.clicked();
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
        color: skinManager.menuBackColor
        width: 0
        height: parent.height
    }

    Widgets.Button {
        id: openButton
        visible: false
        color: skinManager.menuColor
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
        hoverColor: skinManager.menuHoverColor
        imageColor: skinManager.menuColor
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
        clip: true
        color: skinManager.menuHoverColor
        property bool isVisible: false

        function display() {
            if (root.menuVisible) {
                isVisible = true;
                contentAnimation.needStop = true;
                contentAnimation.to = root.width - root.menuWidth;
                contentAnimation.restart();
            }
        }
        function hide() {
            isVisible = false;
            contentAnimation.needStop = false;
            contentAnimation.to = 0;
            contentAnimation.restart();
        }

        NumberAnimation on width {
            id: contentAnimation
            running: false
            duration: 500
            easing.type: Easing.InOutQuad
            onStopped: {
                binder.when = needStop;
                if (!needStop) {
                    musicList.visible = false;
                    skinList.visible = false;
                }
            }
            property bool needStop: false
        }

        Binding {
            id: binder
            when: false
            target: content
            property: "width"
            value: root.width - root.menuWidth
        }

        MusicList {
            id: musicList
            visible: false
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.top: parent.top
            height: root.contentHeight
        }

        SkinList {
            id: skinList
            visible: false
            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.top: parent.top
            anchors.topMargin: 10
            height: root.contentHeight - 20
        }
    }

    MenuButton {
        id: listMenu
        source: "qrc:/image/Player/content.png"
        hoverColor: skinManager.menuHoverColor
        imageColor: skinManager.menuColor
        toolTip: qsTr("播放列表")
        width: 0
        height: parent.menuHeight
        anchors.top: closeMenu.bottom
        onClicked: {
            if (content.isVisible) {
                if (musicList.visible) {
                    content.hide();
                } else {
                    musicList.visible = true;
                    skinList.visible = false;
                }
            } else {
                musicList.visible = true;
                content.display();
            }
        }
    }

    MenuButton {
        id: historyMenu
        source: "qrc:/image/Player/history.png"
        hoverColor: skinManager.menuHoverColor
        imageColor: skinManager.menuColor
        toolTip: qsTr("历史播放记录")
        width: 0
        height: parent.menuHeight
        anchors.top: listMenu.bottom
    }

    MenuButton {
        id: skinMenu
        source: "qrc:/image/Player/skin.png"
        hoverColor: skinManager.menuHoverColor
        imageColor: skinManager.menuColor
        toolTip: qsTr("更换皮肤")
        width: 0
        height: parent.menuHeight
        anchors.top: historyMenu.bottom
        onClicked: {
            if (content.isVisible) {
                if (skinList.visible) {
                    content.hide();
                } else {
                    musicList.visible = false;
                    skinList.visible = true;
                }
            } else {
                skinList.visible = true;
                content.display();
            }
        }
    }
}
