import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

ApplicationWindow {
    id: root
    width: 1200
    height: 800
    visible: true
    title: qsTr("订单流管理系统")

    property OrderManager order_manager: OrderManager {}

    RowLayout {
        anchors.fill: parent
        spacing: 10

        OrderView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * 0.7
            order_manager: root.order_manager
        }

        OrderForm {
            Layout.fillHeight: true
            Layout.preferredWidth: parent.width * 0.3
            order_manager: root.order_manager
        }
    }
}