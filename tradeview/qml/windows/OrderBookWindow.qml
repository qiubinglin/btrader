import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

// Import pages
import "../pages"

Window {
    id: orderBookWindow
    width: 1400
    height: 900
    visible: true
    title: "Order Book - Order Flow Trading System"
    color: "#1e1e1e"
    
    // Window properties
    minimumWidth: 800
    minimumHeight: 600
    
    // Data models - will be passed to the page
    property var orderBookModel: null
    
    // Current symbol
    property string currentSymbol: "BTC/USDT"

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
                text: qsTr("Close Window")
                onTriggered: orderBookWindow.close()
            }
            MenuItem {
                text: qsTr("Exit Application")
                onTriggered: Qt.quit()
            }
        }

        Menu {
            title: qsTr("View")
            MenuItem {
                text: qsTr("Refresh Data")
                onTriggered: {
                    if (orderBookPage && typeof orderBookPage.loadOrderBookData === 'function') {
                        orderBookPage.loadOrderBookData()
                    }
                }
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

    // Main content area
    Rectangle {
        anchors.top: menuBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "#1e1e1e"
        border.color: "#404040"
        border.width: 1

        // Embed OrderBookPage
        OrderBookPage {
            id: orderBookPage
            anchors.fill: parent
            
            // Pass properties to the page
            symbol: currentSymbol
            orderBookModel: orderBookModel
        }
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

    // Initialize when component is completed
    Component.onCompleted: {
        console.log("OrderBookWindow initialized")
        
        // Initialize models if not provided
        if (!orderBookModel && typeof model_mgr !== 'undefined' && model_mgr) {
            orderBookModel = model_mgr.reqOrderBookModel({"symbol": currentSymbol})
        }
        
        console.log("Window loaded with symbol:", currentSymbol)
    }

    // Window close event
    onClosing: {
        console.log("OrderBookWindow closing")
        // Clean up resources if needed
    }
}
