import QtQuick
import QtQuick.Controls

Rectangle {
    property var themePalette
    property string appFont: "Sans Serif"

    radius: 18
    color: "#2b2928f0"
    border.color: Qt.rgba(themePalette.lineBright.r, themePalette.lineBright.g, themePalette.lineBright.b, 0.75)
    border.width: 1

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 1
        color: "#ffffff"
        opacity: 0.08
    }

    Flickable {
        id: flick
        anchors.fill: parent
        anchors.margins: 16
        clip: true
        contentWidth: width
        contentHeight: previewColumn.height
        boundsBehavior: Flickable.StopAtBounds

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
        }

        Column {
            id: previewColumn
            width: flick.width
            spacing: 14

            Text {
                text: "Preview"
                color: themePalette.muted
                font.family: appFont
                font.pixelSize: 11
                font.weight: Font.Medium
                font.letterSpacing: 1.2
                width: parent.width
            }

            Text {
                text: controller.previewTitle.length > 0 ? controller.previewTitle : "No Selection"
                color: themePalette.textColor
                font.family: appFont
                font.pixelSize: 18
                font.weight: Font.DemiBold
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                width: parent.width
            }

            Rectangle {
                width: 48
                height: 3
                radius: 2
                color: themePalette.orange
            }

            Text {
                text: controller.previewText.length > 0 ? controller.previewText : "Select a result to see more detail."
                color: themePalette.subtext
                font.family: appFont
                font.pixelSize: 13
                lineHeight: 1.3
                wrapMode: Text.WrapAnywhere
                width: parent.width
            }
        }
    }
}
