<template>
  <div class="position-list">
    <div class="position-header">
      <div class="header-title">持仓状态</div>
      <div class="header-summary">
        <span class="total-pnl" :class="totalPnlClass">
          总盈亏: {{ formatPnl(totalPnl) }}
        </span>
      </div>
      <div class="header-actions">
        <el-button size="small" @click="refreshPositions" :loading="isRefreshing">刷新</el-button>
        <el-button size="small" @click="closeAllPositions" type="warning" v-if="hasOpenPositions">
          全部平仓
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
        empty-text="暂无持仓"
      >
        <el-table-column prop="symbol" label="品种" width="100" align="center">
          <template #default="{ row }">
            <span class="symbol-cell">{{ row.symbol }}</span>
          </template>
        </el-table-column>
        
        <el-table-column prop="volume" label="数量" width="100" align="right">
          <template #default="{ row }">
            <span class="volume-cell" :class="getVolumeClass(row.volume)">
              {{ formatVolume(row.volume) }}
            </span>
          </template>
        </el-table-column>
        
        <el-table-column prop="avgPrice" label="均价" width="100" align="right">
          <template #default="{ row }">
            <span class="price-cell">{{ formatPrice(row.avgPrice) }}</span>
          </template>
        </el-table-column>
        
        <el-table-column prop="currentPrice" label="现价" width="100" align="right">
          <template #default="{ row }">
            <span class="price-cell">{{ formatPrice(getCurrentPrice(row.symbol)) }}</span>
          </template>
        </el-table-column>
        
        <el-table-column prop="unrealizedPnl" label="浮动盈亏" width="120" align="right">
          <template #default="{ row }">
            <span class="pnl-cell" :class="getPnlClass(row.unrealizedPnl)">
              {{ formatPnl(row.unrealizedPnl) }}
            </span>
          </template>
        </el-table-column>
        
        <el-table-column prop="realizedPnl" label="已实现盈亏" width="120" align="right">
          <template #default="{ row }">
            <span class="pnl-cell" :class="getPnlClass(row.realizedPnl)">
              {{ formatPnl(row.realizedPnl) }}
            </span>
          </template>
        </el-table-column>
        
        <el-table-column prop="margin" label="保证金" width="100" align="right">
          <template #default="{ row }">
            <span class="margin-cell">{{ formatAmount(row.margin) }}</span>
          </template>
        </el-table-column>
        
        <el-table-column label="操作" width="120" align="center" fixed="right">
          <template #default="{ row }">
            <div class="action-buttons">
              <el-button 
                size="small" 
                type="warning" 
                @click="closePosition(row)"
                :disabled="row.volume === 0"
              >
                平仓
              </el-button>
              <el-button 
                size="small" 
                type="primary" 
                @click="addToPosition(row)"
              >
                加仓
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
    ElMessage.success('持仓数据已刷新')
  } catch (error: any) {
    ElMessage.error(`刷新失败: ${error.message}`)
  } finally {
    isRefreshing.value = false
  }
}

const closePosition = async (position: any) => {
  if (position.volume === 0) return
  
  try {
    await ElMessageBox.confirm(
      `确认平仓 ${Math.abs(position.volume)} ${position.symbol}?`,
      '确认平仓',
      {
        confirmButtonText: '确认',
        cancelButtonText: '取消',
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
    ElMessage.success(`平仓订单提交成功: ${result.orderId}`)
    
    // 刷新持仓数据
    await refreshPositions()
  } catch (error: any) {
    if (error !== 'cancel') {
      ElMessage.error(`平仓失败: ${error.message}`)
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
  
  ElMessage.info('已设置加仓参数，请在交易面板调整数量后提交订单')
}

const closeAllPositions = async () => {
  const openPositions = positions.value.filter(p => p.volume !== 0)
  if (openPositions.length === 0) return
  
  try {
    await ElMessageBox.confirm(
      `确认平仓所有 ${openPositions.length} 个持仓?`,
      '确认全部平仓',
      {
        confirmButtonText: '确认',
        cancelButtonText: '取消',
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
        console.error(`平仓 ${position.symbol} 失败:`, error)
      }
    }
    
    ElMessage.success('全部平仓订单已提交')
    await refreshPositions()
  } catch (error: any) {
    if (error !== 'cancel') {
      ElMessage.error(`全部平仓失败: ${error.message}`)
    }
  }
}
</script>

<style lang="scss" scoped>
.position-list {
  height: 100%;
  display: flex;
  flex-direction: column;
}

.position-header {
  height: 40px;
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 0 12px;
  background: $bg-tertiary;
  border-bottom: 1px solid $border-color;
  
  .header-title {
    font-weight: 600;
    color: $text-primary;
  }
  
  .header-summary {
    .total-pnl {
      font-size: $font-size-sm;
      font-weight: bold;
      font-family: 'Courier New', monospace;
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
  
  :deep(.el-table) {
    .el-table__header {
      th {
        background: $bg-tertiary !important;
        font-size: $font-size-sm;
        font-weight: 600;
        padding: 8px 0;
      }
    }
    
    .el-table__body {
      tr {
        &:hover {
          background: rgba(255, 255, 255, 0.05) !important;
        }
        
        td {
          padding: 8px 0;
          font-size: $font-size-sm;
          border-bottom: 1px solid rgba(255, 255, 255, 0.05);
        }
      }
    }
  }
}

.symbol-cell {
  font-weight: bold;
  color: $text-primary;
}

.volume-cell, .price-cell, .margin-cell {
  font-family: 'Courier New', monospace;
  font-weight: bold;
}

.pnl-cell {
  font-family: 'Courier New', monospace;
  font-weight: bold;
}

.action-buttons {
  display: flex;
  gap: 4px;
  justify-content: center;
  
  .el-button {
    font-size: $font-size-xs;
    padding: 4px 8px;
  }
}
</style>