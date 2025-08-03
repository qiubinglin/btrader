import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Shapes 1.15

Page {
    id: candlestickPage
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
                    // 切换交易对时重新加载数据
                    candlestickModel.clear()
                    loadCandlestickData()
                }
            }

            // 时间周期选择
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
                onClicked: loadCandlestickData()
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
                text: "最高: " + (candlestickModel.count > 0 ? candlestickModel.get_candlestick(candlestickModel.count - 1).high.toFixed(2) : "0.00")
                color: "#00ff00"
                font.pixelSize: 12
            }

            Text {
                text: "最低: " + (candlestickModel.count > 0 ? candlestickModel.get_candlestick(candlestickModel.count - 1).low.toFixed(2) : "0.00")
                color: "#ff0000"
                font.pixelSize: 12
            }

            Text {
                text: "成交量: " + (candlestickModel.count > 0 ? candlestickModel.get_candlestick(candlestickModel.count - 1).volume : "0")
                color: "#ffffff"
                font.pixelSize: 12
            }
        }
    }

    // K线图表区域
    Rectangle {
        id: chartArea
        anchors.top: toolbar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        color: "#1e1e1e"
        border.color: "#404040"
        border.width: 1

        // 图表标题
        Text {
            id: chartTitle
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 10
            text: symbolComboBox.currentText + " - " + timeframeComboBox.currentText + " K线图"
            color: "#ffffff"
            font.pixelSize: 16
            font.bold: true
        }

        // K线图表
        ListView {
            id: candlestickListView
            anchors.top: chartTitle.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10
            orientation: ListView.Horizontal
            model: candlestickModel
            clip: true

            delegate: Rectangle {
                width: 20
                height: candlestickListView.height - 20
                color: "transparent"

                // K线实体
                Rectangle {
                    id: body
                    anchors.centerIn: parent
                    width: 12
                    height: Math.max(2, Math.abs(model.close - model.open) * 100)
                    color: model.isUp ? "#00ff00" : "#ff0000"
                    border.color: model.isUp ? "#00cc00" : "#cc0000"
                    border.width: 1
                }

                // 上影线
                Rectangle {
                    anchors.bottom: body.top
                    anchors.horizontalCenter: body.horizontalCenter
                    width: 2
                    height: Math.max(0, (model.high - Math.max(model.open, model.close)) * 100)
                    color: "#ffffff"
                }

                // 下影线
                Rectangle {
                    anchors.top: body.bottom
                    anchors.horizontalCenter: body.horizontalCenter
                    width: 2
                    height: Math.max(0, (Math.min(model.open, model.close) - model.low) * 100)
                    color: "#ffffff"
                }

                // 价格标签
                Text {
                    anchors.top: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: model.close.toFixed(2)
                    color: model.isUp ? "#00ff00" : "#ff0000"
                    font.pixelSize: 10
                    visible: mouseArea.containsMouse
                }

                // 鼠标悬停效果
                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: {
                        body.scale = 1.2
                    }
                    onExited: {
                        body.scale = 1.0
                    }
                }

                // 动画效果
                Behavior on scale {
                    NumberAnimation { duration: 100 }
                }
            }

            // 滚动条
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

        // 成交量图表
        Rectangle {
            id: volumeChart
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: 80
            color: "transparent"
            border.color: "#404040"
            border.width: 1

            ListView {
                anchors.fill: parent
                anchors.margins: 5
                orientation: ListView.Horizontal
                model: candlestickModel
                clip: true

                delegate: Rectangle {
                    width: 20
                    height: volumeChart.height - 10
                    color: "transparent"

                    Rectangle {
                        anchors.bottom: parent.bottom
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 12
                        height: Math.max(2, model.volume / 1000)
                        color: model.isUp ? "#00ff00" : "#ff0000"
                        opacity: 0.7
                    }
                }
            }
        }
    }

    // 自动刷新定时器
    Timer {
        id: autoRefreshTimer
        interval: 5000  // 5秒刷新一次
        running: autoRefreshCheckBox.checked
        repeat: true
        onTriggered: {
            if (autoRefreshCheckBox.checked) {
                loadCandlestickData()
            }
        }
    }

    // 加载K线数据的函数
    function loadCandlestickData() {
        // 这里应该调用C++后端API获取数据
        // 目前使用模拟数据
        for (let i = 0; i < 100; i++) {
            const basePrice = 50000 + Math.random() * 1000
            const open = basePrice
            const close = basePrice + (Math.random() - 0.5) * 100
            const high = Math.max(open, close) + Math.random() * 50
            const low = Math.min(open, close) - Math.random() * 50
            const volume = Math.floor(Math.random() * 1000) + 100

            // 暂时注释掉数据添加，避免类型转换错误
            // candlestickModel.add_candlestick(candlestickData)
        }
    }

    // 页面加载时初始化数据
    Component.onCompleted: {
        loadCandlestickData()
    }
} 