import QtQuick
import QtQuick.Controls

// KH-6: Ham verilerin okunabilir şekilde gösterilmesi
// KH-7: Aktif kaydın vurgulanması
Item {
    id: root

    // KH-7: Timeline değişince aktif kayıt highlight edilir ve listeye scroll yapılır
    Connections {
        target: controller
        function onCurrentIndexChanged() {
            listView.positionViewAtIndex(controller.currentIndex,
                                         ListView.Center)
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "#1e1e2e"
    }

    ListView {
        id: listView
        anchors.fill: parent
        model: controller.messageModel
        clip: true
        cacheBuffer: 400

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
            contentItem: Rectangle {
                implicitWidth: 6
                color: "#585b70"
                radius: 3
            }
        }

        delegate: Rectangle {
            id: delegateRect
            required property int    index
            required property string msgTime
            required property int    msgId
            required property string msgName
            required property string fieldsText
            required property bool   isActive

            width: listView.width
            height: 28

            // KH-7: Active row highlight
            color: isActive ? "#313244" : (index % 2 === 0 ? "#1e1e2e" : "#181825")

            Behavior on color { ColorAnimation { duration: 80 } }

            // Left accent bar for active row
            Rectangle {
                visible: delegateRect.isActive
                width: 3
                height: parent.height
                color: "#89b4fa"
            }

            Row {
                anchors { left: parent.left; leftMargin: 8; verticalCenter: parent.verticalCenter }
                spacing: 0

                // Timestamp
                Text {
                    text: delegateRect.msgTime
                    width: 50
                    font.pixelSize: 12
                    font.family: "monospace"
                    color: delegateRect.isActive ? "#89b4fa" : "#6c7086"
                    verticalAlignment: Text.AlignVCenter
                }

                // Separator
                Rectangle { width: 1; height: 16; color: "#313244"; anchors.verticalCenter: parent.verticalCenter }

                Item { width: 8; height: 1 }

                // Message name
                Text {
                    text: delegateRect.msgName
                    width: 200
                    font.pixelSize: 12
                    font.family: "monospace"
                    font.weight: delegateRect.isActive ? Font.Medium : Font.Normal
                    color: delegateRect.isActive ? "#a6e3a1" : "#94e2d5"
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }

                Rectangle { width: 1; height: 16; color: "#313244"; anchors.verticalCenter: parent.verticalCenter }

                Item { width: 8; height: 1 }

                // Fields text
                Text {
                    text: delegateRect.fieldsText
                    width: listView.width - 280
                    font.pixelSize: 12
                    font.family: "monospace"
                    color: delegateRect.isActive ? "#cdd6f4" : "#a6adc8"
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                }
            }

            // Hover effect
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: if (!delegateRect.isActive) delegateRect.color = "#252535"
                onExited:  delegateRect.color = Qt.binding(() =>
                    delegateRect.isActive ? "#313244" :
                    (delegateRect.index % 2 === 0 ? "#1e1e2e" : "#181825"))
                onClicked: controller.currentIndex = delegateRect.index
            }
        }

        // Column header
        header: Rectangle {
            width: listView.width
            height: 28
            color: "#181825"
            z: 2

            Row {
                anchors { left: parent.left; leftMargin: 8; verticalCenter: parent.verticalCenter }
                spacing: 0

                Text { text: "Zaman"; width: 50;  font.pixelSize: 11; color: "#585b70"; font.weight: Font.Medium }
                Item  { width: 9; height: 1 }
                Text { text: "Mesaj Tipi"; width: 200; font.pixelSize: 11; color: "#585b70"; font.weight: Font.Medium }
                Item  { width: 9; height: 1 }
                Text { text: "Veriler";     font.pixelSize: 11; color: "#585b70"; font.weight: Font.Medium }
            }

            Rectangle {
                anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
                height: 1
                color: "#313244"
            }
        }
    }
}
