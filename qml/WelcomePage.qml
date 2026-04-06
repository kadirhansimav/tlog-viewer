import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

// KH-1: Uygulama açılışında yalnızca dosya yükleme ekranı
Item {
    id: root
    signal fileAccepted

    Rectangle {
        anchors.fill: parent
        color: "#1e1e2e"

        Column {
            anchors.centerIn: parent
            spacing: 32

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "TLOG PARSER"
                font.pixelSize: 42
                font.letterSpacing: 8
                font.weight: Font.Light
                color: "#cdd6f4"
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "TLOG Dosyası Yükle"
                width: 220
                height: 44

                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 13
                    font.letterSpacing: 1
                    color: "#1e1e2e"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle {
                    color: parent.hovered ? "#b4befe" : "#cdd6f4"
                    radius: 4
                    Behavior on color { ColorAnimation { duration: 120 } }
                }

                onClicked: fileDialog.open()
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: "TLOG Dosyası Seç"
        nameFilters: ["TLOG Dosyaları (*.tlog)", "Tüm Dosyalar (*)"]
        onAccepted: {
            if (controller.loadFile(fileDialog.selectedFile.toString())) {
                root.fileAccepted()
            }
        }
    }
}
