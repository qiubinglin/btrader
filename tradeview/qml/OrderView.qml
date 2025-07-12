import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    property alias order_manager: order_list.model

    color: "#f5f5f5"

    ColumnLayout {
        anchors.fill: parent
        spacing: 10

        Label {
            text: qsTr("订单列表")
            font.pixelSize: 20
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 10
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ListView {
                id: order_list
                model: order_manager.orders
                spacing: 5

                delegate: Rectangle {
                    width: ListView.view.width
                    height: 120
                    radius: 5
                    color: "#ffffff"
                    border.color: "#dddddd"

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10
                        spacing: 15

                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 5

                            RowLayout {
                                Label {
                                    text: qsTr("订单号: ") + modelData.order_id
                                    font.bold: true
                                }
                                Label {
                                    text: qsTr("状态: ")
                                    font.bold: true
                                }
                                Label {
                                    text: modelData.status
                                    color: {
                                        if (modelData.status === "Completed") return "green";
                                        if (modelData.status === "Cancelled") return "red";
                                        return "orange";
                                    }
                                }
                            }

                            Label {
                                text: qsTr("产品: ") + modelData.product_name
                            }

                            Label {
                                text: qsTr("数量: ") + modelData.quantity
                            }

                            Label {
                                text: qsTr("价格: ¥") + modelData.price.toFixed(2)
                            }

                            Label {
                                text: qsTr("客户: ") + modelData.customer_name
                            }

                            Label {
                                text: qsTr("下单时间: ") + Qt.formatDateTime(modelData.order_time, "yyyy-MM-dd hh:mm:ss")
                                font.pixelSize: 10
                                color: "gray"
                            }
                        }

                        ColumnLayout {
                            Button {
                                text: qsTr("完成")
                                onClicked: order_manager.update_order_status(modelData.order_id, "Completed")
                                visible: modelData.status !== "Completed"
                            }
                            Button {
                                text: qsTr("取消")
                                onClicked: order_manager.update_order_status(modelData.order_id, "Cancelled")
                                visible: modelData.status !== "Cancelled"
                            }
                            Button {
                                text: qsTr("删除")
                                onClicked: order_manager.remove_order(modelData.order_id)
                            }
                        }
                    }
                }
            }
        }
    }
}