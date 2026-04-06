import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: root
    visible: true
    width: 1200
    height: 750
    minimumWidth: 800
    minimumHeight: 550
    title: "TLOG Parser"

    color: "#1e1e2e"

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: welcomeComponent
    }

    Component { id: welcomeComponent; WelcomePage { onFileAccepted: stack.push(mainComponent) } }
    Component { id: mainComponent;   MainPage {} }

    // Global error toast
    Connections {
        target: controller
        function onLoadError(msg) {
            errorText.text = msg
            errorBar.visible = true
            hideTimer.restart()
        }
    }

    Rectangle {
        id: errorBar
        visible: false
        anchors { bottom: parent.bottom; horizontalCenter: parent.horizontalCenter; bottomMargin: 16 }
        width: Math.min(parent.width - 32, 600)
        height: 44
        radius: 8
        color: "#e74c3c"
        Text {
            id: errorText
            anchors.centerIn: parent
            color: "white"
            font.pixelSize: 13
            elide: Text.ElideRight
            width: parent.width - 24
            horizontalAlignment: Text.AlignHCenter
        }
        Timer { id: hideTimer; interval: 4000; onTriggered: errorBar.visible = false }
        MouseArea { anchors.fill: parent; onClicked: errorBar.visible = false }
    }
}
