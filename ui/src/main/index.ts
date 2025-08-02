import { app, shell, BrowserWindow, ipcMain } from 'electron'
import { join } from 'path'
import { electronApp, optimizer, is } from '@electron-toolkit/utils'
import icon from '../../resources/icon.png?asset'
import { WebSocketBridge } from './websocket-bridge'

let mainWindow: BrowserWindow
let webSocketBridge: WebSocketBridge

function createWindow(): void {
  // 创建浏览器窗口
  mainWindow = new BrowserWindow({
    width: 1920,
    height: 1080,
    show: false,
    autoHideMenuBar: true,
    ...(process.platform === 'linux' ? { icon } : {}),
    webPreferences: {
      preload: join(__dirname, '../preload/index.js'),
      sandbox: false,
      contextIsolation: true,
      enableRemoteModule: false,
      nodeIntegration: false
    }
  })

  mainWindow.on('ready-to-show', () => {
    mainWindow.show()
  })

  mainWindow.webContents.setWindowOpenHandler((details) => {
    shell.openExternal(details.url)
    return { action: 'deny' }
  })

  // HMR for renderer base on electron-vite cli.
  // Load the remote URL for development or the local html file for production.
  if (is.dev && process.env['ELECTRON_RENDERER_URL']) {
    mainWindow.loadURL(process.env['ELECTRON_RENDERER_URL'])
  } else {
    mainWindow.loadFile(join(__dirname, '../renderer/index.html'))
  }
}

// 这个方法将在 Electron 完成初始化和创建浏览器窗口时调用
app.whenReady().then(() => {
  // 为 Windows 设置应用用户模型ID
  electronApp.setAppUserModelId('com.btrader.ui')

  // 开发时默认按F12打开或关闭DevTools
  // 生产时忽略CommandOrControl + R
  app.on('browser-window-created', (_, window) => {
    optimizer.watchWindowShortcuts(window)
  })

  // IPC 测试
  ipcMain.on('ping', () => console.log('pong'))

  createWindow()

  // 初始化 WebSocket 桥接
  webSocketBridge = new WebSocketBridge(mainWindow)
  webSocketBridge.initialize()

  app.on('activate', function () {
    // 在macOS系统，当点击dock图标并且没有其他窗口打开的时候，
    // 通常在应用程序中重新创建一个窗口。
    if (BrowserWindow.getAllWindows().length === 0) createWindow()
  })
})

// 除了 macOS 外，当所有窗口都被关闭的时候退出程序。 因此，通常对程序和它们在
// 任务栏上的图标来说，应当保持活跃状态，直到用户使用 Cmd + Q 退出。
app.on('window-all-closed', () => {
  if (webSocketBridge) {
    webSocketBridge.destroy()
  }
  if (process.platform !== 'darwin') app.quit()
})

// 在这个文件中，你可以包含应用程序剩余的所有主进程代码。
// 也可以拆分成几个文件，然后用 require 导入。