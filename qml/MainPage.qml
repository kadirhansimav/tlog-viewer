import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// KH-3: Sekmeli yapı — Harita + Ham Veri
// KH-4: Alt kısımda ortak TimelineControl
Item {
    id: root

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // ── Tab bar ──────────────────────────────────────────────
        TabBar {
            id: tabBar
            Layout.fillWidth: true
            Layout.minimumHeight: 40
            height: 40

            background: Rectangle { color: "#181825" }

            TabButton {
                text: "Harita"
                width: 120
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 13
                    color: tabBar.currentIndex === 0 ? "#cdd6f4" : "#6c7086"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    Behavior on color { ColorAnimation { duration: 150 } }
                }
                background: Rectangle {
                    color: tabBar.currentIndex === 0 ? "#1e1e2e" : "#181825"
                    Rectangle {
                        visible: tabBar.currentIndex === 0
                        anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
                        height: 2
                        color: "#89b4fa"
                    }
                }
            }

            TabButton {
                text: "Ham Veri"
                width: 120
                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 13
                    color: tabBar.currentIndex === 1 ? "#cdd6f4" : "#6c7086"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    Behavior on color { ColorAnimation { duration: 150 } }
                }
                background: Rectangle {
                    color: tabBar.currentIndex === 1 ? "#1e1e2e" : "#181825"
                    Rectangle {
                        visible: tabBar.currentIndex === 1
                        anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
                        height: 2
                        color: "#89b4fa"
                    }
                }
            }
        }

        // ── Content area ─────────────────────────────────────────
        StackLayout {
            id: contentStack
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex  // KH-9: tab geçişi timeline'ı sıfırlamaz

            MapView {}
            RawDataView {}
        }

        // ── Timeline (shared, persistent) ────────────────────────
        TimelineControl {
            Layout.fillWidth: true
        }
    }
}
