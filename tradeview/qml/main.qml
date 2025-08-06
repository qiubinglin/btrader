import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

// Import custom components
import "components"
import "pages"

Window {
    id: mainWindow
    width: 400
    height: 200
    visible: true
    title: "Order Flow Trading System - Launcher"
    color: "#1e1e1e"

    // 窗口列表管理
    property var windowList: []

    // C++ models are registered via context properties, no need to rebind here

    // Menu Bar
    MenuBar {
        id: menuBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 30

        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("Close All Windows")
                onTriggered: closeAllWindows()
            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit()
            }
        }

        Menu {
            title: qsTr("Windows")
            MenuItem {
                text: qsTr("Open Order Flow Window")
                onTriggered: createOrderFlowWindow()
            }
            MenuItem {
                text: qsTr("Open Tick Trade Window")
                onTriggered: createTickTradeWindow()
            }
            MenuItem {
                text: qsTr("Open Order Book Window")
                onTriggered: createOrderBookWindow()
            }
        }

        Menu {
            title: qsTr("Settings")
            MenuItem {
                text: qsTr("Open Settings")
                onTriggered: settingsPageDialog.open()
            }
        }
    }

    // Main content area - simple launcher interface
    Rectangle {
        anchors.top: menuBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "#1e1e1e"
        border.color: "#404040"
        border.width: 1

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 20

            // Title
            Text {
                text: "Order Flow Trading System"
                font.pixelSize: 24
                font.bold: true
                color: "#ffffff"
                Layout.alignment: Qt.AlignHCenter
            }

            // Subtitle
            Text {
                text: "Use the menu bar to open trading windows"
                font.pixelSize: 14
                color: "#cccccc"
                Layout.alignment: Qt.AlignHCenter
            }

            // Window count
            Text {
                text: "Active Windows: " + windowList.length
                font.pixelSize: 12
                color: "#888888"
                Layout.alignment: Qt.AlignHCenter
            }

            // Quick access buttons
            RowLayout {
                spacing: 10
                Layout.alignment: Qt.AlignHCenter

                Button {
                    text: "📈 Order Flow"
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
                    onClicked: createOrderFlowWindow()
                }

                Button {
                    text: "📊 Tick Trade"
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
                    onClicked: createTickTradeWindow()
                }

                Button {
                    text: "📋 Order Book"
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
                    onClicked: createOrderBookWindow()
                }
            }

            // Close all windows button
            Button {
                text: "Close All Windows"
                Layout.alignment: Qt.AlignHCenter
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
                onClicked: closeAllWindows()
            }
        }
    }

    // 创建订单流窗口
    function createOrderFlowWindow() {
        console.log("Creating OrderFlowWindow...")
        var component = Qt.createComponent("windows/OrderFlowWindow.qml")
        if (component.status === Component.Ready) {
            var window = component.createObject(null, {
                "windowTitle": "Order Flow - " + (windowList.length + 1)
            })
            
            if (window) {
                windowList.push(window)
                window.show()
                
                // 监听窗口关闭事件
                window.closing.connect(function() {
                    removeWindow(window)
                })
                
                console.log("OrderFlowWindow created and shown successfully")
            } else {
                console.error("Failed to create OrderFlowWindow object")
            }
        } else {
            console.error("Failed to create OrderFlowWindow component:", component.errorString())
        }
    }

    // 创建逐笔成交窗口
    function createTickTradeWindow() {
        console.log("Creating TickTradeWindow...")
        var component = Qt.createComponent("windows/TickTradeWindow.qml")
        if (component.status === Component.Ready) {
            var window = component.createObject(null, {
                "windowTitle": "Tick Trade - " + (windowList.length + 1)
            })
            
            if (window) {
                windowList.push(window)
                window.show()
                
                // 监听窗口关闭事件
                window.closing.connect(function() {
                    removeWindow(window)
                })
                
                console.log("TickTradeWindow created and shown successfully")
            } else {
                console.error("Failed to create TickTradeWindow object")
            }
        } else {
            console.error("Failed to create TickTradeWindow component:", component.errorString())
        }
    }

    // 创建订单簿窗口
    function createOrderBookWindow() {
        console.log("Creating OrderBookWindow...")
        var component = Qt.createComponent("windows/OrderBookWindow.qml")
        if (component.status === Component.Ready) {
            var window = component.createObject(null, {
                "windowTitle": "Order Book - " + (windowList.length + 1)
            })
            
            if (window) {
                windowList.push(window)
                window.show()
                
                // 监听窗口关闭事件
                window.closing.connect(function() {
                    removeWindow(window)
                })
                
                console.log("OrderBookWindow created and shown successfully")
            } else {
                console.error("Failed to create OrderBookWindow object")
            }
        } else {
            console.error("Failed to create OrderBookWindow component:", component.errorString())
        }
    }

    // 关闭所有窗口
    function closeAllWindows() {
        console.log("Closing all windows...")
        for (var i = 0; i < windowList.length; i++) {
            if (windowList[i]) {
                windowList[i].close()
            }
        }
        windowList = []
        console.log("All windows closed")
    }

    // 从列表中移除窗口
    function removeWindow(window) {
        var index = windowList.indexOf(window)
        if (index > -1) {
            windowList.splice(index, 1)
            console.log("Window removed from list, remaining:", windowList.length)
        }
    }

    // Initialize after page loads
    Component.onCompleted: {
        console.log("Main launcher window loaded")
    }

    // 应用关闭时清理所有窗口
    Component.onDestruction: {
        closeAllWindows()
    }

    // Settings dialog
    Dialog {
        id: settingsPageDialog
        title: "Settings"
        width: 600
        height: 400
        modal: true
        anchors.centerIn: parent
        
        SettingsPage {
            anchors.fill: parent
        }
    }
} 