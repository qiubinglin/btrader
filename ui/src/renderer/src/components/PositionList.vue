<template>
  <div class="position-list">
    <div class="position-header">
      <div class="header-title">Positions</div>
      <div class="header-summary">
        <span class="total-pnl" :class="totalPnlClass">
          Total PnL: {{ formatPnl(totalPnl) }}
        </span>
      </div>
      <div class="header-actions">
        <el-button size="small" @click="refreshPositions" :loading="isRefreshing">Refresh</el-button>
        <el-button size="small" @click="closeAllPositions" type="warning" v-if="hasOpenPositions">
          Close All
        </el-button>
      </div>
    </div>
    
    <div class="position-table-container">
      <el-table
        :data="positions"
        height="100%"
        size="small"
        :show-header="true"
        stripe
        empty-text="No Positions"
      >
        <el-table-column prop="symbol" label="Symbol" width="100" align="center">
          <template #default="{ row }">
            <span class="symbol-cell">{{ row.symbol }}</span>
          </template>
        </el-table-column>
        
        <el-table-column prop="volume" label="Volume" width="100" align="right">
          <template #default="{ row }">
            <span class="volume-cell" :class="getVolumeClass(row.volume)">
              {{ formatVolume(row.volume) }}
            </span>
          </template>
        </el-table-column>
        
        <el-table-column prop="avgPrice" label="Avg Price" width="100" align="right">
          <template #default="{ row }">
            <span class="price-cell">{{ formatPrice(row.avgPrice) }}</span>
          </template>
        </el-table-column>
        
        <el-table-column prop="currentPrice" label="Current Price" width="100" align="right">
          <template #default="{ row }">
            <span class="price-cell">{{ formatPrice(getCurrentPrice(row.symbol)) }}</span>
          </template>
        </el-table-column>
        
        <el-table-column prop="unrealizedPnl" label="Unrealized PnL" width="120" align="right">
          <template #default="{ row }">
            <span class="pnl-cell" :class="getPnlClass(row.unrealizedPnl)">
              {{ formatPnl(row.unrealizedPnl) }}
            </span>
          </template>
        </el-table-column>
        
        <el-table-column prop="realizedPnl" label="Realized PnL" width="120" align="right">
          <template #default="{ row }">
            <span class="pnl-cell" :class="getPnlClass(row.realizedPnl)">
              {{ formatPnl(row.realizedPnl) }}
            </span>
          </template>
        </el-table-column>
        
        <el-table-column prop="margin" label="Margin" width="100" align="right">
          <template #default="{ row }">
            <span class="margin-cell">{{ formatAmount(row.margin) }}</span>
          </template>
        </el-table-column>
        
        <el-table-column label="Actions" width="120" align="center" fixed="right">
          <template #default="{ row }">
            <div class="action-buttons">
              <el-button 
                size="small" 
                type="warning" 
                @click="closePosition(row)"
                :disabled="row.volume === 0"
              >
                Close
              </el-button>
              <el-button 
                size="small" 
                type="primary" 
                @click="addToPosition(row)"
              >
                Add
              </el-button>
            </div>
          </template>
        </el-table-column>
      </el-table>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue'
import { useStore } from 'vuex'
import { ElMessage, ElMessageBox } from 'element-plus'

const store = useStore()
const isRefreshing = ref(false)

const positions = computed(() => store.state.account.positions)
const totalPnl = computed(() => store.getters['account/totalPnl'] || 0)
const marketData = computed(() => store.state.market.marketData)

const totalPnlClass = computed(() => {
  if (totalPnl.value > 0) return 'text-up'
  if (totalPnl.value < 0) return 'text-down'
  return 'text-equal'
})

const hasOpenPositions = computed(() => {
  return positions.value.some(p => p.volume !== 0)
})

const formatVolume = (volume: number): string => {
  return volume.toFixed(4)
}

const formatPrice = (price: number): string => {
  return price.toFixed(2)
}

const formatAmount = (amount: number): string => {
  return amount.toFixed(2)
}

const formatPnl = (pnl: number): string => {
  const sign = pnl >= 0 ? '+' : ''
  return `${sign}${pnl.toFixed(2)}`
}

const getVolumeClass = (volume: number): string => {
  if (volume > 0) return 'text-up'
  if (volume < 0) return 'text-down'
  return 'text-equal'
}

const getPnlClass = (pnl: number): string => {
  if (pnl > 0) return 'text-up'
  if (pnl < 0) return 'text-down'
  return 'text-equal'
}

const getCurrentPrice = (symbol: string): number => {
  // 从市场数据中获取当前价格
  const latestData = marketData.value
    .filter(data => data.symbol === symbol)
    .pop()
  return latestData ? latestData.price : 0
}

const refreshPositions = async () => {
  isRefreshing.value = true
  try {
    await store.dispatch('account/refreshPositions')
    ElMessage.success('Positions data refreshed')
  } catch (error: any) {
    ElMessage.error(`Failed to refresh: ${error.message}`)
  } finally {
    isRefreshing.value = false
  }
}

const closePosition = async (position: any) => {
  if (position.volume === 0) return
  
  try {
    await ElMessageBox.confirm(
      `Confirm closing ${Math.abs(position.volume)} ${position.symbol}?`,
      'Confirm Close',
      {
        confirmButtonText: 'Confirm',
        cancelButtonText: 'Cancel',
        type: 'warning'
      }
    )
    
    // 设置交易面板参数并提交平仓订单
    const closeSide = position.volume > 0 ? 'sell' : 'buy'
    const closeVolume = Math.abs(position.volume)
    
    store.dispatch('trading/updateOrderForm', {
      symbol: position.symbol,
      side: closeSide,
      volume: closeVolume,
      orderType: 'market'
    })
    
    const result = await store.dispatch('trading/submitOrder')
    ElMessage.success(`Close order submitted successfully: ${result.orderId}`)
    
    // 刷新持仓数据
    await refreshPositions()
  } catch (error: any) {
    if (error !== 'cancel') {
      ElMessage.error(`Failed to close position: ${error.message}`)
    }
  }
}

const addToPosition = (position: any) => {
  // 设置交易面板参数，用于加仓
  const addSide = position.volume >= 0 ? 'buy' : 'sell'
  
  store.dispatch('trading/updateOrderForm', {
    symbol: position.symbol,
    side: addSide,
    orderType: 'limit',
    price: getCurrentPrice(position.symbol)
  })
  
  ElMessage.info('Add position parameters set, please adjust quantity in the trading panel and submit the order')
}

const closeAllPositions = async () => {
  const openPositions = positions.value.filter(p => p.volume !== 0)
  if (openPositions.length === 0) return
  
  try {
    await ElMessageBox.confirm(
      `Confirm closing all ${openPositions.length} positions?`,
      'Confirm All Close',
      {
        confirmButtonText: 'Confirm',
        cancelButtonText: 'Cancel',
        type: 'warning'
      }
    )
    
    // 逐个提交平仓订单
    for (const position of openPositions) {
      const closeSide = position.volume > 0 ? 'sell' : 'buy'
      const closeVolume = Math.abs(position.volume)
      
      try {
        store.dispatch('trading/updateOrderForm', {
          symbol: position.symbol,
          side: closeSide,
          volume: closeVolume,
          orderType: 'market'
        })
        
        await store.dispatch('trading/submitOrder')
      } catch (error) {
        console.error(`Failed to close ${position.symbol}:`, error)
      }
    }
    
    ElMessage.success('All close orders submitted')
    await refreshPositions()
  } catch (error: any) {
    if (error !== 'cancel') {
      ElMessage.error(`Failed to close all positions: ${error.message}`)
    }
  }
}
</script>

<style lang="scss" scoped>
.position-list {
  height: 100%;
  display: flex;
  flex-direction: column;
  background: $bg-secondary;
  border-radius: 4px;
  border: 1px solid $border-color;
  font-family: 'Times New Roman', Times, serif;
}

.position-header {
  height: 40px;
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 0 12px;
  border-bottom: 1px solid $border-color;
  background: $bg-tertiary;
  
  .header-title {
    font-weight: 600;
    color: $text-primary;
    font-family: 'Times New Roman', Times, serif;
  }
  
  .header-summary {
    .total-pnl {
      font-weight: bold;
      font-size: $font-size-sm;
      font-family: 'Times New Roman', Times, serif;
      
      &.text-up {
        color: $price-up;
      }
      
      &.text-down {
        color: $price-down;
      }
    }
  }
  
  .header-actions {
    display: flex;
    gap: 8px;
  }
}

.position-table-container {
  flex: 1;
  overflow: hidden;
}

.symbol-cell {
  font-weight: bold;
  font-family: 'Times New Roman', Times, serif;
}

.volume-cell {
  font-family: 'Courier New', monospace;
  
  &.text-up {
    color: $price-up;
  }
  
  &.text-down {
    color: $price-down;
  }
}

.price-cell {
  font-family: 'Courier New', monospace;
}

.pnl-cell {
  font-family: 'Courier New', monospace;
  font-weight: bold;
  
  &.text-up {
    color: $price-up;
  }
  
  &.text-down {
    color: $price-down;
  }
}

.margin-cell {
  font-family: 'Courier New', monospace;
}

.action-buttons {
  display: flex;
  gap: 4px;
  
  .el-button {
    font-size: $font-size-xs;
    font-family: 'Times New Roman', Times, serif;
  }
}
</style>