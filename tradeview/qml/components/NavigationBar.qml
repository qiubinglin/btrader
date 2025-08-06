import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: navigationBar
    width: 200
    color: "#2d2d2d"
    border.color: "#404040"
    border.width: 1

    // Properties
    property string currentPage: "OrderFlowPage"
    property var pageModel: [
        { name: "Candlestick Chart", page: "pages/OrderFlowPage.qml", icon: "📈", hasWindow: false },
        { name: "Tick Trades", page: "pages/TickTradePage.qml", icon: "📊", hasWindow: true, windowPath: "windows/TickTradeWindow.qml" },
        { name: "Order Book", page: "pages/OrderBookPage.qml", icon: "📋", hasWindow: true, windowPath: "windows/OrderBookWindow.qml" },
        { name: "Footprint", page: "pages/FootprintPage.qml", icon: "👣", hasWindow: false },
        { name: "Micro Order Book", page: "pages/MicroOrderBookPage.qml", icon: "🔍", hasWindow: false }
    ]

    // Signals
    signal pageChanged(string page)
    signal openWindow(string windowPath)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 5

        // Title
        Text {
            text: "Trading View"
            font.pixelSize: 18
            font.bold: true
            color: "#ffffff"
            Layout.alignment: Qt.AlignHCenter
        }

        // Separator
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#404040"
        }

        // Navigation buttons
        Repeater {
            model: navigationBar.pageModel

            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                // Main navigation button
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

                // Window button (only for pages that have windows)
                Button {
                    visible: modelData.hasWindow
                    text: "🪟 Open Window"
                    Layout.fillWidth: true
                    height: 25
                    background: Rectangle {
                        color: parent.pressed ? "#404040" : 
                               parent.hovered ? "#353535" : "#2d2d2d"
                        border.color: "#404040"
                        border.width: 1
                        radius: 2
                    }
                    contentItem: Text {
                        text: parent.text
                        color: "#cccccc"
                        font.pixelSize: 10
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    onClicked: {
                        openWindow(modelData.windowPath)
                    }
                }
            }
        }

        // Separator
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#404040"
        }

        // Settings button
        Button {
            text: "⚙️ Settings"
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