<template>
  <div id="app" class="trading-app">
    <div class="app-header">
      <div class="header-left">
        <h1 class="app-title">BTrader 订单流交易系统</h1>
      </div>
      <div class="header-right">
        <div class="connection-status" :class="{ connected: isConnected }">
          <el-icon><Connection /></el-icon>
          {{ isConnected ? '已连接' : '未连接' }}
        </div>
      </div>
    </div>
    
    <div class="app-content">
      <!-- 左侧：订单流界面 -->
      <div class="left-panel">
        <div class="chart-section">
          <OrderFlowChart />
        </div>
        <div class="market-section">
          <el-tabs v-model="activeTab" class="market-tabs">
            <el-tab-pane label="成交明细" name="trades">
              <TradeList />
            </el-tab-pane>
            <el-tab-pane label="买卖档位" name="orderbook">
              <OrderBook />
            </el-tab-pane>
            <el-tab-pane label="足迹图" name="footprint">
              <FootprintChart />
            </el-tab-pane>
            <el-tab-pane label="微盘口" name="microbook">
              <MicroOrderBook />
            </el-tab-pane>
          </el-tabs>
        </div>
      </div>
      
      <!-- 右侧：交易面板和账户信息 -->
      <div class="right-panel">
        <div class="trading-section">
          <TradingPanel />
        </div>
        <div class="account-section">
          <el-tabs class="account-tabs">
            <el-tab-pane label="持仓状态" name="positions">
              <PositionList />
            </el-tab-pane>
            <el-tab-pane label="委托订单" name="orders">
              <OrderList />
            </el-tab-pane>
          </el-tabs>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, onMounted, onUnmounted } from 'vue'
import { useStore } from 'vuex'
import { Connection } from '@element-plus/icons-vue'
import OrderFlowChart from './components/OrderFlowChart.vue'
import TradeList from './components/TradeList.vue'
import OrderBook from './components/OrderBook.vue'
import FootprintChart from './components/FootprintChart.vue'
import MicroOrderBook from './components/MicroOrderBook.vue'
import TradingPanel from './components/TradingPanel.vue'
import PositionList from './components/PositionList.vue'
import OrderList from './components/OrderList.vue'

const store = useStore()
const activeTab = ref('trades')
const isConnected = ref(false)

let unsubscribeMarketData: (() => void) | null = null
let unsubscribeTradeData: (() => void) | null = null
let unsubscribePositionData: (() => void) | null = null
let unsubscribeOrderData: (() => void) | null = null

onMounted(() => {
  // 订阅市场数据
  unsubscribeMarketData = window.api.onMarketData((data) => {
    store.dispatch('updateMarketData', data)
    isConnected.value = true
  })
  
  // 订阅交易数据
  unsubscribeTradeData = window.api.onTradeData((data) => {
    store.dispatch('updateTradeData', data)
  })
  
  // 订阅持仓数据
  unsubscribePositionData = window.api.onPositionData((data) => {
    store.dispatch('updatePositionData', data)
  })
  
  // 订阅订单数据
  unsubscribeOrderData = window.api.onOrderData((data) => {
    store.dispatch('updateOrderData', data)
  })
})

onUnmounted(() => {
  if (unsubscribeMarketData) unsubscribeMarketData()
  if (unsubscribeTradeData) unsubscribeTradeData()
  if (unsubscribePositionData) unsubscribePositionData()
  if (unsubscribeOrderData) unsubscribeOrderData()
})
</script>

<style lang="scss" scoped>
.trading-app {
  height: 100vh;
  display: flex;
  flex-direction: column;
  background: $bg-primary;
  color: $text-primary;
}

.app-header {
  height: 50px;
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 0 20px;
  background: $bg-secondary;
  border-bottom: 1px solid $border-color;
  
  .app-title {
    margin: 0;
    font-size: 18px;
    font-weight: 600;
    color: $text-primary;
  }
  
  .connection-status {
    display: flex;
    align-items: center;
    gap: 8px;
    padding: 4px 12px;
    border-radius: 4px;
    background: rgba(244, 67, 54, 0.1);
    color: #f44336;
    
    &.connected {
      background: rgba(76, 175, 80, 0.1);
      color: #4caf50;
    }
  }
}

.app-content {
  flex: 1;
  display: flex;
  gap: 10px;
  padding: 10px;
  overflow: hidden;
}

.left-panel {
  flex: 2;
  display: flex;
  flex-direction: column;
  gap: 10px;
}

.chart-section {
  height: 60%;
  background: $bg-secondary;
  border-radius: 4px;
  border: 1px solid $border-color;
}

.market-section {
  flex: 1;
  background: $bg-secondary;
  border-radius: 4px;
  border: 1px solid $border-color;
  
  .market-tabs {
    height: 100%;
    
    :deep(.el-tabs__content) {
      height: calc(100% - 40px);
      overflow: hidden;
    }
  }
}

.right-panel {
  flex: 1;
  display: flex;
  flex-direction: column;
  gap: 10px;
}

.trading-section {
  height: 300px;
  background: $bg-secondary;
  border-radius: 4px;
  border: 1px solid $border-color;
}

.account-section {
  flex: 1;
  background: $bg-secondary;
  border-radius: 4px;
  border: 1px solid $border-color;
  
  .account-tabs {
    height: 100%;
    
    :deep(.el-tabs__content) {
      height: calc(100% - 40px);
      overflow: hidden;
    }
  }
}
</style>