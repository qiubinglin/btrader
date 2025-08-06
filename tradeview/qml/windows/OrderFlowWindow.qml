import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

// Import pages
import "../pages"

Window {
    id: orderFlowWindow
    width: 1400
    height: 900
    visible: true
    title: "Order Flow - Order Flow Trading System"
    color: "#1e1e1e"
    
    // Window properties
    minimumWidth: 800
    minimumHeight: 600
    
    // Data models - will be passed to the page
    property var candlestickModel: null
    
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
                onTriggered: orderFlowWindow.close()
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
                    console.log("Refreshing order flow data...")
                    // Data is automatically updated by DataManager
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

        // Embed OrderFlowPage
        OrderFlowPage {
            id: orderFlowPage
            anchors.fill: parent
            
            // Pass properties to the page
            symbol: currentSymbol
            candlestickModel: candlestickModel
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
        console.log("OrderFlowWindow initialized")
        
        // Initialize models if not provided
        if (!candlestickModel && typeof model_mgr !== 'undefined' && model_mgr) {
            candlestickModel = model_mgr.reqCandlestickModel({"symbol": currentSymbol})
        }
        
        console.log("Window loaded with symbol:", currentSymbol)
    }

    // Window close event
    onClosing: {
        console.log("OrderFlowWindow closing")
        // Clean up resources if needed
    }
}
