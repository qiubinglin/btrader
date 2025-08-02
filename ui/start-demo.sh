#!/bin/bash

echo "启动 BTrader UI 演示模式"
echo "=========================="

# 检查 Node.js 是否安装
if ! command -v node &> /dev/null; then
    echo "错误: Node.js 未安装，请先安装 Node.js 16+ 版本"
    exit 1
fi

# 检查 npm 是否安装
if ! command -v npm &> /dev/null; then
    echo "错误: npm 未安装，请先安装 npm"
    exit 1
fi

# 检查当前目录
if [ ! -d "src" ]; then
    echo "错误: 请在UI项目根目录运行此脚本"
    exit 1
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

echo "启动演示数据生成器..."
node scripts/demo-data-generator.js &
DEMO_PID=$!

# 等待一秒让数据生成器启动
sleep 1

echo "启动 Electron 应用..."
npm run dev &
ELECTRON_PID=$!

echo ""
echo "演示环境已启动!"
echo "数据生成器 PID: $DEMO_PID"
echo "Electron 应用 PID: $ELECTRON_PID"
echo ""
echo "按 Ctrl+C 停止演示"

# 捕获中断信号并清理进程
trap 'echo "正在停止演示..."; kill $DEMO_PID $ELECTRON_PID 2>/dev/null; exit 0' INT TERM

# 等待进程结束
wait $ELECTRON_PID