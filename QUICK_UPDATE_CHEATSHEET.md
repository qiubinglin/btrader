# 🚀 NPM & Electron 快速更新参考

## 📋 快速命令

### 检查版本
```bash
node --version          # Node.js 版本
npm --version           # NPM 版本
npm list electron       # Electron 版本
npm list electron-builder # Electron Builder 版本
```

### 更新 NPM
```bash
npm install -g npm@latest
```

### 更新 Electron
```bash
npm install electron@latest
npm install electron-builder@latest
```

### 清理和重装
```bash
npm cache clean --force
rm -rf node_modules package-lock.json
npm install
```

### 设置镜像（加速下载）
```bash
export ELECTRON_MIRROR=https://npmmirror.com/mirrors/electron/
npm config set electron_mirror https://npmmirror.com/mirrors/electron/
```

## 🔧 一键更新脚本

### 使用自动化脚本
```bash
cd /home/zfwq/btrader/ui
chmod +x update-deps.sh
./update-deps.sh
```

### 手动更新流程
```bash
# 1. 备份
cp package.json package.json.backup

# 2. 更新 npm
npm install -g npm@latest

# 3. 设置镜像
export ELECTRON_MIRROR=https://npmmirror.com/mirrors/electron/

# 4. 清理缓存
npm cache clean --force

# 5. 更新依赖
npm install electron@latest
npm install electron-builder@latest
npm update

# 6. 重装依赖
rm -rf node_modules package-lock.json
npm install

# 7. 测试构建
npm run build
```

## 🛠️ 常见问题解决

### 权限问题
```bash
sudo chown -R $USER:$GROUP ~/.npm
```

### 下载失败
```bash
npm config set registry https://registry.npmmirror.com
export ELECTRON_MIRROR=https://npmmirror.com/mirrors/electron/
```

### 构建失败
```bash
rm -rf out/ dist_electron/
npm run build
```

## 📝 更新检查清单

- [ ] 备份 package.json
- [ ] 更新 npm
- [ ] 设置 Electron 镜像
- [ ] 更新 Electron
- [ ] 更新 Electron Builder
- [ ] 清理缓存
- [ ] 重新安装依赖
- [ ] 测试构建
- [ ] 验证功能

## 🔗 相关文档

- [详细更新指南](./NPM_ELECTRON_UPDATE_GUIDE.md)
- [Node.js 升级指南](./NODEJS_UPGRADE_GUIDE.md)

---

*快速参考 - 2025年8月* 