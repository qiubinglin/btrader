#!/bin/bash

echo "🔧 安装Electron依赖..."

# 检查系统类型
if [ -f /etc/debian_version ]; then
    echo "检测到Debian/Ubuntu系统"
    sudo apt-get update
    sudo apt-get install -y libnss3 libatk-bridge2.0-0 libdrm2 libxkbcommon0 libxcomposite1 libxdamage1 libxrandr2 libgbm1 libxss1 libasound2
elif [ -f /etc/redhat-release ]; then
    echo "检测到RedHat/CentOS系统"
    sudo yum install -y nss atk atk-bridge-x11 libdrm libxkbcommon libxcomposite libxdamage libxrandr mesa-libgbm libXScrnSaver alsa-lib
else
    echo "未知系统，尝试安装通用依赖..."
    sudo apt-get update
    sudo apt-get install -y libnss3 libatk-bridge2.0-0 libdrm2 libxkbcommon0 libxcomposite1 libxdamage1 libxrandr2 libgbm1 libxss1 libasound2
fi

echo "✅ Electron依赖安装完成" 