#!/bin/bash

echo "🚀 BTrader 订单流交易UI - 完整启动脚本"
echo "=========================================="

# 检查当前目录
if [ ! -d "src" ]; then
    echo "错误: 请在UI项目根目录运行此脚本"
    exit 1
fi

# 检查Node.js版本
echo "✅ 检查Node.js版本..."
node --version
npm --version

# 检查并安装Electron依赖
echo "🔧 检查Electron依赖..."
if ! ldd node_modules/electron/dist/electron 2>/dev/null | grep -q "libnss3.so"; then
    echo "⚠️  检测到缺少Electron依赖，正在安装..."
    chmod +x install-electron-deps.sh
    ./install-electron-deps.sh
else
    echo "✅ Electron依赖已就绪"
fi

# 检查端口占用
echo "🔍 检查端口占用..."
if lsof -i :8082 >/dev/null 2>&1; then
    echo "⚠️  端口8082被占用，正在清理..."
    sudo fuser -k 8082/tcp 2>/dev/null
fi
if lsof -i :8083 >/dev/null 2>&1; then
    echo "⚠️  端口8083被占用，正在清理..."
    sudo fuser -k 8083/tcp 2>/dev/null
fi

# 检查是否已安装依赖
if [ ! -d "node_modules" ]; then
    echo "正在安装依赖..."
    npm install
    if [ $? -ne 0 ]; then
        echo "错误: 依赖安装失败"
        exit 1
    fi
fi

# 构建项目
echo "🔨 构建项目..."
npm run build
if [ $? -ne 0 ]; then
    echo "错误: 项目构建失败"
    exit 1
fi

# 启动演示数据生成器
echo "📊 启动演示数据生成器..."
node scripts/demo-data-generator.js &
DEMO_PID=$!

# 等待数据生成器启动
sleep 5

# 启动Electron应用
echo "🖥️  启动Electron应用..."
NODE_ENV=development npx electron out/main/index.js

# 清理进程
echo "🧹 清理进程..."
kill $DEMO_PID 2>/dev/null

echo "✅ 应用已关闭" 