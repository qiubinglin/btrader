# TradeView Models

这个目录包含了订单流交易系统的核心数据模型，使用C++和Qt6实现。

## 概述

本项目实现了五个核心的数据模型，用于支持订单流交易系统的各种图表和数据显示需求。所有模型都继承自Qt的QAbstractListModel，支持Qt的Model/View架构，可以在QML中直接使用。

### 设计原则

- **高性能**: 使用高效的数据结构和算法
- **实时性**: 支持实时数据更新和信号通知
- **可扩展**: 模块化设计，易于扩展新功能
- **Qt集成**: 完全兼容Qt6和QML

## 模型概览

### 1. CandlestickModel (K线数据模型)
- **功能**: 管理K线图表数据
- **数据结构**: 包含开盘价、最高价、最低价、收盘价、成交量等信息
- **特性**: 
  - 支持多种时间周期（1m、5m、15m、1h等）
  - 自动计算涨跌状态和K线形态
  - 提供价格和成交量范围统计
  - 支持实时更新和批量操作
  - 计算实体高度、上下影线长度
- **应用场景**: K线图表显示、技术分析、历史数据回放

### 2. TickTradeModel (逐笔成交明细模型)
- **功能**: 管理逐笔成交数据
- **数据结构**: 包含成交时间、价格、成交量、买卖方向等
- **特性**:
  - 支持买卖方向标识（buy/sell）
  - 自动维护最新价格和累计成交量
  - 可设置最大记录数量，支持FIFO管理
  - 提供累计成交量统计
  - 支持成交ID和交易对符号
- **应用场景**: 逐笔成交明细显示、实时成交监控、成交分析

### 3. OrderBookModel (买卖档位模型)
- **功能**: 管理订单簿深度数据
- **数据结构**: 包含价格档位、挂单量、订单数量等
- **特性**:
  - 支持买卖双方档位管理
  - 自动排序和合并显示
  - 计算累计挂单量和深度百分比
  - 提供最佳买卖价和价差统计
  - 支持档位数量限制
- **应用场景**: 订单簿深度显示、市场深度分析、流动性监控

### 4. FootprintModel (足迹图模型)
- **功能**: 管理订单流足迹图数据
- **数据结构**: 包含价格单元格、买卖成交量、Delta值等
- **特性**:
  - 支持多种时间周期聚合（1m、5m、15m、1h）
  - 计算买卖压力分布和Delta值
  - 提供Delta和Delta百分比统计
  - 支持价格步长设置
  - 自动更新OHLC数据
- **应用场景**: 订单流分析、买卖压力可视化、市场微观结构分析

### 5. MicroOrderBookModel (微盘口模型)
- **功能**: 管理精简版订单簿数据
- **数据结构**: 包含价格、成交量、百分比、高亮状态等
- **特性**:
  - 支持重要档位高亮显示
  - 提供深度颜色渐变效果
  - 可设置高亮阈值和成交量阈值
  - 计算档位百分比和累计成交量
  - 自动更新最佳买卖价
- **应用场景**: 微盘口显示、重要档位监控、市场深度可视化

## 使用方法

### 在QML中使用

```qml
import QtQuick 2.15

ListView {
    model: candlestickModel
    
    delegate: Rectangle {
        width: parent.width
        height: 30
        
        Text {
            text: model.open + " - " + model.close
            color: model.isUp ? "green" : "red"
        }
    }
}
```

### 在C++中使用

```cpp
#include "candlestickmodel.h"

// 创建模型实例
CandlestickModel *model = new CandlestickModel(this);

// 添加K线数据
CandlestickData data;
data.timestamp = QDateTime::currentDateTime();
data.open = 100.0;
data.high = 105.0;
data.low = 99.0;
data.close = 103.0;
data.volume = 1000;

model->addCandlestick(data);
```

## 编译

使用CMake编译：

```bash
mkdir build
cd build
cmake ..
make
```

## 依赖

- Qt6.8.3 或更高版本
- C++17 或更高版本

## 注意事项

1. 所有模型都继承自QAbstractListModel，支持Qt的Model/View架构
2. 模型使用Q_INVOKABLE标记的方法可以在QML中直接调用
3. 模型会自动发出信号通知数据变化
4. 建议在大量数据操作时使用批量更新方法以提高性能 