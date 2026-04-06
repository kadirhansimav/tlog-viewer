import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

// KH-4 & KH-5: Ortak, paylaşımlı timeline — tüm sekmelerle senkron
Rectangle {
    id: root
    height: 52
    color: "#181825"

    // Top border
    Rectangle {
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: 1
        color: "#313244"
    }

    // Auto-play timer (KH-5: timeline hareket ettikçe tüm ekranlar güncellenir)
    Timer {
        id: playTimer
        interval: 50        // ~20 fps playback step
        repeat: true
        running: false
        onTriggered: {
            if (!controller.fileLoaded) { running = false; return }
            const next = controller.currentIndex + playSpeed.value
            if (next >= controller.messageCount - 1) {
                controller.currentIndex = controller.messageCount - 1
                running = false
            } else {
                controller.currentIndex = next
            }
        }
    }

    RowLayout {
        anchors { fill: parent; leftMargin: 10; rightMargin: 12 }
        spacing: 8

        // ── Play / Pause button ──────────────────────────────
        Rectangle {
            width: 32; height: 32
            radius: 16
            color: playTimer.running ? "#f38ba8" : "#89b4fa"
            opacity: controller.fileLoaded ? 1.0 : 0.4
            Behavior on color { ColorAnimation { duration: 150 } }

            Text {
                anchors.centerIn: parent
                text: playTimer.running ? "⏸" : "▶"
                font.pixelSize: 14
                color: "#1e1e2e"
            }

            MouseArea {
                anchors.fill: parent
                enabled: controller.fileLoaded
                cursorShape: Qt.PointingHandCursor
                onClicked: playTimer.running = !playTimer.running
            }
        }

        // ── Speed selector ───────────────────────────────────
        ComboBox {
            id: playSpeed
            implicitWidth: 72
            implicitHeight: 28
            model: ["1×", "5×", "10×", "50×", "100×"]
            // value: step per timer tick
            property int value: [1, 5, 10, 50, 100][currentIndex]

            background: Rectangle { color: "#313244"; radius: 4 }
            contentItem: Text {
                leftPadding: 8
                text: playSpeed.displayText
                color: "#cdd6f4"
                font.pixelSize: 12
                verticalAlignment: Text.AlignVCenter
            }
            popup.background: Rectangle { color: "#313244"; radius: 4 }

            delegate: ItemDelegate {
                width: playSpeed.width
                contentItem: Text {
                    text: modelData
                    color: "#cdd6f4"
                    font.pixelSize: 12
                    verticalAlignment: Text.AlignVCenter
                }
                background: Rectangle {
                    color: hovered ? "#45475a" : "transparent"
                }
            }
        }

        // ── Start time label ─────────────────────────────────
        Text {
            text: controller.fileLoaded ? controller.startTimeStr : "00:00"
            color: "#6c7086"
            font.pixelSize: 12
            font.family: "monospace"
            Layout.preferredWidth: 42
        }

        // ── Slider ───────────────────────────────────────────
        Slider {
            id: slider
            Layout.fillWidth: true
            from: 0
            to: controller.fileLoaded ? Math.max(1, controller.messageCount - 1) : 1
            stepSize: 1
            value: controller.currentIndex
            enabled: controller.fileLoaded

            onMoved: {
                playTimer.running = false   // pause when user drags
                controller.currentIndex = Math.round(value)
            }

            Connections {
                target: controller
                function onCurrentIndexChanged() {
                    if (!slider.pressed)
                        slider.value = controller.currentIndex
                }
            }

            background: Rectangle {
                x: slider.leftPadding
                y: slider.topPadding + slider.availableHeight / 2 - height / 2
                width: slider.availableWidth
                height: 4
                radius: 2
                color: "#313244"

                Rectangle {
                    width: slider.visualPosition * parent.width
                    height: parent.height
                    radius: 2
                    color: "#89b4fa"
                }
            }

            handle: Rectangle {
                x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
                y: slider.topPadding + slider.availableHeight / 2 - height / 2
                width: 14; height: 14; radius: 7
                color: slider.pressed ? "#b4befe" : "#89b4fa"
                border.color: "#1e1e2e"; border.width: 2
                Behavior on color { ColorAnimation { duration: 100 } }
            }
        }

        // ── Current / end time ───────────────────────────────
        Text {
            text: controller.fileLoaded
                  ? controller.currentTimeStr + " / " + controller.endTimeStr
                  : "00:00 / 00:00"
            color: "#6c7086"
            font.pixelSize: 12
            font.family: "monospace"
            Layout.preferredWidth: 110
            horizontalAlignment: Text.AlignRight
        }
    }
}
