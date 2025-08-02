# 🔧 NPM 和 Electron 更新指南

## 📋 目录
- [NPM 更新](#npm-更新)
- [Electron 更新](#electron-更新)
- [常见问题解决](#常见问题解决)
- [最佳实践](#最佳实践)

---

## 🚀 NPM 更新

### 1. 检查当前版本
```bash
# 检查 npm 版本
npm --version

# 检查 Node.js 版本
node --version
```

### 2. 更新 NPM 到最新版本

#### 方法一：使用 npm 自更新
```bash
# 更新 npm 到最新版本
npm install -g npm@latest

# 验证更新
npm --version
```

#### 方法二：使用 Node.js 安装器
```bash
# 下载并安装最新的 Node.js (包含最新 npm)
curl -fsSL https://deb.nodesource.com/setup_20.x | sudo -E bash -
sudo apt-get install -y nodejs

# 验证安装
node --version
npm --version
```

#### 方法三：使用 nvm (推荐)
```bash
# 安装 nvm
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.3/install.sh | bash

# 重新加载终端配置
source ~/.bashrc

# 安装最新的 Node.js
nvm install node
nvm use node
nvm alias default node

# 验证
node --version
npm --version
```

### 3. 清理 NPM 缓存
```bash
# 清理 npm 缓存
npm cache clean --force

# 验证缓存清理
npm cache verify
```

---

## ⚡ Electron 更新

### 1. 检查当前 Electron 版本
```bash
# 在项目目录中检查
cd /path/to/your/project
npm list electron

# 或者查看 package.json
cat package.json | grep electron
```

### 2. 更新 Electron 版本

#### 方法一：使用 npm update
```bash
# 更新到最新版本
npm update electron

# 或者指定版本
npm install electron@latest
```

#### 方法二：手动更新 package.json
```bash
# 编辑 package.json，更新 electron 版本
# 例如：从 "electron": "^31.0.2" 到 "electron": "^32.0.0"

# 然后重新安装
npm install
```

#### 方法三：使用 npm-check-updates
```bash
# 安装 npm-check-updates
npm install -g npm-check-updates

# 检查可更新的包
ncu

# 更新所有包到最新版本
ncu -u

# 重新安装依赖
npm install
```

### 3. 更新 Electron Builder
```bash
# 更新 electron-builder
npm update electron-builder

# 或者安装最新版本
npm install electron-builder@latest
```

### 4. 验证 Electron 更新
```bash
# 检查版本
npm list electron
npm list electron-builder

# 测试构建
npm run build
```

---

## 🔧 项目特定更新步骤

### 1. BTrader 项目更新流程
```bash
# 进入项目目录
cd /home/zfwq/btrader/ui

# 备份当前配置
cp package.json package.json.backup

# 检查当前版本
npm list electron
npm list electron-builder

# 更新 Electron
npm install electron@latest

# 更新 Electron Builder
npm install electron-builder@latest

# 清理并重新安装依赖
rm -rf node_modules package-lock.json
npm cache clean --force
npm install

# 测试构建
npm run build
```

### 2. 更新后的验证
```bash
# 检查版本
npm list electron
npm list electron-builder

# 测试应用启动
npm run dev

# 测试生产构建
npm run build
```

---

## 🛠️ 常见问题解决

### 1. NPM 权限问题
```bash
# 解决权限问题
sudo chown -R $USER:$GROUP ~/.npm
sudo chown -R $USER:$GROUP ~/.config

# 或者使用 nvm 避免权限问题
nvm install node
nvm use node
```

### 2. Electron 下载失败
```bash
# 设置 Electron 镜像
export ELECTRON_MIRROR=https://npmmirror.com/mirrors/electron/

# 或者永久设置
npm config set electron_mirror https://npmmirror.com/mirrors/electron/

# 清理缓存后重试
npm cache clean --force
npm install
```

### 3. 依赖冲突
```bash
# 检查依赖冲突
npm ls

# 强制重新安装
rm -rf node_modules package-lock.json
npm install

# 或者使用 npm audit 修复
npm audit fix
```

### 4. 构建失败
```bash
# 清理构建缓存
rm -rf out/ dist_electron/

# 重新安装依赖
npm install

# 重新构建
npm run build
```

---

## 📚 最佳实践

### 1. 版本管理
- 使用语义化版本控制
- 在更新前备份 package.json
- 记录更新日志

### 2. 安全更新
```bash
# 检查安全漏洞
npm audit

# 修复安全漏洞
npm audit fix

# 查看详细报告
npm audit --json
```

### 3. 性能优化
```bash
# 使用 npm ci 进行生产安装
npm ci

# 清理不必要的包
npm prune

# 检查包大小
npm list --depth=0
```

### 4. 自动化更新脚本
```bash
#!/bin/bash
# update-deps.sh

echo "🔧 开始更新依赖..."

# 备份当前配置
cp package.json package.json.backup

# 更新 npm
npm install -g npm@latest

# 更新项目依赖
npm update

# 更新 Electron
npm install electron@latest
npm install electron-builder@latest

# 清理缓存
npm cache clean --force

# 重新安装
rm -rf node_modules package-lock.json
npm install

# 测试构建
npm run build

echo "✅ 更新完成！"
```

---

## 📝 更新检查清单

### 更新前
- [ ] 备份 package.json
- [ ] 检查当前版本
- [ ] 查看更新日志
- [ ] 准备回滚方案

### 更新中
- [ ] 更新 npm
- [ ] 更新 Electron
- [ ] 更新 Electron Builder
- [ ] 清理缓存
- [ ] 重新安装依赖

### 更新后
- [ ] 验证版本
- [ ] 测试构建
- [ ] 测试应用功能
- [ ] 更新文档
- [ ] 提交更改

---

## 🔗 有用链接

- [NPM 官方文档](https://docs.npmjs.com/)
- [Electron 官方文档](https://www.electronjs.org/docs)
- [Electron Builder 文档](https://www.electron.build/)
- [Node.js 下载页面](https://nodejs.org/)

---

## 📞 技术支持

如果遇到问题，可以：
1. 查看官方文档
2. 搜索 GitHub Issues
3. 检查 Stack Overflow
4. 联系项目维护者

---

*最后更新：2025年8月*
*文档版本：v1.0* 