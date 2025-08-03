import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15

// Import custom components
import "components"

Window {
    id: mainWindow
    width: 1600
    height: 900
    visible: true
    title: "Order Flow Trading System"
    color: "#1e1e1e"

    // C++ models are registered via context properties, no need to rebind here

    // Main layout
    RowLayout {
        anchors.fill: parent
        spacing: 0

        // Left navigation bar
        NavigationBar {
            id: navigationBar
            Layout.preferredWidth: 200
            Layout.fillHeight: true
            
            onPageChanged: function(page) {
                // Switch to corresponding page based on page name
                switch(page) {
                    case "pages/CandlestickPage.qml":
                        stackView.replace("pages/CandlestickPage.qml")
                        break
                    case "pages/TickTradePage.qml":
                        stackView.replace("pages/TickTradePage.qml")
                        break
                    case "pages/OrderBookPage.qml":
                        stackView.replace("pages/OrderBookPage.qml")
                        break
                    case "pages/FootprintPage.qml":
                        stackView.replace("pages/FootprintPage.qml")
                        break
                    case "pages/MicroOrderBookPage.qml":
                        stackView.replace("pages/MicroOrderBookPage.qml")
                        break
                    case "pages/SettingsPage.qml":
                        stackView.replace("pages/SettingsPage.qml")
                        break
                    default:
                        stackView.replace("pages/CandlestickPage.qml")
                        break
                }
            }
        }

        // Center main content area
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#1e1e1e"
            border.color: "#404040"
            border.width: 1

            // Use StackView to manage multiple pages
            StackView {
                id: stackView
                anchors.fill: parent
                initialItem: "pages/CandlestickPage.qml"
                
                // Page transition animations
                replaceEnter: Transition {
                    PropertyAnimation {
                        property: "opacity"
                        from: 0
                        to: 1
                        duration: 200
                    }
                }
                replaceExit: Transition {
                    PropertyAnimation {
                        property: "opacity"
                        from: 1
                        to: 0
                        duration: 200
                    }
                }
            }
        }

        // Right data panel
        DataPanel {
            id: dataPanel
            Layout.preferredWidth: 300
            Layout.fillHeight: true
        }
    }

    // Initialize after page loads
    Component.onCompleted: {
        console.log("Main window loaded, initializing navigation...")
        // Default to candlestick chart page
        navigationBar.currentPage = "pages/CandlestickPage.qml"
    }
} 