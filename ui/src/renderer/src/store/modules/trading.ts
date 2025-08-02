export interface OrderRequest {
  symbol: string
  side: 'buy' | 'sell'
  volume: number
  price: number
  orderType: 'limit' | 'market'
}

export interface TradingState {
  orderForm: OrderRequest
  isSubmitting: boolean
  lastOrderResult: any
}

const state: TradingState = {
  orderForm: {
    symbol: 'BTC/USDT',
    side: 'buy',
    volume: 0,
    price: 0,
    orderType: 'limit'
  },
  isSubmitting: false,
  lastOrderResult: null
}

const mutations = {
  UPDATE_ORDER_FORM(state: TradingState, formData: Partial<OrderRequest>) {
    state.orderForm = { ...state.orderForm, ...formData }
  },
  
  SET_SUBMITTING(state: TradingState, status: boolean) {
    state.isSubmitting = status
  },
  
  SET_ORDER_RESULT(state: TradingState, result: any) {
    state.lastOrderResult = result
  }
}

const actions = {
  async submitOrder({ commit, state }: any) {
    commit('SET_SUBMITTING', true)
    try {
      const result = await window.api.sendOrder(state.orderForm)
      commit('SET_ORDER_RESULT', result)
      return result
    } catch (error) {
      commit('SET_ORDER_RESULT', { error: error.message })
      throw error
    } finally {
      commit('SET_SUBMITTING', false)
    }
  },
  
  async cancelOrder({ commit }: any, orderId: string) {
    try {
      const result = await window.api.cancelOrder(orderId)
      return result
    } catch (error) {
      throw error
    }
  },
  
  updateOrderForm({ commit }: any, formData: Partial<OrderRequest>) {
    commit('UPDATE_ORDER_FORM', formData)
  }
}

export default {
  namespaced: true,
  state,
  mutations,
  actions
}