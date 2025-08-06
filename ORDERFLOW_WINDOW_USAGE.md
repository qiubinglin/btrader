# OrderFlowWindow 使用说明

## 概述

`OrderFlowWindow.qml` 是一个独立的窗口组件，它嵌入了 `OrderFlowPage.qml` 页面，提供了完整的订单流交易分析界面。

## 文件结构

```
tradeview/qml/
├── windows/
│   └── OrderFlowWindow.qml      # 主窗口组件
├── pages/
│   └── OrderFlowPage.qml        # 订单流页面组件
├── main.qml                     # 原始主文件
├── OrderFlowApp.qml             # 新的启动文件
└── qml.qrc                      # 资源文件
```

## 功能特性

### OrderFlowWindow.qml
- **独立窗口**：完整的应用程序窗口，包含菜单栏
- **数据模型管理**：管理 candlestick、orderBook、tickTrade 数据模型
- **多窗口支持**：可以创建多个 OrderFlow 窗口实例
- **窗口控制**：支持最大化、最小化、全屏等操作
- **关于对话框**：显示应用程序信息

### OrderFlowPage.qml
- **订单流分析**：专业的订单流交易分析界面
- **K线图显示**：支持缩放和交互的K线图
- **实时数据**：显示实时市场数据
- **工具栏**：提供各种分析工具

## 使用方法

### 1. 直接启动 OrderFlowWindow

```bash
# 使用 OrderFlowApp.qml 启动
qmlscene OrderFlowApp.qml
```

### 2. 在代码中创建 OrderFlowWindow

```qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

// 创建 OrderFlowWindow 实例
OrderFlowWindow {
    id: orderFlowWindow
    width: 1400
    height: 900
    visible: true
    
    // 设置数据模型
    candlestickModel: myCandlestickModel
    orderBookModel: myOrderBookModel
    tickTradeModel: myTickTradeModel
    
    // 设置交易对
    currentSymbol: "BTC/USDT"
}
```

### 3. 动态创建多个窗口

```qml
// 创建新窗口的函数
function createNewOrderFlowWindow() {
    var component = Qt.createComponent("windows/OrderFlowWindow.qml")
    if (component.status === Component.Ready) {
        var newWindow = component.createObject(null, {
            "currentSymbol": "ETH/USDT",
            "candlestickModel": candlestickModel,
            "orderBookModel": orderBookModel,
            "tickTradeModel": tickTradeModel
        })
        
        if (newWindow) {
            newWindow.show()
        }
    }
}
```

## 属性说明

### OrderFlowWindow 属性

| 属性 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `candlestickModel` | var | null | K线图数据模型 |
| `orderBookModel` | var | null | 订单簿数据模型 |
| `tickTradeModel` | var | null | 逐笔交易数据模型 |
| `currentSymbol` | string | "BTC/USDT" | 当前交易对 |

### OrderFlowPage 属性

| 属性 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| `symbol` | string | "" | 交易对符号 |
| `candlestickModel` | var | null | K线图数据模型 |
| `zoomFactor` | real | 1.0 | 缩放因子 |
| `maxVisibleCandlesticks` | int | 50 | 最大可见K线数量 |

## 菜单功能

### 文件菜单
- **新建窗口**：创建新的 OrderFlow 窗口实例
- **退出**：关闭当前窗口

### 视图菜单
- **全屏**：切换到全屏模式
- **最大化**：最大化窗口
- **最小化**：最小化窗口

### 帮助菜单
- **关于**：显示应用程序信息

## 数据模型集成

OrderFlowWindow 会自动初始化数据模型：

```qml
Component.onCompleted: {
    // 初始化模型
    if (!candlestickModel && typeof model_mgr !== 'undefined' && model_mgr) {
        candlestickModel = model_mgr.reqCandlestickModel({"symbol": currentSymbol})
    }
    if (!orderBookModel && typeof model_mgr !== 'undefined' && model_mgr) {
        orderBookModel = model_mgr.reqOrderBookModel({"symbol": currentSymbol})
    }
    if (!tickTradeModel && typeof model_mgr !== 'undefined' && model_mgr) {
        tickTradeModel = model_mgr.reqTickTradeModel({"symbol": currentSymbol})
    }
}
```

## 窗口管理

### 创建新窗口
- 通过菜单 "文件" -> "新建窗口"
- 新窗口会继承当前窗口的数据模型
- 新窗口位置会稍微偏移以避免重叠

### 窗口生命周期
- 窗口创建时自动初始化数据模型
- 窗口关闭时自动清理资源
- 支持多窗口同时运行

## 样式和主题

OrderFlowWindow 使用深色主题：
- 背景色：`#1e1e1e`
- 边框色：`#404040`
- 文本色：`#ffffff`
- 强调色：`#00ff00`

## 注意事项

1. **资源文件**：确保 `OrderFlowWindow.qml` 和 `OrderFlowPage.qml` 都已添加到 `qml.qrc` 中
2. **导入路径**：OrderFlowWindow 使用相对路径导入 pages 目录
3. **数据模型**：确保 C++ 后端提供了相应的数据模型
4. **窗口大小**：设置了最小窗口大小（800x600）以确保良好的用户体验

## 扩展功能

可以根据需要扩展 OrderFlowWindow：

1. **添加更多菜单项**：如保存配置、导入数据等
2. **添加工具栏**：提供常用功能的快捷访问
3. **添加状态栏**：显示当前状态信息
4. **添加设置对话框**：配置应用程序参数
5. **添加键盘快捷键**：提高操作效率
