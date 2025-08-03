# 订单流交易系统 QML UI 组件

## 概述

这是一个基于Qt Quick (QML)的订单流交易系统用户界面，提供了现代化的深色主题设计和丰富的交易数据可视化功能。

## 项目结构

```
tradeview/qml/
├── main.qml                 # 主窗口
├── pages/                   # 页面组件
│   ├── CandlestickPage.qml  # K线图表页面
│   ├── FootprintPage.qml    # 足迹图页面
│   ├── MicroOrderBookPage.qml # 微盘口页面
│   ├── OrderBookPage.qml    # 买卖档位页面
│   ├── TickTradePage.qml    # 逐笔成交页面
│   └── SettingsPage.qml     # 设置页面
├── components/              # 可复用组件
│   ├── ChartToolbar.qml     # 图表工具栏
│   ├── StatusBar.qml        # 状态栏
│   ├── NavigationBar.qml    # 导航栏
│   ├── DataTable.qml        # 数据表格
│   └── FootprintChart.qml   # 足迹图组件
├── resources/               # 资源文件
│   └── qml.qrc             # QML资源文件
└── README.md               # 本文档
```

## 主要功能

### 1. 主窗口 (main.qml)
- 集成了所有页面和组件
- 提供统一的导航和状态显示
- 支持页面切换和状态管理

### 2. 页面组件

#### K线图表页面 (CandlestickPage.qml)
- 显示K线图表
- 支持多种时间周期
- 实时数据更新

#### 足迹图页面 (FootprintPage.qml)
- 显示订单流足迹图
- 买卖压力可视化
- Delta值分析

#### 逐笔成交页面 (TickTradePage.qml)
- 实时逐笔成交数据
- 成交明细表格
- 价格和成交量统计

#### 买卖档位页面 (OrderBookPage.qml)
- 买卖盘口数据
- 深度图显示
- 实时更新

#### 微盘口页面 (MicroOrderBookPage.qml)
- 微级别盘口数据
- 高频更新
- 详细档位信息

#### 设置页面 (SettingsPage.qml)
- 系统配置选项
- 连接设置
- 显示设置
- 交易设置

### 3. 可复用组件

#### ChartToolbar (ChartToolbar.qml)
```qml
ChartToolbar {
    symbolModel: ["BTC/USDT", "ETH/USDT"]
    timeframeModel: ["1m", "5m", "15m"]
    currentSymbol: "BTC/USDT"
    currentTimeframe: "1m"
    autoRefresh: true
    
    onSymbolChanged: function(symbol) {
        // 处理交易对切换
    }
    
    onTimeframeChanged: function(timeframe) {
        // 处理时间周期切换
    }
}
```

#### StatusBar (StatusBar.qml)
```qml
StatusBar {
    isConnected: true
    latestPrice: "50000.00"
    totalVolume: "1000000"
}
```

#### NavigationBar (NavigationBar.qml)
```qml
NavigationBar {
    currentPage: "CandlestickPage"
    onPageChanged: function(page) {
        // 处理页面切换
    }
}
```

#### DataTable (DataTable.qml)
```qml
DataTable {
    model: dataModel
    headers: ["时间", "价格", "成交量", "方向"]
    columnWidths: [150, 100, 100, 80]
    
    onRowClicked: function(index, rowData) {
        // 处理行点击
    }
}
```

#### FootprintChart (FootprintChart.qml)
```qml
FootprintChart {
    model: footprintModel
    cellWidth: 120
    cellHeight: 80
    showVolume: true
    showDelta: true
    showPercent: true
    
    onCellClicked: function(index, cellData) {
        // 处理单元格点击
    }
}
```

## 主题设计

### 颜色方案
- 主背景: `#1e1e1e`
- 次要背景: `#2d2d2d`
- 边框: `#404040`
- 文字: `#ffffff`
- 买入: `#00ff00`
- 卖出: `#ff0000`

### 设计特点
- 深色主题，适合长时间使用
- 现代化的圆角设计
- 清晰的视觉层次
- 响应式布局

## 数据模型

系统使用C++模型与QML进行数据绑定：

- `candlestickModel`: K线数据模型
- `tickTradeModel`: 逐笔成交模型
- `orderBookModel`: 买卖档位模型
- `footprintModel`: 足迹图模型
- `microOrderBookModel`: 微盘口模型

## 使用方法

1. 确保Qt6环境已正确安装
2. 编译C++模型库
3. 运行QML应用程序
4. 通过导航栏切换不同页面
5. 使用工具栏调整显示参数

## 扩展开发

### 添加新页面
1. 在`pages/`目录下创建新的QML文件
2. 在`NavigationBar.qml`中添加导航项
3. 在`qml.qrc`中注册新文件

### 添加新组件
1. 在`components/`目录下创建新的QML文件
2. 在`qml.qrc`中注册新文件
3. 在其他页面中引用新组件

### 自定义主题
1. 修改颜色常量
2. 调整字体大小和样式
3. 更新组件样式

## 注意事项

- 确保所有QML文件都在资源文件中注册
- 注意组件的信号和槽连接
- 保持代码的一致性和可维护性
- 测试不同屏幕尺寸下的显示效果

## 技术栈

- Qt 6.x
- Qt Quick (QML)
- Qt Quick Controls 2
- Qt Quick Layouts
- C++ (数据模型) 