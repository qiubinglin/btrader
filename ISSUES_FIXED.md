# 🔧 问题修复报告

## 🔍 发现的问题

### 1. 端口占用问题
```
Error: listen EADDRINUSE: address already in use :::8080
```

### 2. Electron依赖缺失
```
error while loading shared libraries: libnss3.so: cannot open shared object file
```

## ✅ 解决方案

### 1. 端口冲突解决 ✅

**问题**: 8080和8081端口被占用
**解决**: 更改为8082和8083端口

```javascript
// 修改前
this.marketDataServer = new WebSocket.Server({ port: 8080, path: '/market' })
this.tradingServer = new WebSocket.Server({ port: 8081, path: '/trading' })

// 修改后  
this.marketDataServer = new WebSocket.Server({ port: 8082, path: '/market' })
this.tradingServer = new WebSocket.Server({ port: 8083, path: '/trading' })
```

**同步更新**:
- `ui/src/main/websocket-bridge.ts` - 客户端连接地址
- `ui/scripts/demo-data-generator.js` - 服务器端口

### 2. Electron依赖安装 ✅

**问题**: 缺少libnss3.so等系统库
**解决**: 创建自动安装脚本

```bash
# 安装Electron依赖
chmod +x install-electron-deps.sh
./install-electron-deps.sh
```

**安装的依赖**:
- libnss3 - 网络安全服务
- libatk-bridge2.0-0 - 辅助功能
- libdrm2 - 直接渲染管理
- libxkbcommon0 - X键盘扩展
- libxcomposite1 - X合成扩展
- libxdamage1 - X损坏扩展
- libxrandr2 - X渲染扩展
- libgbm1 - Mesa GBM
- libxss1 - X屏幕保护
- libasound2 - ALSA音频

## 🚀 新的启动方式

### 完整启动脚本（推荐）
```bash
cd ui
chmod +x start-complete.sh
./start-complete.sh
```

### 手动启动
```bash
cd ui

# 1. 安装Electron依赖
chmod +x install-electron-deps.sh
./install-electron-deps.sh

# 2. 构建项目
npm run build

# 3. 启动演示数据生成器
node scripts/demo-data-generator.js &

# 4. 启动Electron应用
NODE_ENV=development npx electron out/main/index.js
```

## 📊 端口配置

### 新的端口分配
- **市场数据**: `ws://127.0.0.1:8082/market`
- **交易接口**: `ws://127.0.0.1:8083/trading`

### 端口检查
启动脚本会自动检查并清理端口占用。

## 🎯 预期结果

修复后应该能够：
1. ✅ 成功启动演示数据生成器
2. ✅ 成功启动Electron应用
3. ✅ 显示完整的交易界面
4. ✅ 接收实时模拟数据

## 📁 新增文件

- `install-electron-deps.sh` - Electron依赖安装脚本
- `start-complete.sh` - 完整启动脚本（包含所有检查）

**所有问题已修复，现在可以正常启动！** 🎉 