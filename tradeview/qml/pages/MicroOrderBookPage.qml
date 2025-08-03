import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: microOrderBookPage
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
                    microOrderBookModel.clear()
                    loadMicroOrderBookData()
                }
            }

            // Max levels setting
            SpinBox {
                id: maxLevelsSpinBox
                from: 3
                to: 20
                value: 10
                stepSize: 1
                background: Rectangle {
                    color: "#353535"
                    border.color: "#404040"
                    border.width: 1
                    radius: 4
                }
                textFromValue: function(value) {
                    return "Levels: " + value
                }
                valueFromText: function(text) {
                    return parseInt(text.replace("Levels: ", ""))
                }
                onValueChanged: {
                    microOrderBookModel.setMaxLevels(value)
                }
            }

            // Highlight threshold setting
            SpinBox {
                id: highlightThresholdSpinBox
                from: 1
                to: 50
                value: 10
                stepSize: 1
                background: Rectangle {
                    color: "#353535"
                    border.color: "#404040"
                    border.width: 1
                    radius: 4
                }
                textFromValue: function(value) {
                    return "Highlight: " + value + "%"
                }
                valueFromText: function(text) {
                    return parseInt(text.replace("Highlight: ", "").replace("%", ""))
                }
                onValueChanged: {
                    microOrderBookModel.setHighlightThreshold(value / 100)
                }
            }

            // Volume threshold setting
            SpinBox {
                id: volumeThresholdSpinBox
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
                    return "Volume Threshold: " + value
                }
                valueFromText: function(text) {
                    return parseInt(text.replace("Volume Threshold: ", ""))
                }
                onValueChanged: {
                    microOrderBookModel.setVolumeThreshold(value)
                }
            }

            // Refresh button
            Button {
                text: "Refresh"
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
                onClicked: loadMicroOrderBookData()
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
                text: "Best Bid: " + (microOrderBookModel.bestBid || "0.00")
                color: "#00ff00"
                font.pixelSize: 12
            }

            Text {
                text: "Best Ask: " + (microOrderBookModel.bestAsk || "0.00")
                color: "#ff0000"
                font.pixelSize: 12
            }

            Text {
                text: "Spread: " + (microOrderBookModel.spread || "0.00")
                color: "#ffffff"
                font.pixelSize: 12
            }
        }
    }

    // Micro order book display area
    Rectangle {
        id: microOrderBookArea
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
            id: microOrderBookTitle
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 10
            text: symbolComboBox.currentText + " Micro Order Book"
            color: "#ffffff"
            font.pixelSize: 16
            font.bold: true
        }

        // Column headers
        Rectangle {
            id: columnHeader
            anchors.top: microOrderBookTitle.bottom
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
                    Layout.preferredWidth: 100
                }

                Text {
                    text: "Orders"
                    color: "#ffffff"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.preferredWidth: 80
                }

                Text {
                    text: "Cumulative"
                    color: "#ffffff"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.preferredWidth: 100
                }

                Text {
                    text: "Depth %"
                    color: "#ffffff"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.fillWidth: true
                }
            }
        }

        // Micro order book list
        ListView {
            id: microOrderBookListView
            anchors.top: columnHeader.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10
            model: microOrderBookModel
            clip: true
            spacing: 1

            delegate: Rectangle {
                width: microOrderBookListView.width
                height: 30
                color: index % 2 === 0 ? "#1e1e1e" : "#252525"
                border.color: "#404040"
                border.width: 0.5

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                    // Price
                    Text {
                        text: model.price ? model.price.toFixed(2) : "0.00"
                        color: model.isBid ? "#00ff00" : "#ff0000"
                        font.pixelSize: 11
                        font.bold: true
                        Layout.preferredWidth: 100
                    }

                    // Volume
                    Text {
                        text: model.volume ? model.volume.toLocaleString() : "0"
                        color: "#ffffff"
                        font.pixelSize: 11
                        Layout.preferredWidth: 100
                    }

                    // Order count
                    Text {
                        text: model.orderCount ? model.orderCount : "0"
                        color: "#cccccc"
                        font.pixelSize: 11
                        Layout.preferredWidth: 80
                    }

                    // Cumulative volume
                    Text {
                        text: model.cumulativeVolume ? model.cumulativeVolume.toLocaleString() : "0"
                        color: "#ffffff"
                        font.pixelSize: 11
                        Layout.preferredWidth: 100
                    }

                    // Depth percentage bar
                    Rectangle {
                        Layout.fillWidth: true
                        height: 15
                        color: "#2d2d2d"
                        border.color: "#404040"
                        border.width: 1
                        radius: 2

                        Rectangle {
                            anchors.left: parent.left
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            width: parent.width * ((model.depthPercentage ? model.depthPercentage : 0) / 100)
                            color: model.isBid ? "#00ff00" : "#ff0000"
                            opacity: 0.3
                            radius: 2
                        }

                        Text {
                            anchors.centerIn: parent
                            text: (model.depthPercentage ? model.depthPercentage.toFixed(1) : "0.0") + "%"
                            color: "#ffffff"
                            font.pixelSize: 9
                        }
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
                console.log("Micro order book data refreshed by DataManager")
            }
        }
    }

    // Function to load initial data
    function loadMicroOrderBookData() {
        // Data will be provided by DataManager
        console.log("Loading micro order book data...")
    }

    // Initialize data when page loads
    Component.onCompleted: {
        loadMicroOrderBookData()
    }
} 
} 