import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Window {
    Material.accent: Material.Blue

    width: 960
    height: 540
    visible: true
    modality: Qt.ApplicationModal
    flags: Qt.Window
    title: qsTr("Melosty")
}
