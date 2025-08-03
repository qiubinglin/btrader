import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: statusBar
    height: 30
    color: "#2d2d2d"
    border.color: "#404040"
    border.width: 1

    // 属性
    property bool isConnected: true
    property string latestPrice: "0.00"
    property string totalVolume: "0"
    property string currentTime: ""

    RowLayout {
        anchors.fill: parent
        anchors.margins: 5
        spacing: 10

        // 连接状态
        RowLayout {
            spacing: 5
            Rectangle {
                width: 8
                height: 8
                radius: 4
                color: statusBar.isConnected ? "#00ff00" : "#ff0000"
            }
            Text {
                text: "状态: " + (statusBar.isConnected ? "已连接" : "未连接")
                color: statusBar.isConnected ? "#00ff00" : "#ff0000"
                font.pixelSize: 12
            }
        }

        // 最新价格
        Text {
            text: "最新价格: " + statusBar.latestPrice
            color: "#ffffff"
            font.pixelSize: 12
        }

        // 成交量
        Text {
            text: "成交量: " + statusBar.totalVolume
            color: "#ffffff"
            font.pixelSize: 12
        }

        Item { Layout.fillWidth: true }

        // 当前时间
        Text {
            text: statusBar.currentTime
            color: "#cccccc"
            font.pixelSize: 12
        }
    }

    // 定时器更新时间
    Timer {
        interval: 1000
        running: true
        repeat: true
        onTriggered: {
            statusBar.currentTime = new Date().toLocaleTimeString()
        }
    }
} 