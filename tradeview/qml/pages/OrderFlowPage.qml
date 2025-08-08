import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Shapes 1.15

Page {
    id: orderFlowPage
    background: Rectangle {
        color: "#1e1e1e"
    }

    property string symbol: ""
    property var candlestickModel: null

    // Zoom factor property - controls the size of candlesticks
    property real zoomFactor: 1.0
    property int maxVisibleCandlesticks: 50  // Maximum number of candlesticks visible at once

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
                    // Switch model and reload data when symbol changes
                    // console.log("Symbol changed to:", currentText)
                    switchCandlestickModel(currentText)
                }
            }

            // Timeframe selection
            ComboBox {
                id: timeframeComboBox
                model: ["1m", "5m", "15m", "1h", "4h", "1d"]
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
                    if (candlestickModel) {
                        candlestickModel.set_timeframe(currentText)
                        // console.log("Timeframe changed to:", currentText)
                        // Force repaint to show new timeframe data
                        candlestickCanvas.requestPaint()
                        volumeCanvas.requestPaint()
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
                    candlestickCanvas.requestPaint()
                    volumeCanvas.requestPaint()
                }
            }

            // Zoom controls
            Button {
                text: "Zoom In"
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
                    zoomFactor = Math.min(zoomFactor * 1.2, 5.0)
                    candlestickCanvas.requestPaint()
                    volumeCanvas.requestPaint()
                }
            }

            Button {
                text: "Zoom Out"
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
                    zoomFactor = Math.max(zoomFactor / 1.2, 0.2)
                    candlestickCanvas.requestPaint()
                    volumeCanvas.requestPaint()
                }
            }

            Item { Layout.fillWidth: true }

            // Statistics
            Text {
                text: "High: " + (function() {
                    if (candlestickModel && candlestickModel.count > 0) {
                        let candlestick = getCandlestickData(candlestickModel.count - 1)
                        return candlestick && candlestick.high ? candlestick.high.toFixed(2) : "0.00"
                    }
                    return "0.00"
                })()
                color: "#00ff00"
                font.pixelSize: 12
            }

            Text {
                text: "Low: " + (function() {
                    if (candlestickModel && candlestickModel.count > 0) {
                        let candlestick = getCandlestickData(candlestickModel.count - 1)
                        return candlestick && candlestick.low ? candlestick.low.toFixed(2) : "0.00"
                    }
                    return "0.00"
                })()
                color: "#ff0000"
                font.pixelSize: 12
            }

            Text {
                text: "Volume: " + (function() {
                    if (candlestickModel && candlestickModel.count > 0) {
                        let candlestick = getCandlestickData(candlestickModel.count - 1)
                        return candlestick && candlestick.volume ? candlestick.volume : "0"
                    }
                    return "0"
                })()
                color: "#ffffff"
                font.pixelSize: 12
            }

            // Debug info
            Text {
                text: "Count: " + (candlestickModel ? candlestickModel.count : "0")
                color: "#ffff00"
                font.pixelSize: 12
            }

            Text {
                text: "Zoom: " + zoomFactor.toFixed(1) + "x"
                color: "#00ffff"
                font.pixelSize: 12
            }
        }
    }

    // Candlestick chart area
    Rectangle {
        id: candlestickArea
        anchors.top: toolbar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        color: "#1e1e1e"
        border.color: "#404040"
        border.width: 1

        // Chart title
        Text {
            id: chartTitle
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 10
            text: symbolComboBox.currentText + " - " + timeframeComboBox.currentText + " Candlestick Chart"
            color: "#ffffff"
            font.pixelSize: 16
            font.bold: true
        }

        // Debug info
        Text {
            id: debugInfo
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 10
            text: "Canvas: " + candlestickCanvas.width + "x" + candlestickCanvas.height
            color: "#ffff00"
            font.pixelSize: 10
        }

        // Candlestick chart
        Canvas {
            id: candlestickCanvas
            anchors.top: chartTitle.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10
            width: parent.width - 20
            height: parent.height - 20
            antialiasing: true

            onPaint: {
                // console.log("Canvas paint triggered, model count:", candlestickModel ? candlestickModel.count : "undefined")
                
                if (!candlestickModel || candlestickModel.count === 0) {
                    console.log("No candlestick data available, candlestickModel name: ", candlestickModel ? candlestickModel.get_name() : "undefined")
                    return
                }

                let ctx = getContext("2d")
                ctx.reset()

                // Calculate visible candlesticks based on zoom factor
                let baseBarWidth = 8  // Base width of each candlestick
                let barSpacing = 2    // Spacing between candlesticks
                let effectiveBarWidth = baseBarWidth * zoomFactor
                let totalBarWidth = effectiveBarWidth + barSpacing
                
                // Calculate how many candlesticks can fit in the canvas
                let maxVisibleBars = Math.floor((width - 20) / totalBarWidth)
                let barsToShow = Math.min(maxVisibleBars, maxVisibleCandlesticks)
                
                // Determine which candlesticks to show (sliding window from right)
                let startIndex = Math.max(0, candlestickModel.count - barsToShow)
                let endIndex = candlestickModel.count

                // Calculate price range for visible candlesticks
                let minPrice = Number.MAX_VALUE
                let maxPrice = Number.MIN_VALUE
                
                for (let i = startIndex; i < endIndex; i++) {
                    let candlestick = getCandlestickData(i)
                    if (candlestick) {
                        minPrice = Math.min(minPrice, candlestick.low)
                        maxPrice = Math.max(maxPrice, candlestick.high)
                    }
                }

                if (minPrice === Number.MAX_VALUE || maxPrice === Number.MIN_VALUE) {
                    console.log("No valid price range found")
                    return
                }

                // console.log("Price range:", minPrice, "to", maxPrice)

                let priceRange = maxPrice - minPrice
                if (priceRange === 0) priceRange = 1

                // Draw grid lines
                ctx.strokeStyle = "#404040"
                ctx.lineWidth = 1
                
                // Horizontal grid lines
                for (let i = 0; i <= 5; i++) {
                    let y = (i / 5) * height
                    ctx.beginPath()
                    ctx.moveTo(0, y)
                    ctx.lineTo(width, y)
                    ctx.stroke()
                }

                // Vertical grid lines
                for (let i = 0; i <= 10; i++) {
                    let x = (i / 10) * width
                    ctx.beginPath()
                    ctx.moveTo(x, 0)
                    ctx.lineTo(x, height)
                    ctx.stroke()
                }

                // Draw candlesticks from left to right
                let startX = 10  // Start from left margin
                // console.log("Drawing", barsToShow, "candlesticks with bar width:", effectiveBarWidth)

                for (let i = 0; i < endIndex - startIndex; i++) {
                    let dataIndex = startIndex + i
                    let candlestick = getCandlestickData(dataIndex)

                    if (!candlestick) {
                        console.log("Skipping null candlestick at index", dataIndex)
                        continue
                    }

                    let x = startX + i * totalBarWidth
                    let openY = height - ((candlestick.open - minPrice) / priceRange) * height
                    let closeY = height - ((candlestick.close - minPrice) / priceRange) * height
                    let highY = height - ((candlestick.high - minPrice) / priceRange) * height
                    let lowY = height - ((candlestick.low - minPrice) / priceRange) * height

                    // Ensure valid coordinates
                    openY = Math.max(0, Math.min(height, openY))
                    closeY = Math.max(0, Math.min(height, closeY))
                    highY = Math.max(0, Math.min(height, highY))
                    lowY = Math.max(0, Math.min(height, lowY))

                    // Draw candlestick body
                    let bodyHeight = Math.abs(closeY - openY)
                    if (bodyHeight < 1) bodyHeight = 1

                    let bodyY = candlestick.close >= candlestick.open ? closeY : openY
                    
                    if (candlestick.close >= candlestick.open) {
                        ctx.fillStyle = "#00ff00"
                    } else {
                        ctx.fillStyle = "#ff0000"
                    }

                    // Debug: Log drawing coordinates
                    // console.log("Drawing candlestick at x:", x, "bodyY:", bodyY, "width:", effectiveBarWidth, "height:", bodyHeight)

                    ctx.fillRect(x, bodyY, effectiveBarWidth, bodyHeight)
                    
                    // Draw upper shadow
                    ctx.strokeStyle = candlestick.close >= candlestick.open ? "#00cc00" : "#cc0000"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    ctx.moveTo(x + effectiveBarWidth/2, bodyY)
                    ctx.lineTo(x + effectiveBarWidth/2, highY)
                    ctx.stroke()

                    // Draw lower shadow
                    ctx.beginPath()
                    ctx.moveTo(x + effectiveBarWidth/2, bodyY + bodyHeight)
                    ctx.lineTo(x + effectiveBarWidth/2, lowY)
                    ctx.stroke()
                }

                console.log("Canvas painting completed")
            }

            // Mouse hover effect
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onPositionChanged: {
                    // Can add tooltip or highlight functionality here
                }
            }

            // Scrollbar
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

        // Volume chart
        Rectangle {
            id: volumeArea
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: 100
            color: "#1a1a1a"
            border.color: "#404040"
            border.width: 1

            Canvas {
                id: volumeCanvas
                anchors.fill: parent
                anchors.margins: 5
                antialiasing: true

                onPaint: {
                    if (!candlestickModel || candlestickModel.count === 0) return

                    let ctx = getContext("2d")
                    ctx.reset()

                    // Use same logic as candlestick chart for consistency
                    let baseBarWidth = 8
                    let barSpacing = 2
                    let effectiveBarWidth = baseBarWidth * zoomFactor
                    let totalBarWidth = effectiveBarWidth + barSpacing
                    
                    let maxVisibleBars = Math.floor((width - 20) / totalBarWidth)
                    let barsToShow = Math.min(maxVisibleBars, maxVisibleCandlesticks)
                    
                    let startIndex = Math.max(0, candlestickModel.count - barsToShow)
                    let endIndex = candlestickModel.count
                    
                    let maxVolume = 0
                    
                    for (let i = startIndex; i < endIndex; i++) {
                        let candlestick = getCandlestickData(i)
                        if (candlestick && candlestick.volume !== undefined) {
                            maxVolume = Math.max(maxVolume, candlestick.volume)
                        }
                    }

                    if (maxVolume === 0) return

                    let startX = 10

                    for (let i = 0; i < endIndex - startIndex; i++) {
                        let dataIndex = startIndex + i
                        let candlestick = getCandlestickData(dataIndex)
                        if (!candlestick || typeof candlestick.volume !== 'number') continue

                        let x = startX + i * totalBarWidth
                        let volumeHeight = (candlestick.volume / maxVolume) * height
                        let volumeY = height - volumeHeight

                        ctx.fillStyle = candlestick.close >= candlestick.open ? "#00ff00" : "#ff0000"
                        ctx.fillRect(x, volumeY, effectiveBarWidth, volumeHeight)
                    }
                }
            }
        }
    }

    // Canvas update timer
    Timer {
        id: canvasUpdateTimer
        interval: 1000  // Update canvas every second
        running: true
        repeat: true
        onTriggered: {
            candlestickCanvas.requestPaint()
            volumeCanvas.requestPaint()
        }
    }

    // Function to switch candlestick model for different symbol
    function switchCandlestickModel(symbol) {
        console.log("Switching candlestick model for symbol:", symbol)
        
        // Get new model from model manager
        candlestickModel = model_mgr.reqCandlestickModel({"symbol": symbol})
        
        // Clear existing data and reload
        if (candlestickModel) {
            console.log("Switched to new model, cleared data, waiting for C++ data generation...")
            
            // Force canvas repaint with new model
            candlestickCanvas.requestPaint()
            volumeCanvas.requestPaint()
        } else {
            console.log("Failed to get model for symbol:", symbol)
        }
    }

    // Helper function to safely get candlestick data
    function getCandlestickData(index) {
        if (!candlestickModel || index < 0 || index >= candlestickModel.count) {
            console.log("Invalid index or model:", index, "count:", candlestickModel ? candlestickModel.count : "null")
            return null
        }
        
        // Use the get method which returns QVariantMap
        let candlestick = candlestickModel.get(index)
        
        // Validate the data
        if (candlestick && typeof candlestick.open === 'number' && typeof candlestick.close === 'number' && 
            typeof candlestick.high === 'number' && typeof candlestick.low === 'number') {
            return candlestick
        }
        
        console.log("Invalid data returned for index", index, ":", candlestick)
        return null
    }

    // Initialize data when page loads
    Component.onCompleted: {
        console.log("OrderFlowPage loaded")
        
        // Initialize with first symbol from config
        if (symbolComboBox.count > 0) {
            let initialSymbol = symbolComboBox.currentText
            switchCandlestickModel(initialSymbol)
        }
    }
    
    // Listen for data changes from C++ model
    Connections {
        target: candlestickModel
        
        function onDataChanged() {
            // Verify we're still connected to the current model
            if (candlestickModel && candlestickModel === target) {
                // console.log("Candlestick data changed, count:", candlestickModel.count, "model name:", candlestickModel.get_name())
                candlestickCanvas.requestPaint()
                volumeCanvas.requestPaint()
            } else {
                console.log("Data changed but model connection may be stale")
            }
        }
        
        function onCandlestickAdded() {
            // Verify we're still connected to the current model
            if (candlestickModel && candlestickModel === target) {
                // console.log("New candlestick added, total count:", candlestickModel.count, "model name:", candlestickModel.get_name())
                candlestickCanvas.requestPaint()
                volumeCanvas.requestPaint()
            } else {
                console.log("Candlestick added but model connection may be stale")
            }
        }
    }

    // Listen for config changes
    Connections {
        target: configManager
        
        function onConfigLoaded() {
            console.log("Config loaded, updating symbol list")
            // Force ComboBox to update its model
            if (configManager && typeof configManager.getEnabledInstruments === 'function') {
                symbolComboBox.model = configManager.getEnabledInstruments()
                
                // Initialize with first symbol if no model is set yet
                if (!candlestickModel && symbolComboBox.count > 0) {
                    let initialSymbol = symbolComboBox.currentText
                    switchCandlestickModel(initialSymbol)
                }
            } else {
                console.log("ConfigManager methods not available")
            }
        }
    }
} 