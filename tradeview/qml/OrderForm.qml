import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    property alias order_manager: root.order_manager

    id: root
    color: "#e9e9e9"

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 15

        Label {
            text: qsTr("创建新订单")
            font.pixelSize: 18
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        TextField {
            id: product_name_field
            placeholderText: qsTr("产品名称")
            Layout.fillWidth: true
        }

        RowLayout {
            TextField {
                id: quantity_field
                placeholderText: qsTr("数量")
                validator: IntValidator { bottom: 1 }
                Layout.fillWidth: true
            }

            TextField {
                id: price_field
                placeholderText: qsTr("单价")
                validator: DoubleValidator { bottom: 0.01 }
                Layout.fillWidth: true
            }
        }

        TextField {
            id: customer_field
            placeholderText: qsTr("客户名称")
            Layout.fillWidth: true
        }

        Button {
            text: qsTr("提交订单")
            Layout.fillWidth: true
            onClicked: {
                if (product_name_field.text && quantity_field.text && price_field.text && customer_field.text) {
                    order_manager.add_order(
                        product_name_field.text,
                        parseInt(quantity_field.text),
                        parseFloat(price_field.text),
                        customer_field.text
                    );
                    
                    product_name_field.text = "";
                    quantity_field.text = "";
                    price_field.text = "";
                    customer_field.text = "";
                }
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}