import { ElectronAPI } from '@electron-toolkit/preload'

declare global {
  interface Window {
    electron: ElectronAPI
    api: {
      onMarketData: (callback: (data: any) => void) => () => void
      onTradeData: (callback: (data: any) => void) => () => void
      onPositionData: (callback: (data: any) => void) => () => void
      onOrderData: (callback: (data: any) => void) => () => void
      sendOrder: (orderData: any) => void
      cancelOrder: (orderId: string) => void
      queryPosition: () => Promise<any>
      queryOrders: () => Promise<any>
    }
  }
}