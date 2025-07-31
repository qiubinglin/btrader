# 订单流交易系统

这是一个基于 C++ + Node.js + React 架构的订单流交易系统。

## 架构说明

```
C++ 核心引擎 ←→ Node.js API 服务 ←→ React 前端
```

- **C++ 引擎**: 高性能订单处理和市场数据生成
- **Node.js 服务**: RESTful API 和 WebSocket 实时通信
- **React 前端**: 现代化用户界面

## 功能特性

- ✅ 实时订单簿显示
- ✅ 价格走势图表
- ✅ 成交记录追踪
- ✅ 订单提交和管理
- ✅ 市场数据模拟
- ✅ WebSocket 实时更新

## 快速开始

### 1. 安装依赖

```bash
# 安装 C++ 编译依赖
npm install -g node-gyp

# Linux/Ubuntu
sudo apt-get install build-essential

# macOS
xcode-select --install

# Windows
# 安装 Visual Studio Build Tools
```

### 2. 编译 C++ 引擎

```bash
cd cpp
npm install node-addon-api
node-gyp configure
node-gyp build
```

### 3. 启动 Node.js 服务

```bash
cd server
npm install
npm start
```

### 4. 启动 React 前端

```bash
cd client
npm install
npm start
```

### 5. 访问应用

打开浏览器访问: http://localhost:3000

## API 接口

### REST API

- `POST /api/orders` - 提交订单
- `GET /api/orderbook/:symbol` - 获取订单簿
- `GET /api/trades/:symbol` - 获取成交记录
- `POST /api/simulation/start` - 开始市场模拟
- `POST /api/simulation/stop` - 停止市场模拟

### WebSocket 事件

- `marketData` - 实时市场数据
- `orderUpdate` - 订单更新
- `submitOrder` - 提交订单

## 开发说明

### 修改 C++ 代码

```bash
cd cpp
# 修改 src/ 下的文件
node-gyp rebuild
```

### 调试模式

```bash
# Node.js 服务调试
cd server
npm run dev

# React 前端热重载
cd client
npm start
```

## 生产部署

### Docker 部署

```bash
# 构建镜像
docker build -t trading-system .

# 运行容器
docker run -p 3000:3000 -p 8080:8080 trading-system
```

### 性能优化

- C++ 引擎使用无锁数据结构
- Node.js 集群模式
- React 生产构建
- Nginx 反向代理

## 扩展开发

### 添加新的市场数据源

```cpp
// 在 engine.cpp 中添加新的数据源
void TradingEngine::connectToExchange(const std::string& exchange) {
    // 实现交易所连接逻辑
}
```

### 添加新的订单类型

```cpp
// 扩展 Order 结构体
struct Order {
    // 现有字段...
    std::string timeInForce; // "GTC", "IOC", "FOK"
    double stopPrice;        // 止损价格
};
```

### 添加风险管理

```cpp
class RiskManager {
public:
    bool validateOrder(const Order& order);
    void updatePosition(const Trade& trade);
    double calculateRisk();
};
```

## 故障排除

### 常见问题

1. **C++ 编译失败**
   - 检查编译工具链安装
   - 确认 node-gyp 版本兼容

2. **WebSocket 连接失败**
   - 检查端口占用
   - 确认防火墙设置

3. **前端页面空白**
   - 检查 API 服务状态
   - 查看浏览器控制台错误

### 日志查看

```bash
# Node.js 服务日志
cd server && npm start

# C++ 引擎调试
node -e "console.log(require('./cpp/build/Release/trading_engine.node'))"
```

## 许可证

MIT License

## 贡献

欢迎提交 Issue 和 Pull Request！