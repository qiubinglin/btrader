#!/usr/bin/env node

const { spawn } = require('child_process')
const path = require('path')

// 使用electron命令启动应用
const electronPath = path.join(__dirname, 'node_modules', '.bin', 'electron')
const appPath = path.join(__dirname, 'out', 'main', 'index.js')

console.log('🚀 启动Electron应用...')
console.log('Electron路径:', electronPath)
console.log('应用路径:', appPath)

const electronProcess = spawn(electronPath, [appPath], {
  stdio: 'inherit',
  env: {
    ...process.env,
    NODE_ENV: 'development'
  }
})

electronProcess.on('close', (code) => {
  console.log(`✅ Electron应用已关闭，退出码: ${code}`)
})

electronProcess.on('error', (err) => {
  console.error('❌ 启动Electron应用失败:', err)
  process.exit(1)
}) 