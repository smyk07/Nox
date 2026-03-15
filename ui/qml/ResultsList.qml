import QtQuick
import QtQuick.Controls

ListView {
    id: list

    property var themePalette
    property string appFont: "Sans Serif"

    clip: true
    spacing: 10
    model: controller.model
    currentIndex: controller.currentIndex
    boundsBehavior: Flickable.StopAtBounds
    ScrollBar.vertical: ScrollBar {
        policy: ScrollBar.AsNeeded
        contentItem: Rectangle {
            implicitWidth: 5
            radius: 3
            color: themePalette.lineBright
            opacity: 0.55
        }
        background: Item {}
    }

    onCurrentIndexChanged: positionViewAtIndex(currentIndex, ListView.Contain)

    Text {
        anchors.centerIn: parent
        visible: list.count === 0
        text: "No results yet"
        color: themePalette.muted
        font.family: appFont
        font.pixelSize: 13
    }

    delegate: ResultItem {
        required property int index
        required property string title
        required property string subtitle
        required property string iconName

        width: ListView.view.width
        isCurrent: controller.currentIndex === index
        itemTitle: title
        itemSubtitle: subtitle
        itemIconName: iconName
        themePalette: list.themePalette
        appFont: list.appFont

        onHovered: controller.setCurrentIndex(index)
        onClicked: {
            controller.setCurrentIndex(index)
            controller.activateSelection()
        }
    }
}
