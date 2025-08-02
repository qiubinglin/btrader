export interface Position {
  symbol: string
  volume: number
  avgPrice: number
  unrealizedPnl: number
  realizedPnl: number
  margin: number
}

export interface Order {
  orderId: string
  symbol: string
  side: 'buy' | 'sell'
  volume: number
  price: number
  status: 'pending' | 'filled' | 'cancelled' | 'partial'
  filledVolume: number
  timestamp: number
}

export interface AccountState {
  positions: Position[]
  orders: Order[]
  totalBalance: number
  availableBalance: number
  totalPnl: number
}

const state: AccountState = {
  positions: [],
  orders: [],
  totalBalance: 0,
  availableBalance: 0,
  totalPnl: 0
}

const mutations = {
  SET_POSITIONS(state: AccountState, positions: Position[]) {
    state.positions = positions
  },
  
  UPDATE_POSITION(state: AccountState, position: Position) {
    const index = state.positions.findIndex(p => p.symbol === position.symbol)
    if (index !== -1) {
      state.positions[index] = position
    } else {
      state.positions.push(position)
    }
  },
  
  SET_ORDERS(state: AccountState, orders: Order[]) {
    state.orders = orders
  },
  
  UPDATE_ORDER(state: AccountState, order: Order) {
    const index = state.orders.findIndex(o => o.orderId === order.orderId)
    if (index !== -1) {
      state.orders[index] = order
    } else {
      state.orders.push(order)
    }
  },
  
  REMOVE_ORDER(state: AccountState, orderId: string) {
    state.orders = state.orders.filter(o => o.orderId !== orderId)
  },
  
  SET_BALANCE(state: AccountState, balance: { total: number; available: number }) {
    state.totalBalance = balance.total
    state.availableBalance = balance.available
  },
  
  SET_TOTAL_PNL(state: AccountState, pnl: number) {
    state.totalPnl = pnl
  }
}

const actions = {
  updatePositionData({ commit }: any, data: any) {
    if (Array.isArray(data)) {
      commit('SET_POSITIONS', data)
    } else {
      commit('UPDATE_POSITION', data)
    }
    
    // 计算总盈亏
    const totalPnl = data.reduce((sum: number, pos: Position) => 
      sum + pos.unrealizedPnl + pos.realizedPnl, 0)
    commit('SET_TOTAL_PNL', totalPnl)
  },
  
  updateOrderData({ commit }: any, data: any) {
    if (Array.isArray(data)) {
      commit('SET_ORDERS', data)
    } else if (data.orderId) {
      if (data.status === 'cancelled' || data.status === 'filled') {
        commit('REMOVE_ORDER', data.orderId)
      } else {
        commit('UPDATE_ORDER', data)
      }
    }
  },
  
  async refreshPositions({ commit }: any) {
    try {
      const positions = await window.api.queryPosition()
      commit('SET_POSITIONS', positions)
    } catch (error) {
      console.error('Failed to refresh positions:', error)
    }
  },
  
  async refreshOrders({ commit }: any) {
    try {
      const orders = await window.api.queryOrders()
      commit('SET_ORDERS', orders)
    } catch (error) {
      console.error('Failed to refresh orders:', error)
    }
  }
}

const getters = {
  totalPositionValue: (state: AccountState) => {
    return state.positions.reduce((sum, pos) => sum + pos.volume * pos.avgPrice, 0)
  },
  
  activeOrders: (state: AccountState) => {
    return state.orders.filter(order => 
      order.status === 'pending' || order.status === 'partial')
  },
  
  completedOrders: (state: AccountState) => {
    return state.orders.filter(order => 
      order.status === 'filled' || order.status === 'cancelled')
  }
}

export default {
  namespaced: true,
  state,
  mutations,
  actions,
  getters
}