import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: settingsPage
    background: Rectangle {
        color: "#1e1e1e"
    }

    // Toolbar
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

            Text {
                text: "System Settings"
                font.pixelSize: 18
                font.bold: true
                color: "#ffffff"
            }

            Item { Layout.fillWidth: true }

            Button {
                text: "Save Settings"
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
                onClicked: saveSettings()
            }
        }
    }

    // Settings content
    ScrollView {
        anchors.top: toolbar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10

        ColumnLayout {
            width: parent.width
            spacing: 20

            // Connection settings
            Rectangle {
                Layout.fillWidth: true
                height: 200
                color: "#2d2d2d"
                border.color: "#404040"
                border.width: 1
                radius: 4

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 10

                    Text {
                        text: "Connection Settings"
                        font.pixelSize: 16
                        font.bold: true
                        color: "#ffffff"
                    }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        rowSpacing: 10
                        columnSpacing: 10

                        Text {
                            text: "Server Address:"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        TextField {
                            id: serverAddressField
                            Layout.fillWidth: true
                            text: "wss://stream.binance.com:9443/ws/btcusdt@trade"
                            background: Rectangle {
                                color: "#353535"
                                border.color: "#404040"
                                border.width: 1
                                radius: 4
                            }
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        Text {
                            text: "API Key:"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        TextField {
                            id: apiKeyField
                            Layout.fillWidth: true
                            text: ""
                            echoMode: TextInput.Password
                            background: Rectangle {
                                color: "#353535"
                                border.color: "#404040"
                                border.width: 1
                                radius: 4
                            }
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        Text {
                            text: "Auto Reconnect:"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        CheckBox {
                            id: autoReconnectCheckBox
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
                        }
                    }
                }
            }

            // Display settings
            Rectangle {
                Layout.fillWidth: true
                height: 250
                color: "#2d2d2d"
                border.color: "#404040"
                border.width: 1
                radius: 4

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 10

                    Text {
                        text: "Display Settings"
                        font.pixelSize: 16
                        font.bold: true
                        color: "#ffffff"
                    }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        rowSpacing: 10
                        columnSpacing: 10

                        Text {
                            text: "Theme:"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        ComboBox {
                            id: themeComboBox
                            Layout.fillWidth: true
                            model: ["Dark Theme", "Light Theme", "Auto"]
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
                        }

                        Text {
                            text: "Font Size:"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        SpinBox {
                            id: fontSizeSpinBox
                            Layout.fillWidth: true
                            from: 8
                            to: 24
                            value: 12
                            stepSize: 1
                            background: Rectangle {
                                color: "#353535"
                                border.color: "#404040"
                                border.width: 1
                                radius: 4
                            }
                            textFromValue: function(value) {
                                return value + "px"
                            }
                            valueFromText: function(text) {
                                return parseInt(text.replace("px", ""))
                            }
                        }

                        Text {
                            text: "Refresh Interval (s):"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        SpinBox {
                            id: refreshIntervalSpinBox
                            Layout.fillWidth: true
                            from: 1
                            to: 60
                            value: 5
                            stepSize: 1
                            background: Rectangle {
                                color: "#353535"
                                border.color: "#404040"
                                border.width: 1
                                radius: 4
                            }
                        }

                        Text {
                            text: "Show Grid:"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        CheckBox {
                            id: showGridCheckBox
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
                        }

                        Text {
                            text: "Show Volume:"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        CheckBox {
                            id: showVolumeCheckBox
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
                        }
                    }
                }
            }

            // Trading settings
            Rectangle {
                Layout.fillWidth: true
                height: 200
                color: "#2d2d2d"
                border.color: "#404040"
                border.width: 1
                radius: 4

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 10

                    Text {
                        text: "Trading Settings"
                        font.pixelSize: 16
                        font.bold: true
                        color: "#ffffff"
                    }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2
                        rowSpacing: 10
                        columnSpacing: 10

                        Text {
                            text: "Default Symbol:"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        ComboBox {
                            id: defaultSymbolComboBox
                            Layout.fillWidth: true
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
                        }

                        Text {
                            text: "Default Timeframe:"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        ComboBox {
                            id: defaultTimeframeComboBox
                            Layout.fillWidth: true
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
                        }

                        Text {
                            text: "Price Precision:"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        SpinBox {
                            id: pricePrecisionSpinBox
                            Layout.fillWidth: true
                            from: 0
                            to: 8
                            value: 2
                            stepSize: 1
                            background: Rectangle {
                                color: "#353535"
                                border.color: "#404040"
                                border.width: 1
                                radius: 4
                            }
                        }

                        Text {
                            text: "Volume Precision:"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        SpinBox {
                            id: volumePrecisionSpinBox
                            Layout.fillWidth: true
                            from: 0
                            to: 8
                            value: 2
                            stepSize: 1
                            background: Rectangle {
                                color: "#353535"
                                border.color: "#404040"
                                border.width: 1
                                radius: 4
                            }
                        }
                    }
                }
            }
        }
    }

    // Function to save settings
    function saveSettings() {
        console.log("Settings saved")
        // Here you would typically save the settings to a configuration file or database
    }

    // Initialize settings when page loads
    Component.onCompleted: {
        console.log("SettingsPage loaded")
    }
} 