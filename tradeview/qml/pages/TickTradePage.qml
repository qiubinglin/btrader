import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: tickTradePage
    background: Rectangle {
        color: "#1e1e1e"
    }

    // 工具栏
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

            // 交易对选择
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

            // 最大记录数设置
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
                    return "最大记录: " + value
                }
                valueFromText: function(text) {
                    return parseInt(text.replace("最大记录: ", ""))
                }
                onValueChanged: {
                    tickTradeModel.setMaxCount(value)
                }
            }

            // 清空按钮
            Button {
                text: "清空"
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

            // 自动刷新开关
            CheckBox {
                id: autoRefreshCheckBox
                text: "自动刷新"
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

            // 统计信息
            Text {
                text: "最新价格: " + (tickTradeModel.latestPrice || "0.00")
                color: "#ffffff"
                font.pixelSize: 12
            }

            Text {
                text: "总成交量: " + (tickTradeModel.totalVolume || "0")
                color: "#ffffff"
                font.pixelSize: 12
            }

            Text {
                text: "记录数: " + tickTradeModel.count
                color: "#ffffff"
                font.pixelSize: 12
            }
        }
    }

    // 逐笔成交列表
    Rectangle {
        id: tickTradeList
        anchors.top: toolbar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        color: "#1e1e1e"
        border.color: "#404040"
        border.width: 1

        // 列表标题
        Rectangle {
            id: listHeader
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            height: 40
            color: "#2d2d2d"
            border.color: "#404040"
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10
                spacing: 10

                Text {
                    text: "时间"
                    color: "#ffffff"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.preferredWidth: 120
                }

                Text {
                    text: "价格"
                    color: "#ffffff"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.preferredWidth: 100
                }

                Text {
                    text: "数量"
                    color: "#ffffff"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.preferredWidth: 100
                }

                Text {
                    text: "方向"
                    color: "#ffffff"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.preferredWidth: 80
                }

                Text {
                    text: "成交额"
                    color: "#ffffff"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.preferredWidth: 120
                }

                Text {
                    text: "成交ID"
                    color: "#ffffff"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.fillWidth: true
                }
            }
        }

        // 成交数据列表
        ListView {
            id: tickTradeListView
            anchors.top: listHeader.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            model: tickTradeModel
            clip: true
            spacing: 1

            delegate: Rectangle {
                width: tickTradeListView.width
                height: 35
                color: index % 2 === 0 ? "#1e1e1e" : "#252525"
                border.color: "#404040"
                border.width: 0.5

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                    // 时间
                    Text {
                        text: new Date(model.timestamp).toLocaleTimeString()
                        color: "#cccccc"
                        font.pixelSize: 11
                        Layout.preferredWidth: 120
                    }

                    // 价格
                    Text {
                        text: model.price.toFixed(2)
                        color: model.isBuy ? "#00ff00" : "#ff0000"
                        font.pixelSize: 11
                        font.bold: true
                        Layout.preferredWidth: 100
                    }

                    // 数量
                    Text {
                        text: model.volume.toLocaleString()
                        color: "#ffffff"
                        font.pixelSize: 11
                        Layout.preferredWidth: 100
                    }

                    // 方向
                    Rectangle {
                        width: 60
                        height: 20
                        radius: 10
                        color: model.isBuy ? "#00ff00" : "#ff0000"
                        opacity: 0.8

                        Text {
                            anchors.centerIn: parent
                            text: model.isBuy ? "买入" : "卖出"
                            color: "#ffffff"
                            font.pixelSize: 10
                            font.bold: true
                        }
                    }

                    // 成交额
                    Text {
                        text: (model.price * model.volume).toFixed(2)
                        color: "#ffffff"
                        font.pixelSize: 11
                        Layout.preferredWidth: 120
                    }

                    // 成交ID
                    Text {
                        text: model.tradeId || "N/A"
                        color: "#cccccc"
                        font.pixelSize: 11
                        Layout.fillWidth: true
                    }
                }

                // 鼠标悬停效果
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

            // 滚动条
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

    // 自动刷新定时器
    Timer {
        id: autoRefreshTimer
        interval: 1000  // 1秒刷新一次
        running: autoRefreshCheckBox.checked
        repeat: true
        onTriggered: {
            if (autoRefreshCheckBox.checked) {
                addRandomTickTrade()
            }
        }
    }

    // 添加随机成交数据的函数
    function addRandomTickTrade() {
        const basePrice = 50000 + Math.random() * 1000
        const price = basePrice + (Math.random() - 0.5) * 100
        const volume = Math.floor(Math.random() * 100) + 1
        const direction = Math.random() > 0.5 ? "buy" : "sell"
        const tradeId = "T" + Date.now() + Math.floor(Math.random() * 1000)

        const tickTradeData = {
            timestamp: new Date(),
            price: price,
            volume: volume,
            direction: direction,
            tradeId: tradeId,
            symbol: symbolComboBox.currentText
        }

        tickTradeModel.addTickTrade(tickTradeData)
    }

    // 加载初始数据的函数
    function loadTickTradeData() {
        // 添加一些初始数据
        for (let i = 0; i < 50; i++) {
            addRandomTickTrade()
        }
    }

    // 页面加载时初始化数据
    Component.onCompleted: {
        loadTickTradeData()
    }
} 