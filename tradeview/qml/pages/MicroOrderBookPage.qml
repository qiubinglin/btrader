import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: microOrderBookPage
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
                    microOrderBookModel.clear()
                    loadMicroOrderBookData()
                }
            }

            // 档位数量设置
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
                    return "档位数: " + value
                }
                valueFromText: function(text) {
                    return parseInt(text.replace("档位数: ", ""))
                }
                onValueChanged: {
                    microOrderBookModel.setMaxLevels(value)
                }
            }

            // 高亮阈值设置
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
                    return "高亮阈值: " + value + "%"
                }
                valueFromText: function(text) {
                    return parseInt(text.replace("高亮阈值: ", "").replace("%", ""))
                }
                onValueChanged: {
                    microOrderBookModel.setHighlightThreshold(value / 100)
                }
            }

            // 成交量阈值设置
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
                    return "成交量阈值: " + value
                }
                valueFromText: function(text) {
                    return parseInt(text.replace("成交量阈值: ", ""))
                }
                onValueChanged: {
                    microOrderBookModel.setVolumeThreshold(value)
                }
            }

            // 刷新按钮
            Button {
                text: "刷新"
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
                text: "最佳买价: " + (microOrderBookModel.bestBid || "0.00")
                color: "#00ff00"
                font.pixelSize: 12
            }

            Text {
                text: "最佳卖价: " + (microOrderBookModel.bestAsk || "0.00")
                color: "#ff0000"
                font.pixelSize: 12
            }

            Text {
                text: "价差: " + (microOrderBookModel.spread || "0.00")
                color: "#ffffff"
                font.pixelSize: 12
            }
        }
    }

    // 微盘口显示区域
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

        // 标题
        Text {
            id: microOrderBookTitle
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 10
            text: symbolComboBox.currentText + " 微盘口"
            color: "#ffffff"
            font.pixelSize: 16
            font.bold: true
        }

        // 列标题
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
                    text: "订单数"
                    color: "#ffffff"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.preferredWidth: 80
                }

                Text {
                    text: "百分比"
                    color: "#ffffff"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.preferredWidth: 80
                }

                Text {
                    text: "累计"
                    color: "#ffffff"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.preferredWidth: 100
                }

                Text {
                    text: "状态"
                    color: "#ffffff"
                    font.pixelSize: 12
                    font.bold: true
                    Layout.fillWidth: true
                }
            }
        }

        // 微盘口列表
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
                height: 35
                color: model.isHighlighted ? "#353535" : (index % 2 === 0 ? "#1e1e1e" : "#252525")
                border.color: model.isHighlighted ? "#00ff00" : "#404040"
                border.width: model.isHighlighted ? 2 : 0.5

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 10

                    // 价格
                    Text {
                        text: model.price.toFixed(2)
                        color: model.isBid ? "#00ff00" : "#ff0000"
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

                    // 订单数
                    Text {
                        text: model.orderCount
                        color: "#cccccc"
                        font.pixelSize: 11
                        Layout.preferredWidth: 80
                    }

                    // 百分比
                    Text {
                        text: model.percentage.toFixed(1) + "%"
                        color: "#ffffff"
                        font.pixelSize: 11
                        Layout.preferredWidth: 80
                    }

                    // 累计数量
                    Text {
                        text: model.cumulativeVolume.toLocaleString()
                        color: "#ffffff"
                        font.pixelSize: 11
                        Layout.preferredWidth: 100
                    }

                    // 状态指示器
                    Rectangle {
                        Layout.fillWidth: true
                        height: 20
                        color: "transparent"

                        Rectangle {
                            anchors.left: parent.left
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            width: parent.width * (model.percentage / 100)
                            color: model.isBid ? "#00ff00" : "#ff0000"
                            opacity: 0.3
                            radius: 2
                        }

                        Text {
                            anchors.centerIn: parent
                            text: model.isHighlighted ? "★" : ""
                            color: "#ffff00"
                            font.pixelSize: 12
                            font.bold: true
                        }
                    }
                }

                // 鼠标悬停效果
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        if (!model.isHighlighted) {
                            parent.color = "#353535"
                        }
                    }
                    onExited: {
                        if (!model.isHighlighted) {
                            parent.color = index % 2 === 0 ? "#1e1e1e" : "#252525"
                        }
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
        interval: 2000  // 2秒刷新一次
        running: autoRefreshCheckBox.checked
        repeat: true
        onTriggered: {
            if (autoRefreshCheckBox.checked) {
                updateMicroOrderBookData()
            }
        }
    }

    // 更新微盘口数据的函数
    function updateMicroOrderBookData() {
        const basePrice = 50000 + Math.random() * 1000
        const bids = []
        const asks = []

        // 生成买单数据
        for (let i = 0; i < maxLevelsSpinBox.value; i++) {
            const price = basePrice - (i + 1) * 10
            const volume = Math.floor(Math.random() * 1000) + 100
            const orderCount = Math.floor(Math.random() * 50) + 1

            bids.push({
                price: price,
                volume: volume,
                orderCount: orderCount,
                side: "bid"
            })
        }

        // 生成卖单数据
        for (let i = 0; i < maxLevelsSpinBox.value; i++) {
            const price = basePrice + (i + 1) * 10
            const volume = Math.floor(Math.random() * 1000) + 100
            const orderCount = Math.floor(Math.random() * 50) + 1

            asks.push({
                price: price,
                volume: volume,
                orderCount: orderCount,
                side: "ask"
            })
        }

        microOrderBookModel.updateMicroOrderBook(bids, asks)
    }

    // 加载初始数据的函数
    function loadMicroOrderBookData() {
        updateMicroOrderBookData()
    }

    // 页面加载时初始化数据
    Component.onCompleted: {
        loadMicroOrderBookData()
    }
} 