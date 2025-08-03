import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

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
                            text: "Order Book: " + (orderBookModel ? orderBookModel.count : "0") + " levels"
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

                    ListView {
                        anchors.fill: parent
                        anchors.margins: 10
                        orientation: ListView.Horizontal
                        model: candlestickModel
                        clip: true

                        delegate: Rectangle {
                            width: 20
                            height: parent.height - 20
                            color: "transparent"

                            // K线实体
                            Rectangle {
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.bottom: parent.bottom
                                width: 12
                                height: Math.abs(model.close - model.open) * 2
                                y: model.close > model.open ? 
                                   parent.height - height - (model.close - model.low) * 2 :
                                   parent.height - height - (model.open - model.low) * 2
                                color: model.close > model.open ? "#00ff00" : "#ff0000"
                                opacity: 0.8
                            }

                            // 上影线
                            Rectangle {
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.bottom: parent.bottom
                                width: 1
                                height: (model.high - Math.max(model.open, model.close)) * 2
                                y: parent.height - height - (model.high - model.low) * 2
                                color: "#ffffff"
                                opacity: 0.6
                            }

                            // 下影线
                            Rectangle {
                                anchors.horizontalCenter: parent.horizontalCenter
                                anchors.bottom: parent.bottom
                                width: 1
                                height: (Math.min(model.open, model.close) - model.low) * 2
                                y: parent.height - height
                                color: "#ffffff"
                                opacity: 0.6
                            }
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