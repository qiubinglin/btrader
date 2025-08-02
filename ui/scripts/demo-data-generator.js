/**
 * 演示数据生成器
 * 模拟C++交易系统发送的WebSocket消息，用于UI测试
 */

const WebSocket = require('ws')

class DemoDataGenerator {
  constructor() {
    this.isRunning = false
    this.marketDataServer = null
    this.tradingServer = null
    this.marketClients = new Set()
    this.tradingClients = new Set()
    this.currentPrice = 50000
    this.symbols = ['BTC/USDT', 'ETH/USDT', 'BNB/USDT']
    this.currentSymbol = 'BTC/USDT'
  }

  start() {
    try {
      // 创建市场数据WebSocket服务器
      this.marketDataServer = new WebSocket.Server({ port: 8082, path: '/market' })
      this.marketDataServer.on('connection', (ws) => {
        console.log('Market data client connected')
        this.marketClients.add(ws)
        
        ws.on('close', () => {
          console.log('Market data client disconnected')
          this.marketClients.delete(ws)
        })
      })
      
      // 创建交易WebSocket服务器
      this.tradingServer = new WebSocket.Server({ port: 8083, path: '/trading' })
      this.tradingServer.on('connection', (ws) => {
        console.log('Trading client connected')
        this.tradingClients.add(ws)
        
        ws.on('message', (data) => {
          try {
            const message = JSON.parse(data.toString())
            const response = this.handleTradingRequest(message)
            ws.send(JSON.stringify(response))
          } catch (error) {
            console.error('处理交易请求失败:', error)
            ws.send(JSON.stringify({
              success: false,
              error: error.message,
              id: message?.data?.timestamp
            }))
          }
        })
        
        ws.on('close', () => {
          console.log('Trading client disconnected')
          this.tradingClients.delete(ws)
        })
      })
      
      console.log('演示数据生成器已启动')
      console.log('市场数据端口: 8082/market')
      console.log('交易接口端口: 8083/trading')
      
      this.isRunning = true
      this.startMarketDataGeneration()
      
    } catch (error) {
      console.error('启动演示数据生成器失败:', error)
    }
  }

  stop() {
    this.isRunning = false
    if (this.marketDataServer) {
      this.marketDataServer.close()
    }
    if (this.tradingServer) {
      this.tradingServer.close()
    }
    console.log('演示数据生成器已停止')
  }

  startMarketDataGeneration() {
    // 生成K线数据
    setInterval(() => {
      if (!this.isRunning) return
      this.generateKlineData()
    }, 1000)

    // 生成Tick数据
    setInterval(() => {
      if (!this.isRunning) return
      this.generateTickData()
    }, 200)

    // 生成订单簿数据
    setInterval(() => {
      if (!this.isRunning) return
      this.generateOrderBookData()
    }, 500)

    // 生成足迹图数据
    setInterval(() => {
      if (!this.isRunning) return
      this.generateFootprintData()
    }, 2000)
  }

  generateKlineData() {
    const now = Date.now()
    const timestamp = Math.floor(now / 60000) * 60000 // 1分钟K线
    
    const volatility = 100
    const change = (Math.random() - 0.5) * volatility
    this.currentPrice = Math.max(this.currentPrice + change, 1000)
    
    const open = this.currentPrice - change
    const high = Math.max(open, this.currentPrice) + Math.random() * 50
    const low = Math.min(open, this.currentPrice) - Math.random() * 50
    const volume = Math.random() * 100 + 10

    const klineData = {
      type: 'kline',
      data: {
        symbol: this.currentSymbol,
        timestamp,
        open,
        high,
        low,
        close: this.currentPrice,
        volume
      }
    }

    this.broadcastToMarketClients(klineData)
  }

  generateTickData() {
    const side = Math.random() > 0.5 ? 'buy' : 'sell'
    const volume = Math.random() * 5 + 0.1
    const priceChange = (Math.random() - 0.5) * 10
    
    const tickData = {
      type: 'tick',
      data: {
        symbol: this.currentSymbol,
        price: this.currentPrice + priceChange,
        volume,
        timestamp: Date.now(),
        side
      }
    }

    this.broadcastToMarketClients(tickData)
  }

  generateOrderBookData() {
    const levels = 10
    const bids = []
    const asks = []

    for (let i = 0; i < levels; i++) {
      bids.push({
        price: this.currentPrice - (i + 1) * 0.5,
        volume: Math.random() * 50 + 5
      })
      
      asks.push({
        price: this.currentPrice + (i + 1) * 0.5,
        volume: Math.random() * 50 + 5
      })
    }

    const orderBookData = {
      type: 'orderbook',
      data: {
        symbol: this.currentSymbol,
        bids,
        asks,
        timestamp: Date.now()
      }
    }

    this.broadcastToMarketClients(orderBookData)
    
    // 同时发送微盘口数据
    const microOrderBookData = {
      type: 'micro_orderbook',
      data: {
        symbol: this.currentSymbol,
        bids: bids.slice(0, 1),
        asks: asks.slice(0, 1),
        timestamp: Date.now()
      }
    }

    this.broadcastToMarketClients(microOrderBookData)
  }

  generateFootprintData() {
    const footprintData = {
      type: 'footprint',
      data: []
    }

    // 生成足迹图的模拟交易数据
    for (let i = 0; i < 100; i++) {
      footprintData.data.push({
        price: this.currentPrice + (Math.random() - 0.5) * 100,
        volume: Math.random() * 10,
        side: Math.random() > 0.5 ? 'buy' : 'sell',
        timestamp: Date.now() - Math.random() * 60000
      })
    }

    this.broadcastToMarketClients(footprintData)
  }

  broadcastToMarketClients(data) {
    const message = JSON.stringify(data)
    for (const client of this.marketClients) {
      if (client.readyState === WebSocket.OPEN) {
        client.send(message)
      }
    }
  }

  handleTradingRequest(message) {
    const { type, data } = message

    switch (type) {
      case 'place_order':
        return this.handlePlaceOrder(data)
      case 'cancel_order':
        return this.handleCancelOrder(data)
      case 'query_position':
        return this.handleQueryPosition(data)
      case 'query_orders':
        return this.handleQueryOrders(data)
      default:
        throw new Error(`未知的请求类型: ${type}`)
    }
  }

  handlePlaceOrder(data) {
    const orderId = 'ORDER_' + Date.now() + '_' + Math.random().toString(36).substr(2, 5)
    
    console.log(`下单请求: ${data.side} ${data.volume} ${data.symbol} @ ${data.price}`)
    
    // 模拟订单确认
    setTimeout(() => {
      const orderData = {
        type: 'order_data',
        data: {
          orderId,
          symbol: data.symbol,
          side: data.side,
          volume: data.volume,
          price: data.price,
          status: 'pending',
          filledVolume: 0,
          timestamp: Date.now()
        }
      }
      this.broadcastToMarketClients(orderData)
    }, 100)

    return {
      success: true,
      data: { orderId },
      id: data.timestamp
    }
  }

  handleCancelOrder(data) {
    console.log(`撤单请求: ${data.orderId}`)
    
    return {
      success: true,
      data: { orderId: data.orderId },
      id: data.timestamp
    }
  }

  handleQueryPosition(data) {
    const positions = [
      {
        symbol: 'BTC/USDT',
        volume: 0.5,
        avgPrice: 49000,
        unrealizedPnl: 500,
        realizedPnl: 100,
        margin: 2450
      },
      {
        symbol: 'ETH/USDT',
        volume: -2.0,
        avgPrice: 3200,
        unrealizedPnl: -150,
        realizedPnl: 50,
        margin: 640
      }
    ]

    return {
      success: true,
      data: positions,
      id: data.timestamp
    }
  }

  handleQueryOrders(data) {
    const orders = [
      {
        orderId: 'ORDER_12345',
        symbol: 'BTC/USDT',
        side: 'buy',
        volume: 0.1,
        price: 49500,
        status: 'pending',
        filledVolume: 0,
        timestamp: Date.now() - 60000
      }
    ]

    return {
      success: true,
      data: orders,
      id: data.timestamp
    }
  }
}

// 启动演示数据生成器
const generator = new DemoDataGenerator()
generator.start()

// 优雅退出
process.on('SIGINT', () => {
  console.log('\n正在停止演示数据生成器...')
  generator.stop()
  process.exit(0)
})

process.on('SIGTERM', () => {
  generator.stop()
  process.exit(0)
})