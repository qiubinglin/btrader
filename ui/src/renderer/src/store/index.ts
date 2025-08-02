import { createStore } from 'vuex'
import marketModule from './modules/market'
import tradingModule from './modules/trading'
import accountModule from './modules/account'

export default createStore({
  modules: {
    market: marketModule,
    trading: tradingModule,
    account: accountModule
  }
})