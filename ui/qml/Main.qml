import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

Window {
    id: root

    width: 680
    height: 420
    visible: controller.visible
    color: "transparent"
    flags: Qt.Tool | Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    modality: Qt.NonModal
    title: "NOX"
    readonly property color base: "#1d2021"
    readonly property color surface: "#282828"
    readonly property color surfaceSoft: "#32302f"
    readonly property color surfaceStrong: "#3c3836"
    readonly property color line: "#504945"
    readonly property color lineBright: "#665c54"
    readonly property color textColor: "#fbf1c7"
    readonly property color subtext: "#d5c4a1"
    readonly property color muted: "#bdae93"
    readonly property color orange: "#fe8019"
    readonly property color yellow: "#fabd2f"
    readonly property color green: "#b8bb26"
    readonly property color aqua: "#689d6a"
    readonly property color blue: "#83a598"
    readonly property color panelGlass: "#1d2021f7"
    readonly property color sectionGlass: "#282828f2"
    readonly property color chipGlass: "#32302fed"
    readonly property color selectedGlass: "#3c3836"
    readonly property color accentGlow: "#fe801918"
    readonly property color yellowGlow: "#fabd2f12"
    readonly property color shadowColor: "#100d0e"

    FontLoader {
        id: outfitFont
        source: "qrc:/Nox/resources/fonts/Outfit-VariableFont_wght.ttf"
    }

    property string appFont: outfitFont.status === FontLoader.Ready ? outfitFont.name : "Sans Serif"

    x: Math.round((Screen.width - width) / 2)
    y: Math.round((Screen.height - height) / 2)

    onVisibleChanged: {
        if (visible) {
            openAnimation.restart()
        }
    }

    Rectangle {
        anchors.fill: parent
        radius: 32
        color: "#0b0b0d"
        opacity: 0.92
    }

    Rectangle {
        anchors.fill: parent
        radius: 32
        color: root.shadowColor
        opacity: 0.22
        y: 22
        scale: 0.968
    }

    Rectangle {
        width: 180
        height: 180
        radius: 90
        x: 12
        y: 4
        color: root.orange
        opacity: 0.01
    }

    Rectangle {
        width: 160
        height: 160
        radius: 80
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 28
        anchors.topMargin: 18
        color: root.yellow
        opacity: 0.008
    }

    Rectangle {
        width: 150
        height: 150
        radius: 75
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 82
        anchors.bottomMargin: 16
        color: root.green
        opacity: 0.006
    }

    Rectangle {
        id: panel
        anchors.fill: parent
        radius: 26
        color: "#17181be8"
        border.color: root.lineBright
        border.width: 1
        opacity: 0
        y: -18

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: parent.height * 0.42
            radius: parent.radius
            color: "#ffffff"
            opacity: 0.01
        }

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 14

            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                SearchBar {
                    id: searchBar
                    Layout.fillWidth: true
                    themePalette: root
                    appFont: root.appFont
                }

                Rectangle {
                    Layout.preferredWidth: 44
                    Layout.preferredHeight: 44
                    radius: 14
                    color: closeHover.hovered ? "#3a2224" : "#27191b"
                    border.color: closeHover.hovered ? "#fb4934" : Qt.rgba(root.line.r, root.line.g, root.line.b, 0.48)
                    border.width: 1

                    Text {
                        anchors.centerIn: parent
                        text: "\u00d7"
                        color: closeHover.hovered ? "#fb4934" : root.subtext
                        font.family: root.appFont
                        font.pixelSize: 22
                        font.weight: Font.DemiBold
                    }

                    HoverHandler {
                        id: closeHover
                    }

                    TapHandler {
                        onTapped: controller.quitApplication()
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 20
                color: "#232427ee"
                border.color: root.lineBright
                border.width: 1

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    height: 1
                    color: "#ffffff"
                    opacity: 0.06
                }

                Rectangle {
                    anchors.fill: parent
                    anchors.margins: 1
                    radius: 19
                    color: "#00000000"
                    border.color: Qt.rgba(root.base.r, root.base.g, root.base.b, 0.28)
                    border.width: 1
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    spacing: 14

                    ResultsList {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredWidth: 430
                        themePalette: root
                        appFont: root.appFont
                    }

                    PreviewPanel {
                        Layout.preferredWidth: 198
                        Layout.fillHeight: true
                        themePalette: root
                        appFont: root.appFont
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Text {
                    Layout.fillWidth: true
                    color: root.muted
                    font.family: root.appFont
                    font.pixelSize: 11
                    text: controller.statusText
                    elide: Text.ElideRight
                }

                Text {
                    color: root.subtext
                    font.family: root.appFont
                    font.pixelSize: 11
                    text: "↑↓ Navigate  Enter Run  Tab Actions  Esc Hide"
                }
            }
        }

        Rectangle {
            id: actionMenu
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 18
            visible: controller.actionMenuVisible
            width: 220
            implicitHeight: actionMenuContent.implicitHeight + 24
            height: implicitHeight
            radius: 18
            color: "#282828f2"
            border.color: root.lineBright
            border.width: 1
            opacity: visible ? 1 : 0

            Behavior on opacity {
                NumberAnimation {
                    duration: 120
                    easing.type: Easing.OutCubic
                }
            }

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: 1
                color: "#ffffff"
                opacity: 0.08
            }

            Column {
                id: actionMenuContent
                anchors.fill: parent
                anchors.margins: 12
                spacing: 8

                Text {
                    text: "Actions"
                    color: root.textColor
                    font.family: root.appFont
                    font.pixelSize: 13
                    font.weight: Font.DemiBold
                }

                Repeater {
                    model: controller.currentActions
                    delegate: Text {
                        text: modelData
                        color: root.subtext
                        font.family: root.appFont
                        font.pixelSize: 12
                        wrapMode: Text.Wrap
                    }
                }
            }
        }
    }

    ParallelAnimation {
        id: openAnimation
        NumberAnimation {
            target: panel
            property: "opacity"
            from: 0
            to: 1
            duration: 160
            easing.type: Easing.OutCubic
        }
        NumberAnimation {
            target: panel
            property: "y"
            from: -18
            to: 0
            duration: 200
            easing.type: Easing.OutCubic
        }
    }
}
