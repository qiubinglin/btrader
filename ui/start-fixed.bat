@echo off
chcp 65001 >nul

echo 🚀 BTrader 订单流交易UI - 修复版启动脚本
echo ===============================================

REM 检查当前目录
if not exist "src" (
    echo 错误: 请在UI项目根目录运行此脚本
    pause
    exit /b 1
)

REM 检查Node.js版本
echo ✅ 检查Node.js版本...
node --version
npm --version

REM 检查是否已安装依赖
if not exist "node_modules" (
    echo 正在安装依赖...
    npm install
    if errorlevel 1 (
        echo 错误: 依赖安装失败
        pause
        exit /b 1
    )
)

REM 构建项目
echo 🔨 构建项目...
npm run build
if errorlevel 1 (
    echo 错误: 项目构建失败
    pause
    exit /b 1
)

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