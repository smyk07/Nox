import QtQuick
import QtQuick.Controls

TextField {
    id: field

    property var themePalette
    property string appFont: "Sans Serif"

    placeholderText: "Search apps, files, commands..."
    placeholderTextColor: Qt.rgba(themePalette.subtext.r, themePalette.subtext.g, themePalette.subtext.b, 0.45)
    color: themePalette.textColor
    font.family: appFont
    font.pixelSize: 24
    font.weight: Font.Medium
    selectedTextColor: themePalette.base
    selectionColor: themePalette.yellow
    leftPadding: 58
    rightPadding: 20
    topPadding: 17
    bottomPadding: 17
    focus: true
    renderType: Text.NativeRendering

    background: Rectangle {
        radius: 20
        color: field.activeFocus ? "#32302ff6" : "#32302ff0"
        border.color: field.activeFocus ? themePalette.orange : themePalette.line
        border.width: 1

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: 1
            color: "#ffffff"
            opacity: field.activeFocus ? 0.06 : 0.03
        }

        Rectangle {
            width: 34
            height: 34
            x: 14
            y: (parent.height - height) / 2
            radius: 12
            color: field.activeFocus ? "#493027" : "#3a3029"
            border.color: field.activeFocus ? Qt.rgba(themePalette.orange.r, themePalette.orange.g, themePalette.orange.b, 0.3) : "#00000000"
            border.width: 1
        }

        Text {
            anchors.left: parent.left
            anchors.leftMargin: 25
            anchors.verticalCenter: parent.verticalCenter
            text: ">"
            color: field.activeFocus ? themePalette.orange : themePalette.muted
            font.family: appFont
            font.pixelSize: 16
            font.weight: Font.DemiBold
        }
    }

    onTextChanged: controller.setQuery(text)

    Keys.onPressed: function(event) {
        if (event.key === Qt.Key_Down) {
            controller.moveSelection(1)
            event.accepted = true
        } else if (event.key === Qt.Key_Up) {
            controller.moveSelection(-1)
            event.accepted = true
        } else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
            controller.activateSelection()
            event.accepted = true
        } else if (event.key === Qt.Key_Escape) {
            controller.closeLauncher()
            event.accepted = true
        } else if (event.key === Qt.Key_Tab) {
            controller.toggleActionMenu()
            event.accepted = true
        }
    }

    Connections {
        target: controller

        function onVisibleChanged() {
            if (controller.visible) {
                field.text = ""
                field.forceActiveFocus()
            }
        }
    }
}
