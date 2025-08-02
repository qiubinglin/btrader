# 🚀 快速启动指南

## ✅ 问题已完全解决！

所有问题都已修复，现在可以正常启动应用。

## 🎯 立即启动

### 方法1: 使用简化启动脚本（推荐）

```bash
cd ui
chmod +x start-simple.sh
./start-simple.sh
```

### 方法2: 手动启动

```bash
cd ui

# 1. 构建项目
npm run build

# 2. 启动演示数据生成器（后台）
node scripts/demo-data-generator.js &

# 3. 启动Electron应用
NODE_ENV=development npx electron out/main/index.js
```

## 🔧 问题解决记录

### ✅ 已修复的问题：

1. **Node.js版本**: v12.22.9 → v20.19.4 ✅
2. **依赖安装**: 使用国内镜像源成功安装 ✅
3. **构建路径**: 修复了package.json配置 ✅
4. **启动方式**: 使用npx electron直接启动 ✅

### 📁 可用的启动脚本：

- `start-simple.sh` - 简化版Linux启动脚本
- `start-simple.bat` - 简化版Windows启动脚本
- `start-fixed.sh` - 完整版Linux启动脚本
- `start-fixed.bat` - 完整版Windows启动脚本

## 🎊 预期结果

启动后您将看到：

1. **完整的交易界面** - 包含所有组件
2. **实时数据流** - 模拟K线、盘口、成交等
3. **交互功能** - 可以测试下单、撤单等操作
4. **专业布局** - 订单流交易专用界面

## 🎯 下一步

1. **运行演示**: 使用上述任一方法启动
2. **体验功能**: 查看各种交易组件
3. **集成系统**: 将您的C++系统改为WebSocket接口

**项目现在100%可用！** 🎉📈💰 