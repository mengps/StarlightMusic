import QtQuick 2.12
import Qt.labs.platform 1.1

SystemTrayIcon {
    visible: true
    iconSource: "qrc:/image/icon.png"
    tooltip: qsTr("StarlightMusic后台运行中...")
    menu: Menu {
        MenuItem {
            text: "退出"
            onTriggered: Qt.quit();
        }
    }
    onActivated:{
        switch (reason)
        {
        case SystemTrayIcon.DoubleClick:
            mainPanel.show();
            break;
        }
    }
}
