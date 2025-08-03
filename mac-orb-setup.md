# Setup
1. Mac 安装 XQuartz
2. 打开 XQuartz 后，点击菜单栏的 XQuartz > Preferences > Security，确保勾选了 “Allow connections from network clients”
3. Mac 终端运行 `xhost + 127.0.0.1`
4. orb 添加环境变量 `export DISPLAY=host.docker.internal:0`