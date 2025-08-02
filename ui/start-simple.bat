@echo off
chcp 65001 >nul

echo 🚀 BTrader 订单流交易UI - 简化启动
echo =====================================

REM 检查当前目录
if not exist "src" (
    echo 错误: 请在UI项目根目录运行此脚本
    pause
    exit /b 1
)

REM 检查Node.js版本
echo ✅ Node.js版本:
node --version

REM 构建项目
echo 🔨 构建项目...
npm run build

REM 启动演示数据生成器
echo 📊 启动演示数据生成器...
start /b node scripts/demo-data-generator.js

REM 等待数据生成器启动
timeout /t 3 /nobreak >nul

REM 启动Electron应用
echo 🖥️  启动Electron应用...
set NODE_ENV=development
npx electron out/main/index.js

echo ✅ 应用已关闭
pause 