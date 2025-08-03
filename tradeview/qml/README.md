# TradeView QML 多页面系统使用指南

## 概述

TradeView 使用以 `main.qml` 为主入口的多页面架构，通过 StackView 实现页面导航和数据共享。

## 架构设计

```
main.qml (主入口)
├── NavigationBar (左侧导航栏)
├── StackView (中间内容区域)
│   ├── CandlestickPage.qml (K线图表页面)
│   ├── TickTradePage.qml (逐笔成交页面)
│   ├── OrderBookPage.qml (订单簿页面)
│   ├── FootprintPage.qml (足迹图页面)
│   ├── MicroOrderBookPage.qml (微盘口页面)
│   └── SettingsPage.qml (设置页面)
└── DataPanel (右侧数据面板)
```

## 使用方法

### 1. 页面导航

通过左侧导航栏可以切换不同的页面：

```qml
// 在 NavigationBar 中点击按钮
Button {
    onClicked: {
        navigationBar.currentPage = "pages/OrderBookPage.qml"
        pageChanged("pages/OrderBookPage.qml")
    }
}
```

### 2. 页面切换

StackView 会自动处理页面切换：

```qml
// 在 main.qml 中处理页面切换
onPageChanged: function(page) {
    switch(page) {
        case "pages/OrderBookPage.qml":
            stackView.replace("pages/OrderBookPage.qml")
            break
        // ... 其他页面
    }
}
```

### 3. 数据共享

所有页面都可以访问通过 C++ 注册的数据模型：

```qml
// 在任何页面中都可以使用
ListView {
    model: orderBookModel  // C++ 注册的模型
    delegate: Rectangle {
        Text { text: model.price }
    }
}
```

## 组件说明

### NavigationBar
- **位置**: 左侧导航栏
- **功能**: 页面导航、当前页面高亮
- **文件**: `components/NavigationBar.qml`

### StackView
- **位置**: 中间内容区域
- **功能**: 页面管理、切换动画
- **配置**: 在 `main.qml` 中配置

### DataPanel
- **位置**: 右侧数据面板
- **功能**: 显示实时市场数据
- **文件**: `components/DataPanel.qml`

## 添加新页面

### 1. 创建页面文件

```qml
// pages/NewPage.qml
import QtQuick 2.15
import QtQuick.Controls 2.15

Page {
    id: newPage
    
    // 页面内容
    Rectangle {
        anchors.fill: parent
        color: "#1e1e1e"
        
        Text {
            text: "新页面"
            color: "#ffffff"
            anchors.centerIn: parent
        }
    }
}
```

### 2. 更新资源文件

```xml
<!-- qml.qrc -->
<file>pages/NewPage.qml</file>
```

### 3. 添加到导航

```qml
// components/NavigationBar.qml
property var pageModel: [
    // ... 现有页面
    { name: "新页面", page: "pages/NewPage.qml", icon: "🆕" }
]
```

### 4. 添加切换逻辑

```qml
// main.qml
onPageChanged: function(page) {
    switch(page) {
        // ... 现有页面
        case "pages/NewPage.qml":
            stackView.replace("pages/NewPage.qml")
            break
    }
}
```

## 数据模型

### 可用的 C++ 模型

- `candlestickModel`: K线数据模型
- `tickTradeModel`: 逐笔成交模型
- `orderBookModel`: 订单簿模型
- `footprintModel`: 足迹图模型
- `microOrderBookModel`: 微盘口模型

### 在页面中使用模型

```qml
// 在任何页面中
ListView {
    model: candlestickModel
    delegate: Rectangle {
        Text { text: model.open }
        Text { text: model.close }
    }
}
```

## 样式和主题

### 颜色方案

- 主背景: `#1e1e1e`
- 面板背景: `#2d2d2d`
- 边框: `#404040`
- 文字: `#ffffff`
- 绿色: `#00ff00` (买单/上涨)
- 红色: `#ff0000` (卖单/下跌)

### 布局尺寸

- 导航栏宽度: 200px
- 数据面板宽度: 300px
- 主内容区域: 自适应

## 性能优化

### 1. 页面懒加载

StackView 支持页面懒加载，只有在需要时才创建页面实例。

### 2. 数据更新

C++ 模型通过信号槽机制自动更新 QML 界面，无需手动刷新。

### 3. 内存管理

Qt 的对象树自动管理内存，页面切换时会自动清理不需要的对象。

## 调试和开发

### 控制台输出

```qml
// 在页面中添加调试信息
Component.onCompleted: {
    console.log("页面加载完成")
}
```

### 错误处理

```qml
// 在 main.qml 中处理页面加载错误
StackView {
    onStatusChanged: {
        if (status === StackView.Error) {
            console.error("页面加载失败:", error)
        }
    }
}
```

## 扩展建议

### 1. 添加页面缓存

```qml
StackView {
    // 缓存页面实例
    cache: true
}
```

### 2. 添加页面参数传递

```qml
// 在页面切换时传递参数
stackView.replace("pages/OrderBookPage.qml", {
    symbol: "BTC/USDT",
    timeframe: "1m"
})
```

### 3. 添加页面状态保存

```qml
// 保存页面状态
property var pageStates: ({})
```

这个多页面系统提供了灵活、可扩展的架构，便于添加新功能和页面。 