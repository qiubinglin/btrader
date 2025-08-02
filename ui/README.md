# BTrader 订单流交易界面

基于 Electron + Vue 3 + nanomsg 的订单流交易界面，专为量化交易系统设计。

## 功能特性

### 🎯 订单流交易
- **实时K线图表**: 基于 ECharts 的专业K线图，支持多时间周期
- **逐笔成交明细**: 实时显示市场成交数据，支持点击价格快速下单
- **买卖档位展示**: 深度订单簿显示，可视化市场深度
- **足迹图分析**: 价格档位成交量分析，买卖力道对比
- **微盘口**: 最优买卖价快速交易界面

### 💼 交易功能
- **智能下单面板**: 支持限价单、市价单，快速价格和数量设置
- **持仓管理**: 实时持仓状态，盈亏统计，一键平仓
- **订单管理**: 活跃订单监控，批量撤单，改单功能
- **风险控制**: 余额检查，最大下单量限制

### 🔌 系统集成
- **nanomsg通信**: 与 C++ 交易系统无缝对接
- **实时数据流**: 高频行情数据处理，低延迟显示
- **双向通信**: 行情接收 + 交易指令发送

## 技术架构

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Vue 3 UI      │    │  Electron Main  │    │  C++ Trading    │
│                 │◄──►│                 │◄──►│     System      │
│ - 交易界面      │    │ - nanomsg桥接   │    │                 │
│ - 图表组件      │    │ - 数据转发      │    │ - 行情数据      │
│ - 状态管理      │    │ - IPC通信       │    │ - 交易执行      │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

## 安装和运行

### 前置要求
- Node.js 16+
- C++ 交易系统提供WebSocket接口：端口 8080 (行情) 和 8081 (交易)

### 安装依赖
```bash
cd ui
npm install
```

### 开发模式
```bash
npm run dev
```

### 构建应用
```bash
# Windows
npm run build:win

# macOS
npm run build:mac

# Linux
npm run build:linux
```

## 配置说明

### WebSocket 连接配置
在 `src/main/websocket-bridge.ts` 中修改连接地址：

```typescript
// 市场数据接收
this.marketDataWs = new WebSocket('ws://127.0.0.1:8080/market')

// 交易指令发送
this.commandWs = new WebSocket('ws://127.0.0.1:8081/trading')
```

### 消息格式

#### 行情数据消息格式
```json
{
  "type": "tick|orderbook|kline|footprint|micro_orderbook",
  "data": {
    "symbol": "BTC/USDT",
    "price": 50000.00,
    "volume": 1.5,
    "timestamp": 1640995200000,
    "side": "buy"
  }
}
```

#### 交易指令消息格式
```json
{
  "type": "place_order|cancel_order|query_position|query_orders",
  "data": {
    "symbol": "BTC/USDT",
    "side": "buy",
    "volume": 1.0,
    "price": 50000.00,
    "orderType": "limit"
  }
}
```

## 界面布局

```
┌─────────────────────────────────────────────────────────────────┐
│                        BTrader 订单流交易系统                   │
├─────────────────────────────────────┬───────────────────────────┤
│                                     │                           │
│           订单流图表                │        交易面板           │
│        (K线 + 成交量)               │                           │
│                                     │                           │
├─────────────────────────────────────┼───────────────────────────┤
│ 成交明细 │ 买卖档位 │ 足迹图 │ 微盘口 │     持仓状态 │ 委托订单   │
│          │          │        │      │              │            │
└─────────────────────────────────────┴───────────────────────────┘
```

## 开发指南

### 添加新的图表组件
1. 在 `src/renderer/src/components/` 创建组件
2. 在 `App.vue` 中引入和使用
3. 在对应的 store 模块中添加数据处理逻辑

### 扩展交易功能
1. 在 `store/modules/trading.ts` 中添加新的 action
2. 在 `nanomsg-bridge.ts` 中添加对应的消息处理
3. 在交易面板组件中添加 UI 控件

### 自定义样式
在 `src/renderer/src/styles/variables.scss` 中修改颜色和尺寸变量。

## 许可证

MIT License