import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: dataPanel
    width: 300
    color: "#1a1a1a"
    border.color: "#404040"
    border.width: 1

    Column {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        // Title
        Text {
            text: "Live Market Data"
            color: "#00ff00"
            font.pixelSize: 18
            font.bold: true
        }

        // Data statistics
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

        // Latest tick trades
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