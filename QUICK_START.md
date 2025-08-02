# 🚀 BTrader 快速启动指南

## 立即开始使用！

### 第一步：环境检查
确保您已安装：
- Node.js 16+ 
- npm 8+

```bash
node --version  # 应显示 v16.x.x 或更高
npm --version   # 应显示 8.x.x 或更高
```

### 第二步：选择启动方式

#### 🎯 方式一：演示模式 (推荐首次体验)
```bash
cd ui
./start-demo.sh        # Linux/macOS
# 或
start-demo.bat         # Windows
```

演示模式将会：
- ✅ 自动安装所有依赖
- ✅ 启动模拟数据生成器
- ✅ 生成真实的市场数据
- ✅ 启动交易界面
- ✅ 可以进行模拟交易操作

#### 🎯 方式二：连接真实交易系统
```bash
# 1. 确保您的C++交易系统正在运行：
#    - 行情数据端口: 5555 (nanomsg PUB)
#    - 交易接口端口: 5556 (nanomsg REP)

# 2. 启动UI
cd ui
npm install  # 首次需要安装依赖
npm run dev
```

### 第三步：界面操作说明

#### 📊 订单流分析
1. **K线图表**：
   - 点击图表价格可快速设置下单价格
   - 使用右上角按钮切换时间周期
   - 滚轮缩放，拖拽移动图表

2. **成交明细**：
   - 显示最新200笔成交
   - 点击任意价格快速下单
   - 绿色=买入，红色=卖出

3. **买卖档位**：
   - 显示10档深度
   - 点击任意档位价格快速下单
   - 灰色条表示成交量大小

4. **足迹图**：
   - 调整右上角聚合度
   - 查看每个价格的买卖力道
   - Delta值显示净买卖压力

5. **微盘口**：
   - 一键买入/卖出最优价格
   - 查看深度统计
   - 监控买卖比变化

#### 💼 交易操作
1. **下单流程**：
   ```
   选择品种 → 设置方向 → 输入价格 → 输入数量 → 确认下单
   ```

2. **快速下单技巧**：
   - 点击图表/盘口价格自动填入价格
   - 使用"市价/买一/卖一"快速设置价格
   - 使用"25%/50%/75%/最大"快速设置数量

3. **持仓管理**：
   - 查看实时盈亏
   - 一键平仓或加仓
   - 批量平仓所有持仓

4. **订单管理**：
   - 查看活跃订单
   - 撤单或改单
   - 批量撤销所有订单

### 第四步：自定义配置

#### 修改连接地址
编辑 `ui/src/main/nanomsg-bridge.ts`：
```typescript
// 第33行和第36行
this.marketDataSocket.connect('tcp://YOUR_IP:YOUR_PORT')
this.commandSocket.connect('tcp://YOUR_IP:YOUR_PORT')
```

#### 添加交易品种
编辑组件中的品种选择器，例如 `ui/src/renderer/src/components/TradingPanel.vue`：
```vue
<el-option label="YOUR_SYMBOL" value="YOUR_SYMBOL" />
```

#### 修改界面主题
编辑 `ui/src/renderer/src/styles/variables.scss`：
```scss
$bg-primary: #YOUR_COLOR;      // 主背景色
$text-primary: #YOUR_COLOR;    // 主文字色
$buy-color: #YOUR_COLOR;       // 买入颜色
$sell-color: #YOUR_COLOR;      // 卖出颜色
```

## 🐛 故障排除

### 常见问题

**Q: 界面显示"未连接"状态**
A: 检查您的C++系统是否在端口5555和5556上运行

**Q: 下单失败**
A: 确认您的C++系统正确响应交易请求，检查消息格式

**Q: 数据不更新**
A: 验证您的C++系统是否按照协议格式发送数据

**Q: 安装依赖失败**
A: 尝试清理缓存：`npm cache clean --force` 然后重新安装

### 查看日志
- **主进程日志**: 在终端中查看
- **渲染进程日志**: 按F12打开开发者工具

### 重置环境
```bash
cd ui
rm -rf node_modules package-lock.json
npm install
```

## 📞 获得帮助

如遇问题，请准备以下信息：
1. 操作系统版本
2. Node.js 和 npm 版本  
3. 错误日志截图
4. 操作步骤描述

## 🎉 享受交易！

现在您已经拥有了一个功能完整的订单流交易界面！

主要特性：
- ✅ 实时行情展示
- ✅ 专业订单流分析
- ✅ 完整交易功能
- ✅ 风险管理工具
- ✅ 现代化界面设计

开始您的量化交易之旅吧！ 🚀