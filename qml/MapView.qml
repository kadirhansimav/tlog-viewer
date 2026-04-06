import QtQuick
import QtQuick.Controls
import QtLocation
import QtPositioning

// KH-8: Konum bilgisinin 2D harita üzerinde gösterilmesi
Item {
    id: root
    clip: true

    // OSM map plugin — standart OpenStreetMap (API key gerektirmez)
    Plugin {
        id: osmPlugin
        name: "osm"
        // Online providers repository'yi devre dışı bırak, built-in OSM kullan
        PluginParameter { name: "osm.mapping.providersrepository.disabled"; value: true }
        // Açık OSM tile sunucusu
        PluginParameter { name: "osm.mapping.custom.host";         value: "https://tile.openstreetmap.org/" }
        PluginParameter { name: "osm.mapping.custom.mapcopyright"; value: "© OpenStreetMap contributors" }
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: osmPlugin
        zoomLevel: 15
        copyrightsVisible: true
        // API key gerektirmeyen map type'ı seç (CustomMap = built-in OSM)
        onSupportedMapTypesChanged: {
            for (var i = 0; i < supportedMapTypes.length; i++) {
                // Custom map type varsa onu seç (providers repo disabled olduğunda OSM custom gelir)
                if (supportedMapTypes[i].style === MapType.CustomMap ||
                    supportedMapTypes[i].name.toLowerCase().indexOf("custom") >= 0) {
                    activeMapType = supportedMapTypes[i]
                    return
                }
            }
            // Yoksa ilkini seç
            if (supportedMapTypes.length > 0)
                activeMapType = supportedMapTypes[0]
        }

        // KH-5: Timeline değişince harita güncellenir
        center: QtPositioning.coordinate(
            controller.currentLat !== 0.0 ? controller.currentLat : 47.398,
            controller.currentLon !== 0.0 ? controller.currentLon : 8.547
        )

        // Center map when file is first loaded (fly-to animation)
        Behavior on center {
            enabled: !dragHandler.active
            CoordinateAnimation { duration: 200; easing.type: Easing.OutCubic }
        }

        // ── Flight path polyline ──────────────────────────────
        MapPolyline {
            id: flightPathShadow
            line.color: "#80000000"
            line.width: 6
            path: controller.flightPath
        }
        MapPolyline {
            id: flightPath
            line.color: "#89b4fa"
            line.width: 3
            path: controller.flightPath
        }

        // ── Current position marker ───────────────────────────
        // Dış halka (pulse efekti için)
        MapCircle {
            center: QtPositioning.coordinate(controller.currentLat, controller.currentLon)
            radius: 22
            color: "transparent"
            border.color: "#f38ba8"
            border.width: 2
            opacity: 0.6
        }
        // Ana marker
        MapCircle {
            id: posMarker
            center: QtPositioning.coordinate(controller.currentLat, controller.currentLon)
            radius: 14
            color: "#f38ba8"
            border.color: "#ffffff"
            border.width: 3
        }
        // Merkez nokta
        MapCircle {
            center: QtPositioning.coordinate(controller.currentLat, controller.currentLon)
            radius: 4
            color: "#1e1e2e"
            border.width: 0
        }

        // Harita her zaman düz — rotation/tilt değişirse sıfırla
        onBearingChanged: if (bearing !== 0) bearing = 0
        onTiltChanged:    if (tilt    !== 0) tilt    = 0

        // Pan — sürükleme ile taşıma
        DragHandler {
            id: dragHandler
            grabPermissions: PointerHandler.TakeOverForbidden
        }

        // Zoom — mouse wheel
        WheelHandler {
            id: wheelHandler
            acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
            rotationScale: 1.0 / 120.0
            property: "zoomLevel"
        }

        // Pinch zoom — sadece rotation'ı kapat
        PinchHandler {
            id: pinchHandler
            grabPermissions: PointerHandler.TakeOverForbidden
            rotationAxis.enabled: false
        }
    }

    // Zoom controls overlay
    Column {
        anchors { right: parent.right; bottom: parent.bottom; margins: 12 }
        spacing: 4

        RoundButton {
            width: 36; height: 36
            text: "+"
            font.pixelSize: 18
            onClicked: map.zoomLevel = Math.min(map.maximumZoomLevel, map.zoomLevel + 1)
            background: Rectangle { color: "#313244"; radius: 4 }
            contentItem: Text {
                text: parent.text; color: "#cdd6f4"
                font.pixelSize: 18
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
        RoundButton {
            width: 36; height: 36
            text: "−"
            font.pixelSize: 18
            onClicked: map.zoomLevel = Math.max(map.minimumZoomLevel, map.zoomLevel - 1)
            background: Rectangle { color: "#313244"; radius: 4 }
            contentItem: Text {
                text: parent.text; color: "#cdd6f4"
                font.pixelSize: 18
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    // Coordinate info overlay (bottom-left)
    Rectangle {
        anchors { left: parent.left; bottom: parent.bottom; margins: 8 }
        width: coordText.implicitWidth + 16
        height: 26
        color: "#aa1e1e2e"
        radius: 4

        Text {
            id: coordText
            anchors.centerIn: parent
            text: controller.currentLat !== 0.0
                  ? "Lat: %1  Lon: %2"
                    .arg(controller.currentLat.toFixed(6))
                    .arg(controller.currentLon.toFixed(6))
                  : "GPS verisi bekleniyor..."
            color: "#cdd6f4"
            font.pixelSize: 11
        }
    }
}
