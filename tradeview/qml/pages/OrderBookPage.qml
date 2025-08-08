import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: orderBookPage
    background: Rectangle {
        color: "#1e1e1e"
    }

    property string symbol: ""
    property var orderBookModel: null

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
                model: {
                    if (configManager && typeof configManager.getEnabledInstruments === 'function') {
                        return configManager.getEnabledInstruments()
                    } else {
                        console.log("ConfigManager not available, using default symbols")
                        return null
                    }
                }
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
                    if (currentText !== "") {
                        symbol = currentText
                        loadOrderBookData()
                    }
                }
            }

            // Max levels setting
            SpinBox {
                id: maxLevelsSpinBox
                from: 5
                to: 50
                value: 20
                stepSize: 5
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
                    if (orderBookModel) {
                        orderBookModel.setMaxLevels(value)
                    }
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
                onClicked: {
                    // console.log("Manual refresh requested")
                    loadOrderBookData()
                }
            }

            // Debug button
            Button {
                text: "Debug"
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
                onClicked: {
                    console.log("Debug button clicked")
                    console.log("Symbol:", symbol)
                    console.log("OrderBookModel:", orderBookModel)
                    if (orderBookModel) {
                        console.log("Row count:", orderBookModel.rowCount)
                        console.log("Best bid:", orderBookModel.getBestBid())
                        console.log("Best ask:", orderBookModel.getBestAsk())
                        console.log("Spread:", orderBookModel.getSpread())
                    }
                }
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
                text: "Best Bid: " + (orderBookModel && orderBookModel.getBestBid ? orderBookModel.getBestBid().toFixed(2) : "0.00")
                color: "#00ff00"
                font.pixelSize: 12
            }

            Text {
                text: "Best Ask: " + (orderBookModel && orderBookModel.getBestAsk ? orderBookModel.getBestAsk().toFixed(2) : "0.00")
                color: "#ff0000"
                font.pixelSize: 12
            }

            Text {
                text: "Spread: " + (orderBookModel && orderBookModel.getSpread ? orderBookModel.getSpread().toFixed(2) : "0.00")
                color: "#ffffff"
                font.pixelSize: 12
            }

            Text {
                text: "Rows: " + (orderBookModel ? orderBookModel.rowCount : "0")
                color: "#ffff00"
                font.pixelSize: 12
            }
        }
    }

    // Order book display area
    Rectangle {
        id: orderBookArea
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
            id: orderBookTitle
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 10
            text: symbolComboBox.currentText + " Order Book"
            color: "#ffffff"
            font.pixelSize: 16
            font.bold: true
        }

        // Column headers
        Rectangle {
            id: columnHeader
            anchors.top: orderBookTitle.bottom
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

        // Order book list
        ListView {
            id: orderBookListView
            anchors.top: columnHeader.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10
            model: orderBookModel
            clip: true
            spacing: 1

            delegate: Rectangle {
                width: orderBookListView.width
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
        interval: 2000  // Refresh every 2 seconds
        running: autoRefreshCheckBox.checked
        repeat: true
        onTriggered: {
            if (autoRefreshCheckBox.checked) {
                // Data is automatically updated by DataManager, no manual update needed
                console.log("Order book data refreshed by DataManager")
            }
        }
    }

    // Function to load order book data for a specific symbol
    function loadOrderBookData() {
        console.log("Loading order book data for symbol:", symbol)
        
        if (!symbol || symbol === "") {
            console.log("No symbol specified, using default")
            symbol = symbolComboBox.currentText
        }
        
        if (typeof model_mgr !== 'undefined' && model_mgr) {
            try {
                // Get order book model from model manager
                orderBookModel = model_mgr.reqOrderBookModel({"symbol": symbol})
                
                if (orderBookModel) {
                    console.log("Successfully loaded order book model for symbol:", symbol)
                    
                    // Set max levels if spinbox has a value
                    if (maxLevelsSpinBox.value > 0) {
                        orderBookModel.setMaxLevels(maxLevelsSpinBox.value)
                    }
                    
                    // Connect to model signals for real-time updates
                    orderBookModel.dataChanged.connect(function() {
                        console.log("Order book data changed for symbol:", symbol)
                        // Force ListView refresh
                        orderBookListView.model = null
                        orderBookListView.model = orderBookModel
                    })
                    
                    orderBookModel.orderBookUpdated.connect(function() {
                        console.log("Order book updated for symbol:", symbol)
                        // Force ListView refresh
                        orderBookListView.model = null
                        orderBookListView.model = orderBookModel
                    })
                    
                } else {
                    console.log("Failed to get order book model for symbol:", symbol)
                }
            } catch (error) {
                console.error("Error loading order book data:", error)
            }
        } else {
            console.log("Model manager not available")
        }
    }

    // Initialize data when page loads
    Component.onCompleted: {
        console.log("OrderBookPage loaded, initializing order book model")
        
        // Set initial symbol
        if (symbolComboBox.count > 0) {
            symbol = symbolComboBox.currentText
        }
        
        // Load initial order book data
        loadOrderBookData()
        
        // Force initial data generation if no data exists
        if (orderBookModel && orderBookModel.rowCount() === 0) {
            console.log("No order book data found, triggering initial data generation")
            // Trigger a manual refresh to generate initial data
            setTimeout(function() {
                if (orderBookModel && orderBookModel.rowCount() === 0) {
                    console.log("Still no data, forcing refresh")
                    loadOrderBookData()
                }
            }, 1000)
        }
        
        console.log("OrderBookPage initialization completed")
    }
} 