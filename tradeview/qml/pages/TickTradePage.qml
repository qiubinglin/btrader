import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: tickTradePage
    background: Rectangle {
        color: "#1e1e1e"
    }

    // Toolbar
    Rectangle {
        id: toolbar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 50
        color: "#2d2d2d"
        border.color: "#404040"
        border.width: 1

        RowLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10

            // Symbol selection
            ComboBox {
                id: symbolComboBox
                model: ["BTC/USDT", "ETH/USDT", "BNB/USDT", "ADA/USDT"]
                currentIndex: 0
                background: Rectangle {
                    color: "#353535"
                    border.color: "#404040"
                    border.width: 1
                    radius: 4
                }
                contentItem: Text {
                    text: parent.currentText
                    color: "#ffffff"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onCurrentTextChanged: {
                    tickTradeModel.clear()
                    loadTickTradeData()
                }
            }

            // Max records setting
            SpinBox {
                id: maxRecordsSpinBox
                from: 100
                to: 10000
                value: 1000
                stepSize: 100
                background: Rectangle {
                    color: "#353535"
                    border.color: "#404040"
                    border.width: 1
                    radius: 4
                }
                textFromValue: function(value) {
                    return "Max Records: " + value
                }
                valueFromText: function(text) {
                    return parseInt(text.replace("Max Records: ", ""))
                }
                onValueChanged: {
                    tickTradeModel.setMaxCount(value)
                }
            }

            // Clear button
            Button {
                text: "Clear"
                background: Rectangle {
                    color: parent.pressed ? "#404040" : 
                           parent.hovered ? "#353535" : "#2d2d2d"
                    border.color: "#404040"
                    border.width: 1
                    radius: 4
                }
                contentItem: Text {
                    text: parent.text
                    color: "#ffffff"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: tickTradeModel.clear()
            }

            // Auto refresh checkbox
            CheckBox {
                id: autoRefreshCheckBox
                text: "Auto Refresh"
                checked: true
                indicator: Rectangle {
                    width: 16
                    height: 16
                    border.color: "#404040"
                    border.width: 1
                    radius: 2
                    Rectangle {
                        width: 12
                        height: 12
                        radius: 1
                        color: parent.parent.checked ? "#00ff00" : "transparent"
                        anchors.centerIn: parent
                    }
                }
                contentItem: Text {
                    text: parent.text
                    color: "#ffffff"
                    font.pixelSize: 12
                }
            }

            Item { Layout.fillWidth: true }

            // Statistics
            Text {
                text: "Latest Price: " + (tickTradeModel.latestPrice || "0.00")
                color: "#ffff00"
                font.pixelSize: 12
            }

            Text {
                text: "Total Volume: " + (tickTradeModel.totalVolume || "0")
                color: "#ffffff"
                font.pixelSize: 12
            }

            Text {
                text: "Records: " + (tickTradeModel.count || "0")
                color: "#cccccc"
                font.pixelSize: 12
            }
        }
    }

    // Tick trade display area
    Rectangle {
        id: tickTradeArea
        anchors.top: toolbar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        color: "#1e1e1e"
        border.color: "#404040"
        border.width: 1

        // Title
        Text {
            id: tickTradeTitle
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 10
            text: symbolComboBox.currentText + " Tick Trades"
            color: "#ffffff"
            font.pixelSize: 16
            font.bold: true
        }

        // Column headers
        Rectangle {
            id: columnHeader
            anchors.top: tickTradeTitle.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10
            height: 30
            color: "#2d2d2d"
            border.color: "#404040"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                Text {
                    text: "Time"
                    color: "#ffffff"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.preferredWidth: 100
                }

                Text {
                    text: "Price"
                    color: "#ffffff"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.preferredWidth: 100
                }

                Text {
                    text: "Volume"
                    color: "#ffffff"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.preferredWidth: 80
                }

                Text {
                    text: "Direction"
                    color: "#ffffff"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.preferredWidth: 80
                }

                Text {
                    text: "Amount"
                    color: "#ffffff"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.fillWidth: true
                }
            }
        }

        // Tick trade list
        ListView {
            id: tickTradeListView
            anchors.top: columnHeader.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10
            model: tickTradeModel
            clip: true
            spacing: 1

            delegate: Rectangle {
                width: tickTradeListView.width
                height: 30
                color: index % 2 === 0 ? "#1e1e1e" : "#252525"
                border.color: "#404040"
                border.width: 0.5

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                    // Time
                    Text {
                        text: model.timestamp ? new Date(model.timestamp).toLocaleTimeString() : ""
                        color: "#888888"
                        font.pixelSize: 11
                        Layout.preferredWidth: 100
                    }

                    // Price
                    Text {
                        text: model.price ? model.price.toFixed(2) : ""
                        color: model.direction === "buy" ? "#00ff00" : "#ff0000"
                        font.pixelSize: 11
                        font.bold: true
                        Layout.preferredWidth: 100
                    }

                    // Volume
                    Text {
                        text: model.volume ? model.volume.toLocaleString() : ""
                        color: "#ffffff"
                        font.pixelSize: 11
                        Layout.preferredWidth: 80
                    }

                    // Direction
                    Text {
                        text: model.direction ? model.direction.toUpperCase() : ""
                        color: model.direction === "buy" ? "#00ff00" : "#ff0000"
                        font.pixelSize: 11
                        font.bold: true
                        Layout.preferredWidth: 80
                    }

                    // Amount
                    Text {
                        text: model.amount ? model.amount.toLocaleString() : ""
                        color: "#cccccc"
                        font.pixelSize: 11
                        Layout.fillWidth: true
                    }
                }

                // Mouse hover effect
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        parent.color = "#353535"
                    }
                    onExited: {
                        parent.color = index % 2 === 0 ? "#1e1e1e" : "#252525"
                    }
                }
            }

            // Scrollbar
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

    // Auto refresh timer
    Timer {
        id: autoRefreshTimer
        interval: 1000  // Refresh every second
        running: autoRefreshCheckBox.checked
        repeat: true
        onTriggered: {
            if (autoRefreshCheckBox.checked) {
                // Data is automatically updated by DataManager, no manual update needed
                console.log("Tick trade data refreshed by DataManager")
            }
        }
    }

    // Initialize data when page loads
    Component.onCompleted: {
        console.log("TickTradePage loaded, data will be provided by DataManager")
    }
} 