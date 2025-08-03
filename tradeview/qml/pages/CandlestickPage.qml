import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Shapes 1.15

Page {
    id: candlestickPage
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
                    // Reload data when symbol changes
                    candlestickModel.clear()
                    loadCandlestickData()
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
                    candlestickModel.set_timeframe(currentText)
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
                    loadCandlestickData()
                    candlestickCanvas.requestPaint()
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
                text: "High: " + (function() {
                    if (candlestickModel && candlestickModel.count > 0) {
                        let candlestick = candlestickModel.get_candlestick(candlestickModel.count - 1)
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
                        let candlestick = candlestickModel.get_candlestick(candlestickModel.count - 1)
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
                        let candlestick = candlestickModel.get_candlestick(candlestickModel.count - 1)
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
                console.log("Canvas paint triggered, model count:", candlestickModel ? candlestickModel.count : "undefined")
                
                if (!candlestickModel || candlestickModel.count === 0) {
                    console.log("No candlestick data available")
                    return
                }

                let ctx = getContext("2d")
                ctx.reset()

                // Calculate price range
                let minPrice = Number.MAX_VALUE
                let maxPrice = Number.MIN_VALUE
                let barsToShow = Math.min(candlestickModel.count, 100)
                
                console.log("Processing", barsToShow, "candlesticks")
                
                for (let i = 0; i < barsToShow; i++) {
                    let candlestick = getCandlestickData(i)
                    if (candlestick && candlestick.low !== undefined && candlestick.high !== undefined) {
                        minPrice = Math.min(minPrice, candlestick.low)
                        maxPrice = Math.max(maxPrice, candlestick.high)
                    }
                }

                if (minPrice === Number.MAX_VALUE || maxPrice === Number.MIN_VALUE) {
                    console.log("No valid price range found")
                    return
                }

                console.log("Price range:", minPrice, "to", maxPrice)

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

                // Draw candlesticks
                let barWidth = Math.max(2, (width - 20) / barsToShow - 2)
                let barSpacing = 2
                let totalWidth = barsToShow * (barWidth + barSpacing)
                let startX = (width - totalWidth) / 2

                console.log("Drawing", barsToShow, "candlesticks with bar width:", barWidth)

                for (let i = 0; i < barsToShow; i++) {
                    let candlestick = getCandlestickData(i)
                    if (!candlestick || candlestick.open === undefined || candlestick.close === undefined || 
                        candlestick.high === undefined || candlestick.low === undefined) {
                        console.log("Skipping invalid candlestick at index", i)
                        continue
                    }

                    let x = startX + i * (barWidth + barSpacing)
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

                    ctx.fillRect(x, bodyY, barWidth, bodyHeight)
                    
                    // Draw upper shadow
                    ctx.strokeStyle = candlestick.close >= candlestick.open ? "#00cc00" : "#cc0000"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    ctx.moveTo(x + barWidth/2, bodyY)
                    ctx.lineTo(x + barWidth/2, highY)
                    ctx.stroke()

                    // Draw lower shadow
                    ctx.beginPath()
                    ctx.moveTo(x + barWidth/2, bodyY + bodyHeight)
                    ctx.lineTo(x + barWidth/2, lowY)
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

                    let barsToShow = Math.min(candlestickModel.count, 100)
                    let maxVolume = 0
                    
                    for (let i = 0; i < barsToShow; i++) {
                        let candlestick = candlestickModel.get_candlestick(i)
                        if (candlestick && candlestick.volume !== undefined) {
                            maxVolume = Math.max(maxVolume, candlestick.volume)
                        }
                    }

                    if (maxVolume === 0) return

                    let barWidth = Math.max(2, (width - 20) / barsToShow - 2)
                    let barSpacing = 2
                    let totalWidth = barsToShow * (barWidth + barSpacing)
                    let startX = (width - totalWidth) / 2

                    for (let i = 0; i < barsToShow; i++) {
                        let candlestick = candlestickModel.get_candlestick(i)
                        if (!candlestick || candlestick.volume === undefined || candlestick.close === undefined || candlestick.open === undefined) continue

                        let x = startX + i * (barWidth + barSpacing)
                        let volumeHeight = (candlestick.volume / maxVolume) * height
                        let volumeY = height - volumeHeight

                        ctx.fillStyle = candlestick.close >= candlestick.open ? "#00ff00" : "#ff0000"
                        ctx.fillRect(x, volumeY, barWidth, volumeHeight)
                    }
                }
            }
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
                console.log("Auto refresh triggered")
                candlestickCanvas.requestPaint()
                volumeCanvas.requestPaint()
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

    // Function to load initial data
    function loadCandlestickData() {
        console.log("Loading candlestick data for symbol:", symbolComboBox.currentText)
        
        // Clear existing data and let C++ DataManager handle data generation
        if (candlestickModel) {
            candlestickModel.clear()
            console.log("Cleared existing candlestick data, waiting for C++ data generation...")
        }
    }

    // Helper function to safely get candlestick data
    function getCandlestickData(index) {
        if (!candlestickModel || index < 0 || index >= candlestickModel.count) {
            return null
        }
        
        // Use get method which returns QVariantMap
        let data = candlestickModel.get(index)
        if (data && data.open !== undefined) {
            return data
        }
        
        return null
    }

    // Initialize data when page loads
    Component.onCompleted: {
        console.log("CandlestickPage loaded")
        loadCandlestickData()
        
        // Force initial paint
        candlestickCanvas.requestPaint()
        volumeCanvas.requestPaint()
    }
    
    // Listen for data changes from C++ model
    Connections {
        target: candlestickModel
        
        function onDataChanged() {
            console.log("Candlestick data changed, count:", candlestickModel.count)
            candlestickCanvas.requestPaint()
            volumeCanvas.requestPaint()
        }
        
        function onCandlestickAdded() {
            console.log("New candlestick added, total count:", candlestickModel.count)
            candlestickCanvas.requestPaint()
            volumeCanvas.requestPaint()
        }
    }
} 