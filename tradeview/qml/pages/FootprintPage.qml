import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: footprintPage
    background: Rectangle {
        color: "#1e1e1e"
    }

    // Toolbar
    ChartToolbar {
        id: toolbar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        symbolModel: ["BTC/USDT", "ETH/USDT", "BNB/USDT", "ADA/USDT"]
        timeframeModel: ["1m", "5m", "15m", "1h"]
        currentSymbol: "BTC/USDT"
        currentTimeframe: "1m"
        autoRefresh: true
        refreshInterval: 5000

        onSymbolChanged: function(symbol) {
            footprintModel.clear()
            loadFootprintData()
        }

        onTimeframeChanged: function(timeframe) {
            footprintModel.set_timeframe(timeframe)
        }

        onRefreshClicked: function() {
            loadFootprintData()
        }

        onAutoRefreshChanged: function(enabled) {
            autoRefreshTimer.running = enabled
        }

        // Add price step setting
        SpinBox {
            anchors.right: parent.right
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            from: 1
            to: 100
            value: 10
            stepSize: 1
            background: Rectangle {
                color: "#353535"
                border.color: "#404040"
                border.width: 1
                radius: 4
            }
            textFromValue: function(value) {
                return "Price Step: " + value
            }
            valueFromText: function(text) {
                return parseInt(text.replace("Price Step: ", ""))
            }
            onValueChanged: {
                footprintModel.set_price_step(value)
            }
        }

        // Statistics
        RowLayout {
            anchors.right: parent.right
            anchors.rightMargin: 200
            anchors.verticalCenter: parent.verticalCenter
            spacing: 10

            Text {
                text: "Total Buy: " + (footprintModel.count > 0 ? footprintModel.get_footprint_bar(footprintModel.count - 1).totalBuyVolume : "0")
                color: "#00ff00"
                font.pixelSize: 12
            }

            Text {
                text: "Total Sell: " + (footprintModel.count > 0 ? footprintModel.get_footprint_bar(footprintModel.count - 1).totalSellVolume : "0")
                color: "#ff0000"
                font.pixelSize: 12
            }

            Text {
                text: "Delta: " + (footprintModel.count > 0 ? footprintModel.get_footprint_bar(footprintModel.count - 1).delta : "0")
                color: "#ffffff"
                font.pixelSize: 12
            }
        }
    }

    // Footprint chart display area
    FootprintChart {
        id: footprintArea
        anchors.top: toolbar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        model: footprintModel
        cellWidth: 120
        cellHeight: 80
        showVolume: true
        showDelta: true
        showPercent: true

        onCellClicked: function(index, cellData) {
            console.log("Footprint chart cell clicked:", index, cellData)
        }

        onCellHovered: function(index, cellData) {
            // Can show detailed information here
        }
    }

    // Auto refresh timer
    Timer {
        id: autoRefreshTimer
        interval: 5000  // Refresh every 5 seconds
        running: autoRefreshCheckBox.checked
        repeat: true
        onTriggered: {
            if (autoRefreshCheckBox.checked) {
                addRandomFootprintData()
            }
        }
    }

    // Function to add random footprint data
    function addRandomFootprintData() {
        const basePrice = 50000 + Math.random() * 1000
        const open = basePrice
        const close = basePrice + (Math.random() - 0.5) * 100
        const high = Math.max(open, close) + Math.random() * 50
        const low = Math.min(open, close) - Math.random() * 50

        // Add some price cell data
        for (let i = 0; i < 10; i++) {
            const price = basePrice + (i - 5) * priceStepSpinBox.value
            const buyVolume = Math.floor(Math.random() * 1000) + 100
            const sellVolume = Math.floor(Math.random() * 1000) + 100
            const direction = Math.random() > 0.5 ? "buy" : "sell"

            footprintModel.add_tick_data(new Date(), price, buyVolume + sellVolume, direction)
        }
    }

    // Function to load initial data
    function loadFootprintData() {
        // Add some initial data
        for (let i = 0; i < 20; i++) {
            addRandomFootprintData()
        }
    }

    // Initialize data when page loads
    Component.onCompleted: {
        loadFootprintData()
    }
} 