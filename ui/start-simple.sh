#!/bin/bash

echo "🚀 BTrader 订单流交易UI - 简化启动"
echo "====================================="

# 检查当前目录
if [ ! -d "src" ]; then
    echo "错误: 请在UI项目根目录运行此脚本"
    exit 1
fi

# 检查Node.js版本
echo "✅ Node.js版本: $(node --version)"

# 构建项目
echo "🔨 构建项目..."
npm run build

# 启动演示数据生成器
echo "📊 启动演示数据生成器..."
node scripts/demo-data-generator.js &
DEMO_PID=$!

# 等待数据生成器启动
sleep 3

# 启动Electron应用
echo "🖥️  启动Electron应用..."
NODE_ENV=development npx electron out/main/index.js

# 清理进程
echo "🧹 清理进程..."
kill $DEMO_PID 2>/dev/null

echo "✅ 应用已关闭" 