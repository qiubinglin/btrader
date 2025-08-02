import { BrowserWindow, ipcMain } from 'electron'
import * as nanomsg from 'nanomsg'

export interface MarketData {
  symbol: string
  price: number
  volume: number
  timestamp: number
  side: 'buy' | 'sell'
}

export interface OrderBookData {
  symbol: string
  bids: Array<{ price: number; volume: number }>
  asks: Array<{ price: number; volume: number }>
  timestamp: number
}

export interface PositionData {
  symbol: string
  volume: number
  avgPrice: number
  unrealizedPnl: number
  realizedPnl: number
}

export interface OrderData {
  orderId: string
  symbol: string
  side: 'buy' | 'sell'
  volume: number
  price: number
  status: 'pending' | 'filled' | 'cancelled' | 'partial'
  filledVolume: number
  timestamp: number
}

export class NanoMsgBridge {
  private marketDataSocket: any
  private commandSocket: any
  private mainWindow: BrowserWindow
  private isConnected = false

  constructor(mainWindow: BrowserWindow) {
    this.mainWindow = mainWindow
  }

  initialize(): void {
    this.setupIpcHandlers()
    this.connectSockets()
  }

  private setupIpcHandlers(): void {
    // 处理发送订单请求
    ipcMain.handle('send-order', async (_event, orderData) => {
      return this.sendOrder(orderData)
    })

    // 处理取消订单请求
    ipcMain.handle('cancel-order', async (_event, orderId) => {
      return this.cancelOrder(orderId)
    })

    // 处理查询持仓请求
    ipcMain.handle('query-position', async () => {
      return this.queryPosition()
    })

    // 处理查询订单请求
    ipcMain.handle('query-orders', async () => {
      return this.queryOrders()
    })
  }

  private connectSockets(): void {
    try {
      // 连接市场数据接收socket (SUB模式)
      this.marketDataSocket = nanomsg.socket('sub')
      this.marketDataSocket.connect('tcp://127.0.0.1:5555')
      this.marketDataSocket.setsockopt(nanomsg.SUB, nanomsg.SUB_SUBSCRIBE, '')

      // 连接命令发送socket (REQ模式)
      this.commandSocket = nanomsg.socket('req')
      this.commandSocket.connect('tcp://127.0.0.1:5556')

      this.isConnected = true
      console.log('NanoMsg sockets connected successfully')

      // 开始监听市场数据
      this.startMarketDataListener()

    } catch (error) {
      console.error('Failed to connect nanomsg sockets:', error)
      this.isConnected = false
    }
  }

  private startMarketDataListener(): void {
    if (!this.marketDataSocket) return

    this.marketDataSocket.on('data', (buffer: Buffer) => {
      try {
        const message = JSON.parse(buffer.toString())
        this.handleMarketDataMessage(message)
      } catch (error) {
        console.error('Failed to parse market data message:', error)
      }
    })
  }

  private handleMarketDataMessage(message: any): void {
    const { type, data } = message

    switch (type) {
      case 'market_data':
        this.mainWindow.webContents.send('market-data', data)
        break
      case 'trade_data':
        this.mainWindow.webContents.send('trade-data', data)
        break
      case 'position_data':
        this.mainWindow.webContents.send('position-data', data)
        break
      case 'order_data':
        this.mainWindow.webContents.send('order-data', data)
        break
      default:
        console.log('Unknown message type:', type)
    }
  }

  private async sendOrder(orderData: any): Promise<any> {
    if (!this.isConnected || !this.commandSocket) {
      throw new Error('Not connected to trading system')
    }

    const command = {
      type: 'place_order',
      data: {
        symbol: orderData.symbol,
        side: orderData.side,
        volume: orderData.volume,
        price: orderData.price,
        orderType: orderData.orderType || 'limit',
        timestamp: Date.now()
      }
    }

    return new Promise((resolve, reject) => {
      try {
        this.commandSocket.send(JSON.stringify(command))
        
        this.commandSocket.on('data', (buffer: Buffer) => {
          try {
            const response = JSON.parse(buffer.toString())
            if (response.success) {
              resolve(response.data)
            } else {
              reject(new Error(response.error))
            }
          } catch (error) {
            reject(error)
          }
        })
      } catch (error) {
        reject(error)
      }
    })
  }

  private async cancelOrder(orderId: string): Promise<any> {
    if (!this.isConnected || !this.commandSocket) {
      throw new Error('Not connected to trading system')
    }

    const command = {
      type: 'cancel_order',
      data: {
        orderId,
        timestamp: Date.now()
      }
    }

    return new Promise((resolve, reject) => {
      try {
        this.commandSocket.send(JSON.stringify(command))
        
        this.commandSocket.on('data', (buffer: Buffer) => {
          try {
            const response = JSON.parse(buffer.toString())
            if (response.success) {
              resolve(response.data)
            } else {
              reject(new Error(response.error))
            }
          } catch (error) {
            reject(error)
          }
        })
      } catch (error) {
        reject(error)
      }
    })
  }

  private async queryPosition(): Promise<any> {
    if (!this.isConnected || !this.commandSocket) {
      throw new Error('Not connected to trading system')
    }

    const command = {
      type: 'query_position',
      data: {
        timestamp: Date.now()
      }
    }

    return new Promise((resolve, reject) => {
      try {
        this.commandSocket.send(JSON.stringify(command))
        
        this.commandSocket.on('data', (buffer: Buffer) => {
          try {
            const response = JSON.parse(buffer.toString())
            if (response.success) {
              resolve(response.data)
            } else {
              reject(new Error(response.error))
            }
          } catch (error) {
            reject(error)
          }
        })
      } catch (error) {
        reject(error)
      }
    })
  }

  private async queryOrders(): Promise<any> {
    if (!this.isConnected || !this.commandSocket) {
      throw new Error('Not connected to trading system')
    }

    const command = {
      type: 'query_orders',
      data: {
        timestamp: Date.now()
      }
    }

    return new Promise((resolve, reject) => {
      try {
        this.commandSocket.send(JSON.stringify(command))
        
        this.commandSocket.on('data', (buffer: Buffer) => {
          try {
            const response = JSON.parse(buffer.toString())
            if (response.success) {
              resolve(response.data)
            } else {
              reject(new Error(response.error))
            }
          } catch (error) {
            reject(error)
          }
        })
      } catch (error) {
        reject(error)
      }
    })
  }

  destroy(): void {
    if (this.marketDataSocket) {
      this.marketDataSocket.close()
    }
    if (this.commandSocket) {
      this.commandSocket.close()
    }
    this.isConnected = false
  }
}