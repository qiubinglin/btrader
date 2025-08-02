import { contextBridge, ipcRenderer } from 'electron'
import { electronAPI } from '@electron-toolkit/preload'

// 自定义 API 用于渲染器
const api = {
  // 订阅市场数据
  onMarketData: (callback: (data: any) => void) => {
    ipcRenderer.on('market-data', (_event, data) => callback(data))
    return () => ipcRenderer.removeAllListeners('market-data')
  },
  
  // 订阅交易数据
  onTradeData: (callback: (data: any) => void) => {
    ipcRenderer.on('trade-data', (_event, data) => callback(data))
    return () => ipcRenderer.removeAllListeners('trade-data')
  },
  
  // 订阅持仓数据
  onPositionData: (callback: (data: any) => void) => {
    ipcRenderer.on('position-data', (_event, data) => callback(data))
    return () => ipcRenderer.removeAllListeners('position-data')
  },
  
  // 订阅订单数据
  onOrderData: (callback: (data: any) => void) => {
    ipcRenderer.on('order-data', (_event, data) => callback(data))
    return () => ipcRenderer.removeAllListeners('order-data')
  },
  
  // 发送交易指令
  sendOrder: (orderData: any) => {
    ipcRenderer.invoke('send-order', orderData)
  },
  
  // 取消订单
  cancelOrder: (orderId: string) => {
    ipcRenderer.invoke('cancel-order', orderId)
  },
  
  // 查询持仓
  queryPosition: () => {
    return ipcRenderer.invoke('query-position')
  },
  
  // 查询订单
  queryOrders: () => {
    return ipcRenderer.invoke('query-orders')
  }
}

// 使用 `contextBridge` API 将 Electron API 暴露给渲染器
// 只有在上下文隔离启用时才可用，否则直接添加到 DOM 全局。
if (process.contextIsolated) {
  try {
    contextBridge.exposeInMainWorld('electron', electronAPI)
    contextBridge.exposeInMainWorld('api', api)
  } catch (error) {
    console.error(error)
  }
} else {
  // @ts-ignore (define in dts)
  window.electron = electronAPI
  // @ts-ignore (define in dts)
  window.api = api
}