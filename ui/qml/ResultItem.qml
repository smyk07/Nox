import QtQuick
import QtQuick.Controls

Item {
    id: root

    property bool isCurrent: false
    property string itemTitle: ""
    property string itemSubtitle: ""
    property string itemIconName: ""
    property var themePalette
    property string appFont: "Sans Serif"
    property bool isHovered: false

    signal hovered()
    signal clicked()

    width: 360
    height: 68

    Rectangle {
        anchors.fill: parent
        radius: 18
        color: root.isCurrent ? themePalette.selectedGlass : (root.isHovered ? "#32302ff6" : "#2c2a29f2")
        opacity: 1
        border.color: root.isCurrent ? themePalette.orange : (root.isHovered
                       ? Qt.rgba(themePalette.lineBright.r, themePalette.lineBright.g, themePalette.lineBright.b, 0.55)
                       : Qt.rgba(themePalette.line.r, themePalette.line.g, themePalette.line.b, 0.35))
        border.width: 1

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            height: 1
            color: "#ffffff"
            opacity: root.isCurrent ? 0.08 : (root.isHovered ? 0.06 : 0.04)
        }

        Behavior on color {
            ColorAnimation {
                duration: 110
            }
        }

        Behavior on border.color {
            ColorAnimation {
                duration: 110
            }
        }
    }

    Rectangle {
        width: 3
        height: parent.height - 22
        radius: 2
        x: 10
        y: 11
        color: root.isCurrent ? themePalette.orange : themePalette.aqua
        opacity: root.isCurrent ? 0.95 : 0
    }

    Row {
        anchors.fill: parent
        anchors.leftMargin: 20
        anchors.rightMargin: 14
        anchors.topMargin: 12
        anchors.bottomMargin: 12
        spacing: 14

        Rectangle {
            width: 42
            height: 42
            radius: 15
            color: root.isCurrent ? "#4b3225" : "#3a3328"
            border.color: root.isCurrent ? Qt.rgba(themePalette.orange.r, themePalette.orange.g, themePalette.orange.b, 0.32) : "#00000000"
            border.width: 1

            Image {
                anchors.centerIn: parent
                width: 22
                height: 22
                source: "image://icons/" + root.itemIconName
                fillMode: Image.PreserveAspectFit
                smooth: true
            }
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 2
            width: parent.width - 70

            Text {
                text: root.itemTitle
                color: themePalette.textColor
                font.family: appFont
                font.pixelSize: 17
                font.weight: Font.DemiBold
                elide: Text.ElideRight
                width: parent.width
            }

            Text {
                text: root.itemSubtitle
                color: themePalette.subtext
                font.family: appFont
                font.pixelSize: 12
                elide: Text.ElideRight
                width: parent.width
            }
        }
    }

    HoverHandler {
        onHoveredChanged: {
            root.isHovered = hovered
            if (hovered) {
                root.hovered()
            }
        }
    }

    TapHandler {
        onTapped: root.clicked()
    }
}
