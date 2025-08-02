@echo off
echo 启动 BTrader UI 演示模式
echo ==========================

REM 检查 Node.js 是否安装
where node >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo 错误: Node.js 未安装，请先安装 Node.js 16+ 版本
    pause
    exit /b 1
)

REM 检查 npm 是否安装
where npm >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo 错误: npm 未安装，请先安装 npm
    pause
    exit /b 1
)

REM 检查当前目录
if not exist "src" (
    echo 错误: 请在UI项目根目录运行此脚本
    pause
    exit /b 1
)

REM 检查是否已安装依赖
if not exist "node_modules" (
    echo 正在安装依赖...
    npm install
    if %ERRORLEVEL% NEQ 0 (
        echo 错误: 依赖安装失败
        pause
        exit /b 1
    )
)

echo 启动演示数据生成器...
start /b node scripts/demo-data-generator.js

REM 等待一秒让数据生成器启动
timeout /t 1 /nobreak >nul

echo 启动 Electron 应用...
npm run dev

echo.
echo 演示环境已启动!
echo 按任意键退出...
pause >nul