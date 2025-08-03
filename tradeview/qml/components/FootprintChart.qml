import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Shapes 1.15

Rectangle {
    id: footprintChart
    color: "#1e1e1e"
    border.color: "#404040"
    border.width: 1

    // 属性
    property var model: []
    property int cellWidth: 120
    property int cellHeight: 80
    property bool showVolume: true
    property bool showDelta: true
    property bool showPercent: true
    property color buyColor: "#00ff00"
    property color sellColor: "#ff0000"
    property color neutralColor: "#ffffff"

    // 信号
    signal cellClicked(int index, var cellData)
    signal cellHovered(int index, var cellData)

    // 标题
    Text {
        id: chartTitle
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.margins: 10
        text: "足迹图"
        color: "#ffffff"
        font.pixelSize: 16
        font.bold: true
    }

    // 足迹图网格
    GridView {
        id: footprintGridView
        anchors.top: chartTitle.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        cellWidth: footprintChart.cellWidth
        cellHeight: footprintChart.cellHeight
        model: footprintChart.model
        clip: true

        delegate: Rectangle {
            id: cellRect
            width: footprintGridView.cellWidth - 2
            height: footprintGridView.cellHeight - 2
            color: "#252525"
            border.color: "#404040"
            border.width: 1
            radius: 4

            // 背景渐变
            gradient: Gradient {
                GradientStop { position: 0.0; color: cellRect.color }
                GradientStop { position: 1.0; color: Qt.darker(cellRect.color, 1.2) }
            }

            // 价格标签
            Text {
                id: priceText
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 5
                text: model.close ? model.close.toFixed(2) : "0.00"
                color: model.isUp ? footprintChart.buyColor : footprintChart.sellColor
                font.pixelSize: 12
                font.bold: true
                horizontalAlignment: Text.AlignHCenter
            }

            // 成交量信息
            ColumnLayout {
                anchors.top: priceText.bottom
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 5
                spacing: 2

                // 买入量
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 5

                    Rectangle {
                        width: 8
                        height: 8
                        radius: 4
                        color: footprintChart.buyColor
                    }

                    Text {
                        text: footprintChart.showVolume ? "买: " + (model.totalBuyVolume || "0") : ""
                        color: footprintChart.buyColor
                        font.pixelSize: 10
                        Layout.fillWidth: true
                    }
                }

                // 卖出量
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 5

                    Rectangle {
                        width: 8
                        height: 8
                        radius: 4
                        color: footprintChart.sellColor
                    }

                    Text {
                        text: footprintChart.showVolume ? "卖: " + (model.totalSellVolume || "0") : ""
                        color: footprintChart.sellColor
                        font.pixelSize: 10
                        Layout.fillWidth: true
                    }
                }

                // Delta值
                Text {
                    text: footprintChart.showDelta ? "Δ: " + (model.delta || "0") : ""
                    color: model.delta > 0 ? footprintChart.buyColor : 
                           model.delta < 0 ? footprintChart.sellColor : footprintChart.neutralColor
                    font.pixelSize: 10
                    font.bold: true
                    Layout.alignment: Qt.AlignHCenter
                }

                // Delta百分比
                Text {
                    text: footprintChart.showPercent ? (model.deltaPercent || "0").toFixed(1) + "%" : ""
                    color: model.deltaPercent > 0 ? footprintChart.buyColor : 
                           model.deltaPercent < 0 ? footprintChart.sellColor : footprintChart.neutralColor
                    font.pixelSize: 9
                    Layout.alignment: Qt.AlignHCenter
                }
            }

            // 鼠标悬停效果
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    cellRect.color = "#353535"
                    cellRect.scale = 1.05
                    cellHovered(index, model)
                }
                onExited: {
                    cellRect.color = "#252525"
                    cellRect.scale = 1.0
                }
                onClicked: {
                    cellClicked(index, model)
                }
            }

            // 动画效果
            Behavior on scale {
                NumberAnimation { duration: 150 }
            }

            Behavior on color {
                ColorAnimation { duration: 150 }
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

        ScrollBar.horizontal: ScrollBar {
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

    // 图例
    Rectangle {
        id: legend
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 10
        width: 150
        height: 80
        color: "#2d2d2d"
        border.color: "#404040"
        border.width: 1
        radius: 4

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 5

            Text {
                text: "图例"
                color: "#ffffff"
                font.pixelSize: 12
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 5

                Rectangle {
                    width: 12
                    height: 12
                    radius: 6
                    color: footprintChart.buyColor
                }

                Text {
                    text: "买入压力"
                    color: footprintChart.buyColor
                    font.pixelSize: 10
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 5

                Rectangle {
                    width: 12
                    height: 12
                    radius: 6
                    color: footprintChart.sellColor
                }

                Text {
                    text: "卖出压力"
                    color: footprintChart.sellColor
                    font.pixelSize: 10
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 5

                Rectangle {
                    width: 12
                    height: 12
                    radius: 6
                    color: footprintChart.neutralColor
                }

                Text {
                    text: "中性"
                    color: footprintChart.neutralColor
                    font.pixelSize: 10
                }
            }
        }
    }
} 