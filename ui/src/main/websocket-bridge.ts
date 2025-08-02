import { BrowserWindow, ipcMain } from 'electron'
import WebSocket from 'ws'

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

export class WebSocketBridge {
  private marketDataWs: WebSocket | null = null
  private commandWs: WebSocket | null = null
  private mainWindow: BrowserWindow
  private isConnected = false

  constructor(mainWindow: BrowserWindow) {
    this.mainWindow = mainWindow
  }

  initialize(): void {
    this.setupIpcHandlers()
    this.connectWebSockets()
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

  private connectWebSockets(): void {
    try {
      // 连接市场数据WebSocket
      this.marketDataWs = new WebSocket('ws://127.0.0.1:8082/market')
      this.marketDataWs.on('open', () => {
        console.log('Market data WebSocket connected')
        this.checkConnectionStatus()
      })
      
      this.marketDataWs.on('message', (data: WebSocket.Data) => {
        try {
          const message = JSON.parse(data.toString())
          this.handleMarketDataMessage(message)
        } catch (error) {
          console.error('Failed to parse market data message:', error)
        }
      })

      this.marketDataWs.on('error', (error) => {
        console.error('Market data WebSocket error:', error)
        this.isConnected = false
      })

      // 连接交易指令WebSocket
      this.commandWs = new WebSocket('ws://127.0.0.1:8083/trading')
      this.commandWs.on('open', () => {
        console.log('Trading command WebSocket connected')
        this.checkConnectionStatus()
      })

      this.commandWs.on('error', (error) => {
        console.error('Trading command WebSocket error:', error)
        this.isConnected = false
      })

    } catch (error) {
      console.error('Failed to connect WebSockets:', error)
      this.isConnected = false
    }
  }

  private checkConnectionStatus(): void {
    const marketConnected = this.marketDataWs?.readyState === WebSocket.OPEN
    const tradingConnected = this.commandWs?.readyState === WebSocket.OPEN
    this.isConnected = marketConnected && tradingConnected
    
    if (this.isConnected) {
      console.log('All WebSocket connections established successfully')
    }
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
    if (!this.isConnected || !this.commandWs) {
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
        this.commandWs!.send(JSON.stringify(command))
        
        // 等待响应
        const responseHandler = (data: WebSocket.Data) => {
          try {
            const response = JSON.parse(data.toString())
            if (response.id === command.data.timestamp) {
              this.commandWs!.removeListener('message', responseHandler)
              if (response.success) {
                resolve(response.data)
              } else {
                reject(new Error(response.error))
              }
            }
          } catch (error) {
            reject(error)
          }
        }
        
        this.commandWs!.on('message', responseHandler)
        
        // 超时处理
        setTimeout(() => {
          this.commandWs!.removeListener('message', responseHandler)
          reject(new Error('Request timeout'))
        }, 5000)
        
      } catch (error) {
        reject(error)
      }
    })
  }

  private async cancelOrder(orderId: string): Promise<any> {
    if (!this.isConnected || !this.commandWs) {
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
        this.commandWs!.send(JSON.stringify(command))
        
        const responseHandler = (data: WebSocket.Data) => {
          try {
            const response = JSON.parse(data.toString())
            if (response.id === command.data.timestamp) {
              this.commandWs!.removeListener('message', responseHandler)
              if (response.success) {
                resolve(response.data)
              } else {
                reject(new Error(response.error))
              }
            }
          } catch (error) {
            reject(error)
          }
        }
        
        this.commandWs!.on('message', responseHandler)
        
        setTimeout(() => {
          this.commandWs!.removeListener('message', responseHandler)
          reject(new Error('Request timeout'))
        }, 5000)
        
      } catch (error) {
        reject(error)
      }
    })
  }

  private async queryPosition(): Promise<any> {
    if (!this.isConnected || !this.commandWs) {
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
        this.commandWs!.send(JSON.stringify(command))
        
        const responseHandler = (data: WebSocket.Data) => {
          try {
            const response = JSON.parse(data.toString())
            if (response.id === command.data.timestamp) {
              this.commandWs!.removeListener('message', responseHandler)
              if (response.success) {
                resolve(response.data)
              } else {
                reject(new Error(response.error))
              }
            }
          } catch (error) {
            reject(error)
          }
        }
        
        this.commandWs!.on('message', responseHandler)
        
        setTimeout(() => {
          this.commandWs!.removeListener('message', responseHandler)
          reject(new Error('Request timeout'))
        }, 5000)
        
      } catch (error) {
        reject(error)
      }
    })
  }

  private async queryOrders(): Promise<any> {
    if (!this.isConnected || !this.commandWs) {
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
        this.commandWs!.send(JSON.stringify(command))
        
        const responseHandler = (data: WebSocket.Data) => {
          try {
            const response = JSON.parse(data.toString())
            if (response.id === command.data.timestamp) {
              this.commandWs!.removeListener('message', responseHandler)
              if (response.success) {
                resolve(response.data)
              } else {
                reject(new Error(response.error))
              }
            }
          } catch (error) {
            reject(error)
          }
        }
        
        this.commandWs!.on('message', responseHandler)
        
        setTimeout(() => {
          this.commandWs!.removeListener('message', responseHandler)
          reject(new Error('Request timeout'))
        }, 5000)
        
      } catch (error) {
        reject(error)
      }
    })
  }

  destroy(): void {
    if (this.marketDataWs) {
      this.marketDataWs.close()
    }
    if (this.commandWs) {
      this.commandWs.close()
    }
    this.isConnected = false
  }
}