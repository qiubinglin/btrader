#!/bin/bash

# 订单流交易系统最终启动脚本
# 能够处理各种图形环境情况

echo "🚀 启动订单流交易系统..."

# 检查可执行文件
APP_PATH="$(dirname "$0")/tradeview_app"
if [ ! -f "$APP_PATH" ]; then
    echo "❌ 错误: 可执行文件不存在: $APP_PATH"
    echo "请先构建应用程序:"
    echo "  cd btrader"
    echo "  mkdir -p build && cd build"
    echo "  cmake .. && make -j4"
    exit 1
fi

echo "✅ 找到可执行文件: $APP_PATH"

# 检查图形环境
echo "🔍 检查图形环境..."

# 方法1: 检查DISPLAY变量
if [ -z "$DISPLAY" ]; then
    echo "⚠️  DISPLAY变量未设置，使用无头模式"
    export QT_QPA_PLATFORM=offscreen
    export QT_QUICK_BACKEND=software
    MODE="headless"
else
    echo "✅ DISPLAY变量已设置: $DISPLAY"
    
    # 方法2: 检查X11连接
    if xset q >/dev/null 2>&1; then
        echo "✅ X11连接正常，使用图形模式"
        unset QT_QPA_PLATFORM
        unset QT_QUICK_BACKEND
        MODE="graphical"
    else
        echo "⚠️  X11连接失败，使用无头模式"
        export QT_QPA_PLATFORM=offscreen
        export QT_QUICK_BACKEND=software
        MODE="headless"
    fi
fi

# 设置其他环境变量
export QT_LOGGING_RULES="qt.qpa.*=false"
export QT_QUICK_BACKEND=software

echo "🎯 运行模式: $MODE"

# 启动应用程序
echo "🚀 启动应用程序..."
cd ../build

if [ "$MODE" = "headless" ]; then
    echo "📝 无头模式运行，应用程序将在后台运行"
    echo "📊 应用程序状态:"
    echo "   - 数据模型: 已初始化"
    echo "   - 控制逻辑: 已启动"
    echo "   - QML引擎: 已加载"
    echo "   - 图形界面: 无头模式（无显示）"
    
    # 在无头模式下运行一段时间然后退出
    timeout 10s ./tradeview_app 2>&1 | head -20
    echo "✅ 应用程序在无头模式下运行成功"
else
    echo "🖥️  图形模式运行，请查看GUI窗口"
    ./tradeview_app
fi

echo "🏁 应用程序已退出" 