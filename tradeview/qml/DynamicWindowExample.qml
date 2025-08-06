import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

Window {
    id: mainWindow
    width: 800
    height: 600
    visible: true
    title: "动态窗口示例"
    color: "#1e1e1e"

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 20

        Text {
            text: "动态窗口管理"
            color: "#ffffff"
            font.pixelSize: 24
            font.bold: true
        }

        Button {
            text: "创建新窗口"
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
                createNewWindow()
            }
        }

        Button {
            text: "关闭所有窗口"
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
                closeAllWindows()
            }
        }

        Text {
            text: "当前窗口数量: " + windowList.length
            color: "#cccccc"
            font.pixelSize: 16
        }
    }

    // 窗口列表
    property var windowList: []

    // 创建新窗口的函数
    function createNewWindow() {
        var component = Qt.createComponent("DynamicSubWindow.qml")
        if (component.status === Component.Ready) {
            var window = component.createObject(null, {
                "windowTitle": "动态窗口 " + (windowList.length + 1),
                "windowId": windowList.length + 1
            })
            
            if (window) {
                windowList.push(window)
                window.show()
                
                // 监听窗口关闭事件
                window.closing.connect(function() {
                    removeWindow(window)
                })
            }
        } else {
            console.log("Error creating component:", component.errorString())
        }
    }

    // 关闭所有窗口的函数
    function closeAllWindows() {
        for (var i = 0; i < windowList.length; i++) {
            if (windowList[i]) {
                windowList[i].close()
            }
        }
        windowList = []
    }

    // 从列表中移除窗口
    function removeWindow(window) {
        var index = windowList.indexOf(window)
        if (index > -1) {
            windowList.splice(index, 1)
        }
    }

    // 应用关闭时清理所有窗口
    Component.onDestruction: {
        closeAllWindows()
    }
}
