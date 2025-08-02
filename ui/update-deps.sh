#!/bin/bash

# 🔧 BTrader 依赖更新脚本
# 用于自动更新 npm 和 Electron 依赖

set -e  # 遇到错误时退出

echo "🚀 开始更新 BTrader 项目依赖..."

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 日志函数
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 检查是否在正确的目录
if [ ! -f "package.json" ]; then
    log_error "请在项目根目录运行此脚本"
    exit 1
fi

# 备份当前配置
log_info "备份当前配置..."
cp package.json package.json.backup.$(date +%Y%m%d_%H%M%S)
log_success "配置已备份"

# 检查当前版本
log_info "检查当前版本..."
echo "Node.js 版本: $(node --version)"
echo "NPM 版本: $(npm --version)"
echo "Electron 版本: $(npm list electron --depth=0 | grep electron || echo '未安装')"

# 更新 npm
log_info "更新 npm..."
npm install -g npm@latest
log_success "npm 更新完成"

# 设置 Electron 镜像（加速下载）
log_info "设置 Electron 镜像..."
export ELECTRON_MIRROR=https://npmmirror.com/mirrors/electron/
npm config set electron_mirror https://npmmirror.com/mirrors/electron/
log_success "Electron 镜像设置完成"

# 清理缓存
log_info "清理 npm 缓存..."
npm cache clean --force
log_success "缓存清理完成"

# 更新 Electron
log_info "更新 Electron..."
npm install electron@latest
log_success "Electron 更新完成"

# 更新 Electron Builder
log_info "更新 Electron Builder..."
npm install electron-builder@latest
log_success "Electron Builder 更新完成"

# 更新其他依赖
log_info "更新其他依赖..."
npm update
log_success "依赖更新完成"

# 清理并重新安装
log_info "清理并重新安装依赖..."
rm -rf node_modules package-lock.json
npm install
log_success "依赖重新安装完成"

# 检查安全漏洞
log_info "检查安全漏洞..."
npm audit fix --force || log_warning "发现安全漏洞，请手动检查"

# 测试构建
log_info "测试构建..."
npm run build
log_success "构建测试通过"

# 显示更新结果
log_info "更新结果:"
echo "Node.js 版本: $(node --version)"
echo "NPM 版本: $(npm --version)"
echo "Electron 版本: $(npm list electron --depth=0 | grep electron)"
echo "Electron Builder 版本: $(npm list electron-builder --depth=0 | grep electron-builder)"

log_success "🎉 所有依赖更新完成！"
log_info "建议运行以下命令测试应用:"
echo "  npm run dev    # 开发模式"
echo "  npm run build  # 生产构建"

# 清理备份文件（保留最近3个）
log_info "清理旧备份文件..."
ls -t package.json.backup.* | tail -n +4 | xargs -r rm
log_success "备份文件清理完成" 