import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Controls.Material

Window {
    id: window
    Material.accent: Material.Blue

    width: 960
    height: 540
    visible: true
    modality: Qt.ApplicationModal
    flags: Qt.Window | Qt.FramelessWindowHint
    title: qsTr("Melosty")

    ToolBar {
        id: titleBar
        x: 0
        y: 0
        width: 960
        height: 52

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton

            onPressedChanged: {
                if (pressed && window && typeof window.startSystemMove === "function") {
                    window.startSystemMove()
                }
            }
        }

        TabButton {
            id: homeButton
            x: 287
            y: 0
            width: 91
            height: 52
            text: qsTr("主页")
            checked: true
            down: false
        }

        TabButton {
            id: downloadsButton
            x: 384
            y: 0
            width: 91
            height: 52
            text: qsTr("下载")
        }

        TabButton {
            id: settingsButton
            x: 481
            y: 0
            width: 91
            height: 52
            text: qsTr("设置")
        }

        TabButton {
            id: pluginsButton
            x: 578
            y: 0
            width: 91
            height: 52
            text: qsTr("插件")
            down: false
        }

        RoundButton {
            id: minizedsize
            x: 875
            y: 9
            width: 35
            height: 35
            text: "\u2212"
            font.pointSize: 15
            font.bold: true

            Connections {
                target: minizedsize
                onClicked: {
                    console.log("Action: Window minimized.")
                    window.showMinimized()
                }
            }
        }

        RoundButton {
            id: exit
            x: 916
            y: 9
            width: 35
            height: 35
            text: "\u00d7"
            font.pointSize: 15
            font.bold: true

            Connections {
                target: exit
                onClicked: {
                    console.log("Action: Goodbye Melosty!")
                    Qt.quit()
                }
            }
        }

        Text {
            id: titleText
            x: 10
            y: 10
            color: "#ffffff"
            text: qsTr("Melosty")
            font.pixelSize: 24
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.bold: true
        }
    }
}
