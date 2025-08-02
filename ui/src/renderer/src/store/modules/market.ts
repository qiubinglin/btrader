export interface MarketData {
  symbol: string
  price: number
  volume: number
  timestamp: number
  side: 'buy' | 'sell'
}

export interface OrderBookLevel {
  price: number
  volume: number
}

export interface OrderBookData {
  symbol: string
  bids: OrderBookLevel[]
  asks: OrderBookLevel[]
  timestamp: number
}

export interface KlineData {
  timestamp: number
  open: number
  high: number
  low: number
  close: number
  volume: number
}

export interface MarketState {
  currentSymbol: string
  marketData: MarketData[]
  orderBook: OrderBookData | null
  klineData: KlineData[]
  tradeHistory: MarketData[]
  footprintData: any[]
  microOrderBook: OrderBookData | null
}

const state: MarketState = {
  currentSymbol: 'BTC/USDT',
  marketData: [],
  orderBook: null,
  klineData: [],
  tradeHistory: [],
  footprintData: [],
  microOrderBook: null
}

const mutations = {
  SET_CURRENT_SYMBOL(state: MarketState, symbol: string) {
    state.currentSymbol = symbol
  },
  
  ADD_MARKET_DATA(state: MarketState, data: MarketData) {
    state.marketData.push(data)
    if (state.marketData.length > 1000) {
      state.marketData.shift()
    }
  },
  
  UPDATE_ORDER_BOOK(state: MarketState, orderBook: OrderBookData) {
    state.orderBook = orderBook
  },
  
  ADD_KLINE_DATA(state: MarketState, kline: KlineData) {
    const existingIndex = state.klineData.findIndex(k => k.timestamp === kline.timestamp)
    if (existingIndex !== -1) {
      state.klineData[existingIndex] = kline
    } else {
      state.klineData.push(kline)
      if (state.klineData.length > 500) {
        state.klineData.shift()
      }
    }
  },
  
  ADD_TRADE_HISTORY(state: MarketState, trade: MarketData) {
    state.tradeHistory.unshift(trade)
    if (state.tradeHistory.length > 200) {
      state.tradeHistory.pop()
    }
  },
  
  SET_TRADE_HISTORY(state: MarketState, trades: MarketData[]) {
    state.tradeHistory = trades
  },
  
  UPDATE_FOOTPRINT_DATA(state: MarketState, data: any) {
    state.footprintData = data
  },
  
  UPDATE_MICRO_ORDER_BOOK(state: MarketState, microBook: OrderBookData) {
    state.microOrderBook = microBook
  }
}

const actions = {
  updateMarketData({ commit }: any, data: any) {
    if (data.type === 'tick') {
      commit('ADD_MARKET_DATA', data)
      commit('ADD_TRADE_HISTORY', data)
    } else if (data.type === 'orderbook') {
      commit('UPDATE_ORDER_BOOK', data)
    } else if (data.type === 'kline') {
      commit('ADD_KLINE_DATA', data)
    } else if (data.type === 'footprint') {
      commit('UPDATE_FOOTPRINT_DATA', data)
    } else if (data.type === 'micro_orderbook') {
      commit('UPDATE_MICRO_ORDER_BOOK', data)
    }
  },
  
  updateTradeData({ commit }: any, data: MarketData) {
    commit('ADD_TRADE_HISTORY', data)
  },
  
  setCurrentSymbol({ commit }: any, symbol: string) {
    commit('SET_CURRENT_SYMBOL', symbol)
  }
}

const getters = {
  currentPrice: (state: MarketState) => {
    return state.marketData.length > 0 ? state.marketData[state.marketData.length - 1].price : 0
  },
  
  priceChange: (state: MarketState) => {
    if (state.klineData.length < 2) return 0
    const current = state.klineData[state.klineData.length - 1]
    const previous = state.klineData[state.klineData.length - 2]
    return current.close - previous.close
  },
  
  priceChangePercent: (state: MarketState, getters: any) => {
    if (state.klineData.length < 2) return 0
    const previous = state.klineData[state.klineData.length - 2]
    return previous.close > 0 ? (getters.priceChange / previous.close) * 100 : 0
  }
}

export default {
  namespaced: true,
  state,
  mutations,
  actions,
  getters
}