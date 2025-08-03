import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: dataTable
    color: "#1e1e1e"
    border.color: "#404040"
    border.width: 1

    // 属性
    property var model: []
    property var headers: []
    property int rowHeight: 30
    property int headerHeight: 40
    property var columnWidths: []
    property bool showAlternateRows: true
    property color alternateRowColor: "#252525"
    property color headerColor: "#2d2d2d"
    property color headerTextColor: "#ffffff"
    property color rowTextColor: "#ffffff"

    // 信号
    signal rowClicked(int index, var rowData)
    signal headerClicked(int column)

    // 标题栏
    Rectangle {
        id: headerRow
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: dataTable.headerHeight
        color: dataTable.headerColor
        border.color: "#404040"
        border.width: 1

        RowLayout {
            anchors.fill: parent
            anchors.margins: 5
            spacing: 0

            Repeater {
                model: dataTable.headers

                Rectangle {
                    Layout.preferredWidth: dataTable.columnWidths[index] || 100
                    Layout.fillHeight: true
                    color: "transparent"
                    border.color: "#404040"
                    border.width: 1

                    Text {
                        anchors.centerIn: parent
                        text: modelData
                        color: dataTable.headerTextColor
                        font.pixelSize: 12
                        font.bold: true
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: headerClicked(index)
                    }
                }
            }
        }
    }

    // 数据行
    ListView {
        id: dataListView
        anchors.top: headerRow.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        model: dataTable.model
        clip: true

        delegate: Rectangle {
            width: dataListView.width
            height: dataTable.rowHeight
            color: showAlternateRows && index % 2 === 1 ? dataTable.alternateRowColor : "transparent"
            border.color: "#404040"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 5
                spacing: 0

                Repeater {
                    model: dataTable.headers

                    Rectangle {
                        Layout.preferredWidth: dataTable.columnWidths[index] || 100
                        Layout.fillHeight: true
                        color: "transparent"
                        border.color: "#404040"
                        border.width: 1

                        Text {
                            anchors.centerIn: parent
                            text: {
                                if (parent.parent.parent.parent.parent.modelData && 
                                    parent.parent.parent.parent.parent.modelData[index] !== undefined) {
                                    return parent.parent.parent.parent.parent.modelData[index]
                                }
                                return ""
                            }
                            color: dataTable.rowTextColor
                            font.pixelSize: 11
                        }
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: rowClicked(index, dataTable.model[index])
            }
        }

        // 滚动条
        ScrollBar.vertical: ScrollBar {
            active: true
            background: Rectangle {
                color: "#2d2d2d"
                border.color: "#404040"
                border.width: 1
            }
            contentItem: Rectangle {
                color: "#404040"
                radius: 2
            }
        }
    }
} 