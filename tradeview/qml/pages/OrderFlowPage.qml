// OrderFlowPage.qml
//
// Purpose:
// - Interactive candlestick (OHLC) + volume chart page with time axis
// - Supports timeframe switching, symbol switching, zoom/pan, hover crosshair,
//   rubber-band zoom, adaptive ticks, and performance-conscious rendering
//
// Performance notes:
// - Canvases render with antialiasing disabled for faster rasterization
// - Repaints are coalesced via a 16ms debounce timer to avoid flooding
// - Level-of-detail: wicks are skipped when bars are too thin
// - Heavy calculations are precomputed (e.g., vertical scale factor)

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Shapes 1.15

Page {
    id: orderFlowPage
    background: Rectangle {
        color: "#1e1e1e"
    }

    //
    // Core state and configuration
    // - candlestickModel: data provider from C++ side (QAbstractListModel-like)
    // - zoomFactor/panOffsetBars: controls viewport scale and horizontal pan
    // - visual constants: padding, spacing, axis sizes
    //
    property string symbol: ""
    property var candlestickModel: null

    // Zoom factor property - controls the size of candlesticks
    property real zoomFactor: 1.0
    property int maxVisibleCandlesticks: 1000  // Maximum number of candlesticks visible at once
    property int barBaseWidth: 8
    property int barSpacing: 2
    property bool debugLogs: false
    property int gridHLines: 5
    property int gridVLines: 10
    property int canvasMargin: 10
    // Pan/zoom and hover state
    property real panOffsetBars: 0           // how many bars from the right are hidden due to panning (allow fractional for damping)
    property real hoverX: -1
    property real hoverY: -1
    property bool hoverVisible: false
    property int hoveredIndex: -1
    // Volume canvas hover state
    property bool volumeHoverVisible: false
    property real volumeHoverX: -1
    property real volumeHoverY: -1
    property int volumeHoveredIndex: -1
    // Tooltip options: "fixed" | "follow" (default follow: show at mouse position)
    property string tooltipMode: "follow"
    property int tooltipMaxWidth: 180
    // Vertical padding (px) to leave space at top/bottom for candle drawing
    property int priceVerticalPadding: 12
    // Spacing between candlestick chart and volume chart
    property int separatorHeight: 14
    // Time axis height
    property int timeAxisHeight: 28
    // Minimum pixel spacing between time ticks
    property int minTimeTickPx: 60
    // Control periodic canvas refresh to reduce unnecessary repaints
    property bool autoRefresh: false
    // Debounce flag for UI repaint coalescing
    property bool repaintScheduled: false

    // Debounced repaint helper
    Timer {
        id: repaintTimer
        interval: 16 // ~60 FPS
        running: false
        repeat: false
        onTriggered: {
            repaintScheduled = false
            candlestickCanvas.requestPaint()
            volumeCanvas.requestPaint()
            timeAxisCanvas.requestPaint()
        }
    }

    function requestRepaintDebounced() {
        if (!repaintScheduled) {
            repaintScheduled = true
            repaintTimer.start()
        }
    }

    // Convert timeframe text (e.g., "1m", "1h") to minutes
    function timeframeMinutes() {
        const tf = timeframeComboBox.currentText
        if (!tf) return 1
        if (tf.endsWith("m")) return parseInt(tf) || 1
        if (tf.endsWith("h")) return (parseInt(tf) || 1) * 60
        if (tf.endsWith("d")) return (parseInt(tf) || 1) * 60 * 24
        return 1
    }

    // Select a compact time label format based on the current zoom density
    function formatTimeLabel(dt, stepBars) {
        // stepBars in number of bars; choose format by overall scale
        const minutesPerBar = timeframeMinutes()
        const minutesPerTick = minutesPerBar * Math.max(1, stepBars)
        // Heuristics for label format
        if (minutesPerTick >= 24*60) {
            return dt.toLocaleDateString()
        } else if (minutesPerTick >= 60) {
            // show date + hour
            return dt.toLocaleDateString() + " " + dt.toLocaleTimeString().slice(0,5)
        } else {
            return dt.toLocaleTimeString()
        }
    }

    // Compose small statistics text for the toolbar (High/Low/Vol/Count/Zoom)
    function buildStatsText() {
        let last = null
        if (candlestickModel && candlestickModel.count > 0) {
            last = getCandlestickData(candlestickModel.count - 1)
        }
        const high = (last && typeof last.high === 'number') ? last.high.toFixed(2) : "0.00"
        const low = (last && typeof last.low === 'number') ? last.low.toFixed(2) : "0.00"
        const vol = (last && typeof last.volume === 'number') ? last.volume.toLocaleString() : "0"
        const count = candlestickModel ? candlestickModel.count : 0
        const zoom = zoomFactor.toFixed(1) + "x"
        return `<span style="color:#00ff00">High: ${high}</span> | ` +
               `<span style="color:#ff0000">Low: ${low}</span> | ` +
               `<span style="color:#ffffff">Volume: ${vol}</span> | ` +
               `<span style="color:#ffff00">Count: ${count}</span> | ` +
               `<span style="color:#00ffff">Zoom: ${zoom}</span>`
    }

    // Compute view parameters shared by canvases
    // Compute viewport parameters shared across canvases
    // Returns: effectiveBarWidth, totalBarWidth, barsToShow, startIndex, endIndex
    function computeViewParams(canvasWidth) {
        const effectiveBarWidth = barBaseWidth * zoomFactor
        const totalBarWidth = effectiveBarWidth + barSpacing
        const maxVisibleBars = Math.max(1, Math.floor((canvasWidth - 2*canvasMargin) / totalBarWidth))
        const barsToShow = Math.min(maxVisibleBars, maxVisibleCandlesticks)
        const count = candlestickModel ? candlestickModel.count : 0
        // Apply panning (rounded to integer bars for indexing)
        const endIndex = Math.max(0, Math.min(count, count - Math.round(panOffsetBars)))
        const startIndex = Math.max(0, endIndex - barsToShow)
        return {
            effectiveBarWidth: effectiveBarWidth,
            totalBarWidth: totalBarWidth,
            barsToShow: barsToShow,
            startIndex: startIndex,
            endIndex: endIndex
        }
    }

    // Measure price range for visible items while caching data for the paint loop
    function measurePriceRangeAndCollect(startIndex, endIndex) {
        const data = []
        let minPrice = Number.MAX_VALUE
        let maxPrice = Number.MIN_VALUE
        for (let i = startIndex; i < endIndex; i++) {
            const c = getCandlestickData(i)
            data.push(c)
            if (c) {
                minPrice = Math.min(minPrice, c.low)
                maxPrice = Math.max(maxPrice, c.high)
            }
        }
        return { data: data, minPrice: minPrice, maxPrice: maxPrice }
    }

    // Draw background grid for readability (H/V lines)
    function drawGrid(ctx, w, h) {
        ctx.strokeStyle = "#404040"
        ctx.lineWidth = 1
        const top = priceVerticalPadding
        const bottom = h - priceVerticalPadding
        const drawableH = Math.max(0, bottom - top)
        // Horizontal lines
        for (let i = 0; i <= gridHLines; i++) {
            const y = top + (i / gridHLines) * drawableH + 0.5
            ctx.beginPath()
            ctx.moveTo(0, y)
            ctx.lineTo(w, y)
            ctx.stroke()
        }
        // Vertical lines
        for (let i = 0; i <= gridVLines; i++) {
            const x = (i / gridVLines) * w + 0.5
            ctx.beginPath()
            ctx.moveTo(x, 0)
            ctx.lineTo(x, h)
            ctx.stroke()
        }
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
                        timeAxisCanvas.requestPaint()
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
                onClicked: { requestRepaintDebounced() }
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
                    requestRepaintDebounced()
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
                    requestRepaintDebounced()
                }
            }

            Button {
                text: "Reset View"
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
                    zoomFactor = 1.0
                    panOffsetBars = 0
                    hoveredIndex = -1
                    requestRepaintDebounced()
                }
            }

            Item { Layout.fillWidth: true }

            // Statistics (merged)
            Text {
                textFormat: Text.RichText
                text: buildStatsText()
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
            text: symbolComboBox.currentText + " - " + timeframeComboBox.currentText + " Order Flow Chart"
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
        // Responsible for OHLC bodies/wicks, crosshair, price axis labels and inline time ticks
        Canvas {
            id: candlestickCanvas
            z: 1
            anchors.top: chartTitle.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: timeAxisCanvas.top
            anchors.margins: 10
            antialiasing: false

            onPaint: {
                // Rendering pipeline:
                // 1) Validate model and reset context
                // 2) Compute viewport and collect visible data + price range
                // 3) Draw grid, then iterate visible bars to draw bodies/wicks
                // 4) Draw crosshair + tooltip if hovering
                // 5) Draw price labels and inline time ticks
                // console.log("Canvas paint triggered, model count:", candlestickModel ? candlestickModel.count : "undefined")
                
                if (!candlestickModel || candlestickModel.count === 0) {
                    if (debugLogs)
                        console.log("No candlestick data available, candlestickModel name: ", candlestickModel ? candlestickModel.get_name() : "undefined")
                    return
                }

                let ctx = getContext("2d")
                ctx.reset()

                // Compute viewport params
                const vp = computeViewParams(width)
                const effectiveBarWidth = vp.effectiveBarWidth
                const totalBarWidth = vp.totalBarWidth
                const startIndex = vp.startIndex
                const endIndex = vp.endIndex

                // Collect data and measure price range once
                const m = measurePriceRangeAndCollect(startIndex, endIndex)
                const minPrice = m.minPrice
                const maxPrice = m.maxPrice

                if (minPrice === Number.MAX_VALUE || maxPrice === Number.MIN_VALUE) {
                    if (debugLogs)
                        console.log("No valid price range found")
                    return
                }

                // console.log("Price range:", minPrice, "to", maxPrice)

                let priceRange = maxPrice - minPrice
                if (priceRange === 0) priceRange = 1
                const scaleY = (height - 2 * priceVerticalPadding) / priceRange

                // Draw grid lines
                drawGrid(ctx, width, height)

                // Draw candlesticks from left to right
                let startX = canvasMargin  // Start from left margin
                // console.log("Drawing", barsToShow, "candlesticks with bar width:", effectiveBarWidth)

                for (let i = 0; i < endIndex - startIndex; i++) {
                    let candlestick = m.data[i]

                    if (!candlestick) {
                        if (debugLogs)
                            console.log("Skipping null candlestick at logical index", startIndex + i)
                        continue
                    }

                    let x = startX + i * totalBarWidth
                    const top = priceVerticalPadding
                    const bottom = height - priceVerticalPadding
                    const drawableH = Math.max(1, bottom - top)
                    let openY = top + (maxPrice - candlestick.open) * scaleY
                    let closeY = top + (maxPrice - candlestick.close) * scaleY
                    let highY = top + (maxPrice - candlestick.high) * scaleY
                    let lowY = top + (maxPrice - candlestick.low) * scaleY

                    // Ensure valid coordinates
                    openY = Math.max(top, Math.min(bottom, openY))
                    closeY = Math.max(top, Math.min(bottom, closeY))
                    highY = Math.max(top, Math.min(bottom, highY))
                    lowY = Math.max(top, Math.min(bottom, lowY))

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

                    // Highlight hovered bar background
                    if (hoveredIndex === (startIndex + i)) {
                        ctx.fillStyle = "#333333"
                        ctx.fillRect(x, 0, effectiveBarWidth, height)
                    }

                    // Body
                    ctx.fillStyle = candlestick.close >= candlestick.open ? "#00ff00" : "#ff0000"
                    ctx.fillRect(x, bodyY, effectiveBarWidth, bodyHeight)
                    
                    // Wicks (LOD: skip when bars are very thin)
                    if (effectiveBarWidth >= 2) {
                        ctx.strokeStyle = candlestick.close >= candlestick.open ? "#00cc00" : "#cc0000"
                        ctx.lineWidth = 1
                        ctx.beginPath()
                        const midX = Math.round(x + effectiveBarWidth / 2) + 0.5
                        ctx.moveTo(midX, bodyY)
                        ctx.lineTo(midX, highY)
                        ctx.stroke()

                        ctx.beginPath()
                        ctx.moveTo(midX, bodyY + bodyHeight)
                        ctx.lineTo(midX, lowY)
                        ctx.stroke()
                    }
                }

                // Crosshair and hover feedback (vertical/horizontal lines + tooltip)
                if (hoverVisible && hoveredIndex >= startIndex && hoveredIndex < endIndex) {
                    const i = hoveredIndex - startIndex
                    const x = Math.round(startX + i * totalBarWidth + effectiveBarWidth / 2) + 0.5
                    ctx.strokeStyle = "#808080"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    ctx.moveTo(x, 0)
                    ctx.lineTo(x, height)
                    ctx.stroke()

                    // Horizontal crosshair at hoverY
                    if (hoverY >= 0) {
                        const y = Math.max(0, Math.min(height, hoverY)) + 0.5
                        ctx.beginPath()
                        ctx.moveTo(0, y)
                        ctx.lineTo(width, y)
                        ctx.stroke()
                    }

                    // Tooltip with OHLC/time/volume
                    const c = getCandlestickData(hoveredIndex)
                    if (c) {
                        const dt = new Date(c.timestamp)
                        const lines = [
                            formatTimeLabel(dt, 1),
                            `O:${c.open?.toFixed(2)} H:${c.high?.toFixed(2)} L:${c.low?.toFixed(2)} C:${c.close?.toFixed(2)}`,
                            `V:${(c.volume ?? 0).toLocaleString()}`
                        ]
                        ctx.font = "10px sans-serif"
                        const pad = 6
                        const lineH = 14
                        let boxW = 0
                        for (let t of lines) boxW = Math.max(boxW, Math.min(tooltipMaxWidth, ctx.measureText(t).width))
                        const boxH = lineH * lines.length + pad*2
                        let boxX = 0
                        let boxY = 0
                        if (tooltipMode === "fixed") {
                            boxX = width - (boxW + pad*2) - 6
                            boxY = 6
                        } else {
                            // follow cursor: place tooltip directly at mouse position
                            const rawX = (hoverX >= 0 ? hoverX : x)
                            const rawY = (hoverY >= 0 ? hoverY : 0)
                            boxX = Math.min(Math.max(0, rawX), width - (boxW + pad*2))
                            boxY = Math.min(Math.max(0, rawY), height - boxH)
                        }
                        // Background
                        ctx.fillStyle = "#202020"
                        ctx.strokeStyle = "#555555"
                        ctx.lineWidth = 1
                        ctx.beginPath()
                        ctx.rect(boxX, boxY, boxW + pad*2, boxH)
                        ctx.fill()
                        ctx.stroke()
                        // Text
                        ctx.fillStyle = "#dddddd"
                        for (let k=0; k<lines.length; ++k) {
                            ctx.fillText(lines[k], boxX + pad, boxY + pad + (k+1)*lineH - 4)
                        }
                    }
                }

                // Price axis labels (right side) with a background band for readability
                ctx.font = "10px sans-serif"
                // y-axis background band for readability
                ctx.fillStyle = "#151515"
                ctx.strokeStyle = "#404040"
                ctx.lineWidth = 1
                ctx.beginPath(); ctx.rect(width - 56, 0, 56, height); ctx.fill(); ctx.stroke()
                ctx.fillStyle = "#aaaaaa"
                {
                    const top = priceVerticalPadding
                    const bottom = height - priceVerticalPadding
                    const drawableH = Math.max(0, bottom - top)
                    for (let i = 0; i <= gridHLines; i++) {
                        const y = top + (i / gridHLines) * drawableH
                        const price = maxPrice - (i / gridHLines) * priceRange
                        const label = price.toFixed(2)
                        ctx.fillText(label, width - 50, Math.min(height - 2, Math.max(10, y + 3)))
                    }
                }

                // Note: time axis ticks and labels are rendered by timeAxisCanvas

                // console.log("Canvas painting completed")
            }

            // Mouse/gesture interactions:
            // - Hover to show crosshair and tooltip
            // - Left/Middle drag to pan with soft limits and damping
            // - Mouse wheel zoom around cursor anchor
            // - Right-button rubber-band selection to zoom into a range
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton | Qt.MiddleButton | Qt.RightButton
                property real lastX: -1
                property bool rubberActive: false
                property real rubberStartX: 0
                property real rubberEndX: 0
                onEntered: { hoverVisible = true }
                onExited: { hoverVisible = false; hoveredIndex = -1; hoverX = -1; hoverY = -1; }
                onPressed: function(mouse){
                    lastX = mouse.x
                    if (mouse.button === Qt.RightButton) {
                        rubberActive = true
                        rubberStartX = mouse.x
                        rubberEndX = mouse.x
                    }
                }
                onReleased: function(mouse){
                    if (rubberActive && mouse.button === Qt.RightButton) {
                        rubberActive = false
                        // Perform zoom to selection
                        const x1 = Math.min(rubberStartX, rubberEndX)
                        const x2 = Math.max(rubberStartX, rubberEndX)
                        const vp = computeViewParams(candlestickCanvas.width)
                        const i1 = Math.max(0, Math.floor((x1 - canvasMargin) / vp.totalBarWidth))
                        const i2 = Math.max(i1 + 1, Math.floor((x2 - canvasMargin) / vp.totalBarWidth))
                        const selBars = Math.min(vp.endIndex - vp.startIndex, i2 - i1)
                        if (selBars >= 2) {
                            // Adjust zoom so that selBars fit into viewport
                            const targetZoom = Math.min(5.0, Math.max(0.2, (candlestickCanvas.width - 2*canvasMargin) / (selBars * (barBaseWidth + barSpacing) - barSpacing)))
                            zoomFactor = targetZoom
                            // Align pan so that selection right edge aligns with viewport right edge
                            panOffsetBars = Math.max(0, (candlestickModel ? candlestickModel.count : 0) - (vp.startIndex + i2))
                        }
                        candlestickCanvas.requestPaint(); volumeCanvas.requestPaint(); timeAxisCanvas.requestPaint()
                    }
                    lastX = -1
                }
                onPositionChanged: function(mouse){
                    hoverX = mouse.x; hoverY = mouse.y
                    // Map to index for hover
                    const vp = computeViewParams(candlestickCanvas.width)
                    const i = Math.floor((mouse.x - canvasMargin) / vp.totalBarWidth)
                    const idx = vp.startIndex + i
                    if (i >= 0 && idx >= vp.startIndex && idx < vp.endIndex) {
                        hoveredIndex = idx
                    } else {
                        hoveredIndex = -1
                    }
                    if (rubberActive) {
                        rubberEndX = mouse.x
                    }
                    // Panning with left or middle button
                    if (pressed && lastX >= 0 && (mouse.buttons & (Qt.MiddleButton | Qt.LeftButton))) {
                        const dx = mouse.x - lastX
                        lastX = mouse.x
                        const barsPerPixel = 1.0 / computeViewParams(candlestickCanvas.width).totalBarWidth
                        let desired = panOffsetBars + (-dx * barsPerPixel)
                        // soft clamp with damping
                        const count = candlestickModel ? candlestickModel.count : 0
                        const maxPan = count
                        if (desired < 0) {
                            desired = desired * 0.2
                        } else if (desired > maxPan) {
                            desired = maxPan + (desired - maxPan) * 0.2
                        }
                        if (Math.abs(desired - panOffsetBars) > 0.01) {
                            panOffsetBars = desired
                        }
                        if (barsPerPixel !== 0) {
                            const barsShift = Math.round(dx / barsPerPixel)
                            if (barsShift !== 0) {
                                const count = candlestickModel ? candlestickModel.count : 0
                                panOffsetBars = Math.max(0, Math.min(count, panOffsetBars + barsShift))
                            }
                        }
                    }
                    requestRepaintDebounced()
                }
                onWheel: function(w){
                    // Zoom around mouse.x
                    const delta = w.angleDelta.y
                    const factor = delta > 0 ? 1.1 : 1/1.1
                    const oldZoom = zoomFactor
                    const oldBarsPerPx = 1.0 / computeViewParams(candlestickCanvas.width).totalBarWidth
                    zoomFactor = Math.max(0.2, Math.min(5.0, zoomFactor * factor))
                    // Keep anchor index under cursor stable by adjusting pan
                    const vpNew = computeViewParams(candlestickCanvas.width)
                    const newBarsPerPx = 1.0 / vpNew.totalBarWidth
                    const pxFromRight = candlestickCanvas.width - w.x - canvasMargin
                    const deltaBars = (newBarsPerPx - oldBarsPerPx) * pxFromRight
                    panOffsetBars = panOffsetBars + deltaBars
                    // soft clamp
                    const count = candlestickModel ? candlestickModel.count : 0
                    if (panOffsetBars < 0) panOffsetBars = panOffsetBars * 0.5
                    if (panOffsetBars > count) panOffsetBars = count + (panOffsetBars - count) * 0.5
                    if (zoomFactor !== oldZoom) {
                        requestRepaintDebounced()
                    }
                    w.accepted = true
                }
                onDoubleClicked: {
                    zoomFactor = 1.0
                    panOffsetBars = 0
                    hoveredIndex = -1
                    requestRepaintDebounced()
                }
                Rectangle {
                    // Rubber-band rectangle for right-button selection zoom
                    visible: parent.rubberActive
                    x: Math.min(parent.rubberStartX, parent.rubberEndX)
                    width: Math.abs(parent.rubberEndX - parent.rubberStartX)
                    y: 0
                    height: parent.height
                    color: "#3388ff22"
                    border.color: "#3388ff"
                    border.width: 1
                }
            }

            // Scrollbar synced with horizontal panning
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

        // Time axis (separate canvas, aligned with candlesticks)
        // Dedicated canvas for bottom axis to keep layering simple and crisp
        Canvas {
            id: timeAxisCanvas
            z: 2
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: chartSeparator.top
            height: timeAxisHeight
            anchors.margins: 10
            antialiasing: false

            onPaint: {
                // Compute adaptive tick step by minimum pixel spacing (minTimeTickPx)
                // Render baseline, ticks, labels and subtle day/month dividers
                if (!candlestickModel || candlestickModel.count === 0) return
                const vp = computeViewParams(candlestickCanvas.width)
                const startIndex = vp.startIndex
                const endIndex = vp.endIndex
                const effectiveBarWidth = vp.effectiveBarWidth
                const totalBarWidth = vp.totalBarWidth
                const startX = canvasMargin

                let ctx = getContext("2d")
                ctx.reset()
                // background
                ctx.fillStyle = "#151515"
                ctx.fillRect(0, 0, width, height)
                // baseline
                ctx.strokeStyle = "#404040"
                ctx.beginPath(); ctx.moveTo(0, 0.5); ctx.lineTo(width, 0.5); ctx.stroke()
                ctx.beginPath(); ctx.moveTo(0, height - 0.5); ctx.lineTo(width, height - 0.5); ctx.stroke()
                ctx.textBaseline = "top"

                // adaptive ticks & labels
                const visibleBars = endIndex - startIndex
                const ppb = totalBarWidth   // pixels per bar
                const minBarsPerTick = Math.max(1, Math.ceil(minTimeTickPx / ppb))
                const baseSteps = [1, 2, 3, 5, 10, 15, 20, 30, 60, 120]
                let step = baseSteps[0]
                for (let s of baseSteps) {
                    if (s >= minBarsPerTick) { step = s; break }
                }
                let lastDay = -1
                let lastMonth = -1
                ctx.font = "10px sans-serif"
                ctx.fillStyle = "#aaaaaa"
                for (let idx = startIndex; idx < endIndex; idx += step) {
                    const x = Math.round(startX + (idx - startIndex) * totalBarWidth + effectiveBarWidth / 2) + 0.5
                    const c = getCandlestickData(idx)
                    if (!c || !c.timestamp) continue
                    const dt = new Date(c.timestamp)
                    // tick
                    ctx.strokeStyle = "#505050"
                    ctx.beginPath(); ctx.moveTo(x, 0); ctx.lineTo(x, 6); ctx.stroke()
                    // label
                    const txt = formatTimeLabel(dt, step)
                    ctx.fillText(txt, Math.max(2, x - 30), 2)
                    // day divider (subtle)
                    const day = dt.getDate();
                    if (lastDay !== -1 && day !== lastDay) {
                        ctx.strokeStyle = "#606060"
                        ctx.beginPath(); ctx.moveTo(x, 0); ctx.lineTo(x, height); ctx.stroke()
                    }
                    lastDay = day
                    // month header
                    const month = dt.getMonth()
                    if (lastMonth !== -1 && month !== lastMonth) {
                        ctx.strokeStyle = "#707070"
                        ctx.beginPath(); ctx.moveTo(x, 0); ctx.lineTo(x, 12); ctx.stroke()
                        ctx.fillStyle = "#bbbbbb"
                        ctx.fillText(`${dt.getFullYear()}-${month+1}`, Math.max(2, x - 22), 10)
                        ctx.fillStyle = "#aaaaaa"
                    }
                    lastMonth = month
                }
            }
        }

        // Separator between candlestick and volume
        Rectangle {
            id: chartSeparator
            z: 0
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: volumeArea.top
            height: separatorHeight
            color: "#171717"
            // Top border line
            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                height: 1
                color: "#505050"
            }
            // Center accent band
            Rectangle {
                width: parent.width
                height: 2
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                color: "#2a2a2a"
            }
            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 1
                color: "#303030"
            }
        }

        // Volume chart
        // Draws per-bar volume with the same X alignment as candlesticks
        Rectangle {
            id: volumeArea
            z: 0
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
                anchors.margins: canvasMargin
                antialiasing: false

                onPaint: {
                    // Reuse viewport from the candlestick canvas to ensure alignment
                    // Draw simple rectangles scaled to the maximum visible volume
                    if (!candlestickModel || candlestickModel.count === 0) return

                    let ctx = getContext("2d")
                    ctx.reset()

                    // Shared viewport params
                    const vp = computeViewParams(candlestickCanvas.width)
                    const effectiveBarWidth = vp.effectiveBarWidth
                    const totalBarWidth = vp.totalBarWidth
                    const startIndex = vp.startIndex
                    const endIndex = vp.endIndex

                    const m = measurePriceRangeAndCollect(startIndex, endIndex)
                    let maxVolume = 0
                    for (let i = 0; i < m.data.length; i++) {
                        const c = m.data[i]
                        if (c && typeof c.volume === 'number') maxVolume = Math.max(maxVolume, c.volume)
                    }
                    if (maxVolume === 0) return

                    let startX = canvasMargin

                    for (let i = 0; i < endIndex - startIndex; i++) {
                        let candlestick = m.data[i]
                        if (!candlestick || typeof candlestick.volume !== 'number') continue

                        let x = startX + i * totalBarWidth
                        let volumeHeight = (candlestick.volume / maxVolume) * height
                        let volumeY = height - volumeHeight

                        ctx.fillStyle = candlestick.close >= candlestick.open ? "#00ff00" : "#ff0000"
                        ctx.fillRect(x, volumeY, effectiveBarWidth, volumeHeight)
                    }

                    // Right-side axis for volume values
                    ctx.font = "10px sans-serif"
                    ctx.fillStyle = "#151515"
                    ctx.strokeStyle = "#404040"
                    ctx.lineWidth = 1
                    const axisWidth = 56
                    ctx.beginPath(); ctx.rect(width - axisWidth, 0, axisWidth, height); ctx.fill(); ctx.stroke()
                    ctx.fillStyle = "#aaaaaa"
                    const ticks = 5
                    for (let t = 0; t <= ticks; t++) {
                        const y = height - (t / ticks) * height
                        const v = Math.round((t / ticks) * maxVolume)
                        const label = v.toLocaleString()
                        ctx.fillText(label, width - axisWidth + 6, Math.min(height - 12, Math.max(0, y - 6)))
                    }

                    // Hover tooltip for volume at mouse position
                    if (volumeHoverVisible && volumeHoveredIndex >= startIndex && volumeHoveredIndex < endIndex) {
                        const idx = volumeHoveredIndex
                        const localIndex = idx - startIndex
                        const c = m.data[localIndex]
                        if (c && typeof c.volume === 'number') {
                            const pad = 6
                            const txt = `V: ${c.volume.toLocaleString()}`
                            ctx.font = "10px sans-serif"
                            const tw = Math.min(tooltipMaxWidth, ctx.measureText(txt).width)
                            const th = 16
                            const rawX = Math.max(0, Math.min(width - (tw + pad*2), volumeHoverX))
                            const rawY = Math.max(0, Math.min(height - th, volumeHoverY))
                            ctx.fillStyle = "#202020"
                            ctx.strokeStyle = "#555555"
                            ctx.lineWidth = 1
                            ctx.beginPath(); ctx.rect(rawX, rawY, tw + pad*2, th); ctx.fill(); ctx.stroke()
                            ctx.fillStyle = "#dddddd"
                            ctx.fillText(txt, rawX + pad, rawY + th - 6)
                        }
                    }
                }

                // Mouse interaction for volume hover
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: { volumeHoverVisible = true }
                    onExited: { volumeHoverVisible = false; volumeHoveredIndex = -1; volumeHoverX = -1; volumeHoverY = -1; requestRepaintDebounced() }
                    onPositionChanged: function(mouse) {
                        volumeHoverX = mouse.x; volumeHoverY = mouse.y
                        const vp = computeViewParams(candlestickCanvas.width)
                        const i = Math.floor((mouse.x - canvasMargin) / vp.totalBarWidth)
                        const idx = vp.startIndex + i
                        if (i >= 0 && idx >= vp.startIndex && idx < vp.endIndex) {
                            volumeHoveredIndex = idx
                        } else {
                            volumeHoveredIndex = -1
                        }
                        requestRepaintDebounced()
                    }
                }
            }
        }
    }

    // Canvas update timer
    // If autoRefresh is enabled, triggers periodic repaints (off by default)
    Timer {
        id: canvasUpdateTimer
        interval: 1000  // Update canvas every second
        running: autoRefresh
        repeat: true
        onTriggered: { requestRepaintDebounced() }
    }

    // Function to switch candlestick model for different symbol
    // Acquires a model from model_mgr and requests initial repaint
    function switchCandlestickModel(symbol) {
        if (debugLogs) console.log("Switching candlestick model for symbol:", symbol)
        
        // Get new model from model manager
        candlestickModel = model_mgr.reqCandlestickModel({"symbol": symbol})
        
        // Clear existing data and reload
        if (candlestickModel) {
            if (debugLogs) console.log("Switched to new model, cleared data, waiting for C++ data generation...")
            
            // Force canvas repaint with new model
            candlestickCanvas.requestPaint()
            volumeCanvas.requestPaint()
            timeAxisCanvas.requestPaint()
        } else {
            console.log("Failed to get model for symbol:", symbol)
        }
    }

    // Helper function to safely get candlestick data (QVariantMap)
    // Expects: open/high/low/close/volume/timestamp fields
    function getCandlestickData(index) {
        if (!candlestickModel || index < 0 || index >= candlestickModel.count) {
                if (debugLogs) console.log("Invalid index or model:", index, "count:", candlestickModel ? candlestickModel.count : "null")
            return null
        }
        
        // Use the get method which returns QVariantMap
        let candlestick = candlestickModel.get(index)
        
        // Validate the data
        if (candlestick && typeof candlestick.open === 'number' && typeof candlestick.close === 'number' && 
            typeof candlestick.high === 'number' && typeof candlestick.low === 'number') {
            return candlestick
        }
        
        if (debugLogs) console.log("Invalid data returned for index", index, ":", candlestick)
        return null
    }

    // Initialize data when page loads
    // Binds to the first symbol in config and requests its model
    Component.onCompleted: {
        if (debugLogs) console.log("OrderFlowPage loaded")
        
        // Initialize with first symbol from config
        if (symbolComboBox.count > 0) {
            let initialSymbol = symbolComboBox.currentText
            switchCandlestickModel(initialSymbol)
        }
    }
    
    // Listen for data changes from C++ model
    // Coalesces UI repaints when data is updated or a candlestick is appended
    Connections {
        target: candlestickModel
        
        function onDataChanged() {
            // Verify we're still connected to the current model
            if (candlestickModel && candlestickModel === target) {
                // console.log("Candlestick data changed, count:", candlestickModel.count, "model name:", candlestickModel.get_name())
                requestRepaintDebounced()
            } else {
                if (debugLogs) console.log("Data changed but model connection may be stale")
            }
        }
        
        function onCandlestickAdded() {
            // Verify we're still connected to the current model
            if (candlestickModel && candlestickModel === target) {
                // console.log("New candlestick added, total count:", candlestickModel.count, "model name:", candlestickModel.get_name())
                requestRepaintDebounced()
            } else {
                if (debugLogs) console.log("Candlestick added but model connection may be stale")
            }
        }
    }

    // Listen for config changes
    // Updates symbol list and ensures an initial model is selected
    Connections {
        target: configManager
        
        function onConfigLoaded() {
                if (debugLogs) console.log("Config loaded, updating symbol list")
            // Force ComboBox to update its model
            if (configManager && typeof configManager.getEnabledInstruments === 'function') {
                symbolComboBox.model = configManager.getEnabledInstruments()
                
                // Initialize with first symbol if no model is set yet
                if (!candlestickModel && symbolComboBox.count > 0) {
                    let initialSymbol = symbolComboBox.currentText
                    switchCandlestickModel(initialSymbol)
                }
            } else {
                if (debugLogs) console.log("ConfigManager methods not available")
            }
        }
    }
} 