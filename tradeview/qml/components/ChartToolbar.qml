import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: chartToolbar
    height: 50
    color: "#2d2d2d"
    border.color: "#404040"
    border.width: 1

    // 属性
    property var symbolModel: ["BTC/USDT", "ETH/USDT", "BNB/USDT", "ADA/USDT"]
    property var timeframeModel: ["1m", "5m", "15m", "1h", "4h", "1d"]
    property string currentSymbol: "BTC/USDT"
    property string currentTimeframe: "1m"
    property bool autoRefresh: true
    property int refreshInterval: 5000

    // 信号
    signal symbolChanged(string symbol)
    signal timeframeChanged(string timeframe)
    signal refreshClicked()
    signal autoRefreshChanged(bool enabled)

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        // 交易对选择
        ComboBox {
            id: symbolComboBox
            model: chartToolbar.symbolModel
            currentIndex: model.indexOf(chartToolbar.currentSymbol)
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
                chartToolbar.currentSymbol = currentText
                symbolChanged(currentText)
            }
        }

        // 时间周期选择
        ComboBox {
            id: timeframeComboBox
            model: chartToolbar.timeframeModel
            currentIndex: model.indexOf(chartToolbar.currentTimeframe)
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
                chartToolbar.currentTimeframe = currentText
                timeframeChanged(currentText)
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
            onClicked: refreshClicked()
        }

        // 自动刷新开关
        CheckBox {
            id: autoRefreshCheckBox
            text: "自动刷新"
            checked: chartToolbar.autoRefresh
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
            onCheckedChanged: {
                chartToolbar.autoRefresh = checked
                autoRefreshChanged(checked)
            }
        }

        Item { Layout.fillWidth: true }
    }
} 