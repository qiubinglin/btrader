# 🚀 Node.js 版本升级指南

## 🔴 当前问题
- **当前版本**: Node.js v12.22.9
- **需要版本**: Node.js 18+ 或 20+
- **影响**: 无法安装现代JavaScript依赖包

## ✅ 升级方案（选择其中一种）

### 方案1: 使用 NodeSource 官方源 (推荐)

```bash
# 下载并运行Node.js 20.x安装脚本
curl -fsSL https://deb.nodesource.com/setup_20.x | sudo -E bash -

# 安装Node.js 20.x
sudo apt-get install -y nodejs

# 验证安装
node --version
npm --version
```

### 方案2: 使用 nvm (Node Version Manager)

```bash
# 安装nvm
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.3/install.sh | bash

# 重新加载终端配置
source ~/.bashrc

# 安装并使用Node.js 20
nvm install 20
nvm use 20
nvm alias default 20

# 验证安装
node --version
```

### 方案3: 使用 Snap (如果可用)

```bash
# 安装Node.js 20
sudo snap install node --classic --channel=20/stable

# 验证安装
node --version
```

## 🎯 升级后步骤

升级Node.js后，清理并重新安装依赖：

```bash
cd ~/btrader/ui

# 清理旧的node_modules和缓存
rm -rf node_modules package-lock.json
npm cache clean --force

# 重新安装依赖
npm install

# 运行演示
./start-demo.sh
```

## 🔧 验证升级成功

升级后应该看到：
```bash
$ node --version
v20.x.x  # 或 v18.x.x

$ npm --version
10.x.x   # 或更新版本
```

## ⚠️ 注意事项

1. **权限**: 某些方法需要sudo权限
2. **路径**: 升级后可能需要重新打开终端
3. **兼容性**: Node.js 20是LTS版本，稳定可靠

选择最适合您环境的升级方案！