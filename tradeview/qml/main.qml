import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

// 导入自定义组件
import "components"

Window {
    id: mainWindow
    width: 1400
    height: 900
    visible: true
    title: "Order Flow Trading System"
    color: "#1e1e1e"

    // C++模型通过上下文属性注册，不需要在这里重新绑定

    // 主布局
    RowLayout {
        anchors.fill: parent
        spacing: 0

        // 左侧数据面板
        Rectangle {
            Layout.preferredWidth: 300
            Layout.fillHeight: true
            color: "#1a1a1a"
            border.color: "#404040"
            border.width: 1

            Column {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                // 标题
                Text {
                    text: "Live Market Data"
                    color: "#00ff00"
                    font.pixelSize: 18
                    font.bold: true
                }

                // 数据统计
                Rectangle {
                    width: parent.width - 20
                    height: 120
                    color: "#2d2d2d"
                    radius: 5

                    Column {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 5

                        Text {
                            text: "Candlestick: " + (candlestickModel ? candlestickModel.count : "0") + " bars"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        Text {
                            text: "Tick Trades: " + (tickTradeModel ? tickTradeModel.count : "0") + " trades"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        Text {
                            text: "Order Book: " + (orderBookModel ? orderBookModel.rowCount() : "0") + " levels"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        Text {
                            text: "Latest Price: " + (tickTradeModel ? tickTradeModel.latestPrice + " USDT" : "N/A")
                            color: "#ffff00"
                            font.pixelSize: 12
                        }

                        Text {
                            text: "Total Volume: " + (tickTradeModel ? tickTradeModel.totalVolume : "N/A")
                            color: "#ffff00"
                            font.pixelSize: 12
                        }
                    }
                }

                // 最新逐笔成交
                Text {
                    text: "Latest Tick Trades"
                    color: "#00ff00"
                    font.pixelSize: 14
                    font.bold: true
                }

                Rectangle {
                    width: parent.width - 20
                    height: 200
                    color: "#2d2d2d"
                    radius: 5

                    ListView {
                        anchors.fill: parent
                        anchors.margins: 5
                        model: tickTradeModel
                        clip: true

                        delegate: Rectangle {
                            width: parent ? parent.width : 0
                            height: 30
                            color: "transparent"

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 5
                                spacing: 10

                                Text {
                                    text: model.timestamp ? new Date(model.timestamp).toLocaleTimeString() : ""
                                    color: "#888888"
                                    font.pixelSize: 10
                                    Layout.preferredWidth: 60
                                }

                                Text {
                                    text: model.price ? model.price.toFixed(2) : ""
                                    color: model.direction === "buy" ? "#00ff00" : "#ff0000"
                                    font.pixelSize: 10
                                    Layout.preferredWidth: 60
                                }

                                Text {
                                    text: model.volume ? model.volume : ""
                                    color: "#ffffff"
                                    font.pixelSize: 10
                                    Layout.preferredWidth: 40
                                }

                                Text {
                                    text: model.direction ? model.direction.toUpperCase() : ""
                                    color: model.direction === "buy" ? "#00ff00" : "#ff0000"
                                    font.pixelSize: 10
                                    Layout.preferredWidth: 40
                                }
                            }
                        }
                    }
                }
            }
        }

        // 右侧主图表区域
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#1e1e1e"
            border.color: "#404040"
            border.width: 1

            Column {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                // 标题
                Text {
                    text: "BTC/USDT - Live Chart"
                    color: "#ffffff"
                    font.pixelSize: 16
                    font.bold: true
                }

                // K线图表区域
                Rectangle {
                    width: parent.width - 20
                    height: 300
                    color: "#2d2d2d"
                    radius: 5

                    // 标题栏
                    RowLayout {
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.margins: 10
                        height: 30

                        Text {
                            text: "BTC/USDT - Live Candlestick Chart"
                            color: "#ffffff"
                            font.pixelSize: 14
                            font.bold: true
                            Layout.fillWidth: true
                        }

                        Text {
                            text: "Price: " + (candlestickModel && candlestickModel.count > 0 ? 
                                candlestickModel.get(candlestickModel.count - 1).close.toFixed(2) : "0.00")
                            color: "#00ff00"
                            font.pixelSize: 12
                            font.bold: true
                        }

                        Text {
                            text: "Change: " + (candlestickModel && candlestickModel.count > 1 ? 
                                ((candlestickModel.get(candlestickModel.count - 1).close - 
                                 candlestickModel.get(candlestickModel.count - 2).close).toFixed(2)) : "0.00")
                            color: "#00ff00"
                            font.pixelSize: 12
                        }
                    }

                    // K线图表
                    Canvas {
                        id: candlestickCanvas
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        anchors.margins: 40
                        antialiasing: true

                        onPaint: {
                            if (!candlestickModel || candlestickModel.count === 0) return

                            let ctx = getContext("2d")
                            ctx.reset()

                            // 计算价格范围
                            let minPrice = Number.MAX_VALUE
                            let maxPrice = Number.MIN_VALUE
                            let barsToShow = Math.min(candlestickModel.count, 50)
                            
                            for (let i = 0; i < barsToShow; i++) {
                                let candlestick = candlestickModel.get(i)
                                if (candlestick) {
                                    minPrice = Math.min(minPrice, candlestick.low)
                                    maxPrice = Math.max(maxPrice, candlestick.high)
                                }
                            }

                            if (minPrice === Number.MAX_VALUE || maxPrice === Number.MIN_VALUE) return

                            let priceRange = maxPrice - minPrice
                            if (priceRange === 0) priceRange = 1

                            // 绘制网格线
                            ctx.strokeStyle = "#404040"
                            ctx.lineWidth = 1
                            
                            // 水平网格线
                            for (let i = 0; i <= 5; i++) {
                                let y = (i / 5) * height
                                ctx.beginPath()
                                ctx.moveTo(0, y)
                                ctx.lineTo(width, y)
                                ctx.stroke()
                            }

                            // 绘制K线
                            let barWidth = 6
                            let barSpacing = 1
                            let totalWidth = barsToShow * (barWidth + barSpacing)
                            let startX = (width - totalWidth) / 2

                            for (let i = 0; i < barsToShow; i++) {
                                let candlestick = candlestickModel.get(i)
                                if (!candlestick) continue

                                let x = startX + i * (barWidth + barSpacing)
                                let openY = height - ((candlestick.open - minPrice) / priceRange) * height
                                let closeY = height - ((candlestick.close - minPrice) / priceRange) * height
                                let highY = height - ((candlestick.high - minPrice) / priceRange) * height
                                let lowY = height - ((candlestick.low - minPrice) / priceRange) * height

                                // 绘制影线
                                ctx.strokeStyle = candlestick.close >= candlestick.open ? "#00ff00" : "#ff0000"
                                ctx.lineWidth = 1
                                ctx.beginPath()
                                ctx.moveTo(x + barWidth/2, highY)
                                ctx.lineTo(x + barWidth/2, lowY)
                                ctx.stroke()

                                // 绘制K线实体
                                let bodyHeight = Math.abs(closeY - openY)
                                if (bodyHeight < 1) bodyHeight = 1

                                let bodyY = candlestick.close >= candlestick.open ? closeY : openY
                                
                                if (candlestick.close >= candlestick.open) {
                                    ctx.fillStyle = "#00ff00"
                                } else {
                                    ctx.fillStyle = "#ff0000"
                                }

                                ctx.fillRect(x, bodyY, barWidth, bodyHeight)
                                
                                // 绘制边框
                                ctx.strokeStyle = candlestick.close >= candlestick.open ? "#00cc00" : "#cc0000"
                                ctx.lineWidth = 1
                                ctx.strokeRect(x, bodyY, barWidth, bodyHeight)
                            }
                        }
                    }

                    // 价格标签
                    Column {
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        anchors.margins: 50
                        spacing: 0

                        Repeater {
                            model: 6
                            Text {
                                text: {
                                    if (!candlestickModel || candlestickModel.count === 0) return "0.00"
                                    let minPrice = Number.MAX_VALUE
                                    let maxPrice = Number.MIN_VALUE
                                    let barsToShow = Math.min(candlestickModel.count, 50)
                                    
                                    for (let i = 0; i < barsToShow; i++) {
                                        let candlestick = candlestickModel.get(i)
                                        if (candlestick) {
                                            minPrice = Math.min(minPrice, candlestick.low)
                                            maxPrice = Math.max(maxPrice, candlestick.high)
                                        }
                                    }
                                    
                                    if (minPrice === Number.MAX_VALUE || maxPrice === Number.MIN_VALUE) return "0.00"
                                    let priceRange = maxPrice - minPrice
                                    return (maxPrice - (index / 5) * priceRange).toFixed(2)
                                }
                                color: "#888888"
                                font.pixelSize: 10
                                height: parent.height / 6
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }

                    // 定时器更新
                    Timer {
                        interval: 1000
                        running: true
                        repeat: true
                        onTriggered: {
                            candlestickCanvas.requestPaint()
                        }
                    }
                }

                // 订单簿区域
                Text {
                    text: "Order Book"
                    color: "#ffffff"
                    font.pixelSize: 14
                    font.bold: true
                }

                Rectangle {
                    width: parent.width - 20
                    height: 200
                    color: "#2d2d2d"
                    radius: 5

                    Row {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 0

                        // 买单
                        Column {
                            width: parent.width / 2
                            height: parent.height
                            spacing: 2

                            Text {
                                text: "BIDS"
                                color: "#00ff00"
                                font.pixelSize: 12
                                font.bold: true
                                anchors.horizontalCenter: parent.horizontalCenter
                            }

                            ListView {
                                width: parent.width
                                height: parent.height - 20
                                model: orderBookModel
                                clip: true

                                delegate: Rectangle {
                                    width: parent ? parent.width : 0
                                    height: 20
                                    color: model.side === "bid" ? "transparent" : "transparent"
                                    visible: model.side === "bid"

                                    RowLayout {
                                        anchors.fill: parent
                                        anchors.margins: 2
                                        spacing: 10

                                        Text {
                                            text: model.price ? model.price.toFixed(2) : ""
                                            color: "#00ff00"
                                            font.pixelSize: 10
                                            Layout.preferredWidth: 60
                                        }

                                        Text {
                                            text: model.volume ? model.volume : ""
                                            color: "#ffffff"
                                            font.pixelSize: 10
                                            Layout.preferredWidth: 60
                                        }
                                    }
                                }
                            }
                        }

                        // 卖单
                        Column {
                            width: parent.width / 2
                            height: parent.height
                            spacing: 2

                            Text {
                                text: "ASKS"
                                color: "#ff0000"
                                font.pixelSize: 12
                                font.bold: true
                                anchors.horizontalCenter: parent.horizontalCenter
                            }

                            ListView {
                                width: parent.width
                                height: parent.height - 20
                                model: orderBookModel
                                clip: true

                                delegate: Rectangle {
                                    width: parent ? parent.width : 0
                                    height: 20
                                    color: model.side === "ask" ? "transparent" : "transparent"
                                    visible: model.side === "ask"

                                    RowLayout {
                                        anchors.fill: parent
                                        anchors.margins: 2
                                        spacing: 10

                                        Text {
                                            text: model.price ? model.price.toFixed(2) : ""
                                            color: "#ff0000"
                                            font.pixelSize: 10
                                            Layout.preferredWidth: 60
                                        }

                                        Text {
                                            text: model.volume ? model.volume : ""
                                            color: "#ffffff"
                                            font.pixelSize: 10
                                            Layout.preferredWidth: 60
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
} 