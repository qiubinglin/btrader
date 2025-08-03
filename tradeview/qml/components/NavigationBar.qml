import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: navigationBar
    width: 200
    color: "#2d2d2d"
    border.color: "#404040"
    border.width: 1

    // 属性
    property string currentPage: "CandlestickPage"
    property var pageModel: [
        { name: "K线图表", page: "pages/CandlestickPage.qml", icon: "📈" },
        { name: "逐笔成交", page: "pages/TickTradePage.qml", icon: "📊" },
        { name: "买卖档位", page: "pages/OrderBookPage.qml", icon: "📋" },
        { name: "足迹图", page: "pages/FootprintPage.qml", icon: "👣" },
        { name: "微盘口", page: "pages/MicroOrderBookPage.qml", icon: "🔍" }
    ]

    // 信号
    signal pageChanged(string page)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 5

        // 标题
        Text {
            text: "交易视图"
            font.pixelSize: 18
            font.bold: true
            color: "#ffffff"
            Layout.alignment: Qt.AlignHCenter
        }

        // 分隔线
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#404040"
        }

        // 导航按钮
        Repeater {
            model: navigationBar.pageModel

            Button {
                text: modelData.icon + " " + modelData.name
                Layout.fillWidth: true
                height: 40
                background: Rectangle {
                    color: parent.pressed ? "#404040" : 
                           parent.hovered ? "#353535" : 
                           navigationBar.currentPage === modelData.page ? "#404040" : "#2d2d2d"
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
                    navigationBar.currentPage = modelData.page
                    pageChanged(modelData.page)
                }
            }
        }

        // 分隔线
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#404040"
        }

        // 设置按钮
        Button {
            text: "⚙️ 设置"
            Layout.fillWidth: true
            height: 40
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
                navigationBar.currentPage = "pages/SettingsPage.qml"
                pageChanged("pages/SettingsPage.qml")
            }
        }

        Item { Layout.fillHeight: true }
    }
} 