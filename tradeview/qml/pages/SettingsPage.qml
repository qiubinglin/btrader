import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {
    id: settingsPage
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

            Text {
                text: "系统设置"
                font.pixelSize: 18
                font.bold: true
                color: "#ffffff"
            }

            Item { Layout.fillWidth: true }

            Button {
                text: "保存设置"
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

    // 设置内容
    ScrollView {
        anchors.top: toolbar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10

        ColumnLayout {
            width: parent.width
            spacing: 20

            // 连接设置
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
                        text: "连接设置"
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
                            text: "服务器地址:"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        TextField {
                            id: serverAddressField
                            text: "ws://localhost:8080"
                            Layout.fillWidth: true
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
                            text: "API密钥:"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        TextField {
                            id: apiKeyField
                            text: ""
                            echoMode: TextInput.Password
                            Layout.fillWidth: true
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
                            text: "自动重连:"
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

            // 显示设置
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
                        text: "显示设置"
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
                            text: "主题:"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        ComboBox {
                            id: themeComboBox
                            model: ["深色主题", "浅色主题", "自动"]
                            currentIndex: 0
                            Layout.fillWidth: true
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
                            text: "字体大小:"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        SpinBox {
                            id: fontSizeSpinBox
                            from: 8
                            to: 20
                            value: 12
                            stepSize: 1
                            Layout.fillWidth: true
                            background: Rectangle {
                                color: "#353535"
                                border.color: "#404040"
                                border.width: 1
                                radius: 4
                            }
                        }

                        Text {
                            text: "刷新间隔(秒):"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        SpinBox {
                            id: refreshIntervalSpinBox
                            from: 1
                            to: 60
                            value: 5
                            stepSize: 1
                            Layout.fillWidth: true
                            background: Rectangle {
                                color: "#353535"
                                border.color: "#404040"
                                border.width: 1
                                radius: 4
                            }
                        }

                        Text {
                            text: "显示网格:"
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
                            text: "显示成交量:"
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

            // 交易设置
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
                        text: "交易设置"
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
                            text: "默认交易对:"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        ComboBox {
                            id: defaultSymbolComboBox
                            model: ["BTC/USDT", "ETH/USDT", "BNB/USDT", "ADA/USDT"]
                            currentIndex: 0
                            Layout.fillWidth: true
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
                            text: "默认时间周期:"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        ComboBox {
                            id: defaultTimeframeComboBox
                            model: ["1m", "5m", "15m", "1h", "4h", "1d"]
                            currentIndex: 0
                            Layout.fillWidth: true
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
                            text: "价格精度:"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        SpinBox {
                            id: pricePrecisionSpinBox
                            from: 0
                            to: 8
                            value: 2
                            stepSize: 1
                            Layout.fillWidth: true
                            background: Rectangle {
                                color: "#353535"
                                border.color: "#404040"
                                border.width: 1
                                radius: 4
                            }
                        }

                        Text {
                            text: "数量精度:"
                            color: "#ffffff"
                            font.pixelSize: 12
                        }

                        SpinBox {
                            id: quantityPrecisionSpinBox
                            from: 0
                            to: 8
                            value: 4
                            stepSize: 1
                            Layout.fillWidth: true
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

    // 保存设置函数
    function saveSettings() {
        // 这里可以添加保存设置的逻辑
        console.log("设置已保存")
    }
} 