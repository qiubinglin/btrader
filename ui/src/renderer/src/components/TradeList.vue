<template>
  <div class="trade-list">
    <div class="trade-header">
      <div class="header-title">逐笔成交</div>
      <div class="header-actions">
        <el-button size="small" @click="clearTrades">清空</el-button>
      </div>
    </div>
    <div class="trade-table-container">
      <el-table
        :data="tradeHistory"
        height="100%"
        size="small"
        :show-header="true"
        stripe
        @row-click="handleRowClick"
      >
        <el-table-column prop="timestamp" label="时间" width="80" align="center">
          <template #default="{ row }">
            <span class="time-cell">{{ formatTime(row.timestamp) }}</span>
          </template>
        </el-table-column>
        <el-table-column prop="price" label="价格" width="90" align="right">
          <template #default="{ row }">
            <span class="price-cell" :class="getPriceClass(row)">
              {{ formatPrice(row.price) }}
            </span>
          </template>
        </el-table-column>
        <el-table-column prop="volume" label="数量" width="80" align="right">
          <template #default="{ row }">
            <span class="volume-cell">{{ formatVolume(row.volume) }}</span>
          </template>
        </el-table-column>
        <el-table-column prop="side" label="方向" width="50" align="center">
          <template #default="{ row }">
            <span class="side-indicator" :class="row.side">
              {{ row.side === 'buy' ? '买' : '卖' }}
            </span>
          </template>
        </el-table-column>
      </el-table>
    </div>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import { useStore } from 'vuex'

const store = useStore()

const tradeHistory = computed(() => store.state.market.tradeHistory)

const formatTime = (timestamp: number): string => {
  const date = new Date(timestamp)
  return `${date.getHours().toString().padStart(2, '0')}:${date.getMinutes().toString().padStart(2, '0')}:${date.getSeconds().toString().padStart(2, '0')}`
}

const formatPrice = (price: number): string => {
  return price.toFixed(2)
}

const formatVolume = (volume: number): string => {
  if (volume >= 1000000) {
    return (volume / 1000000).toFixed(1) + 'M'
  } else if (volume >= 1000) {
    return (volume / 1000).toFixed(1) + 'K'
  }
  return volume.toString()
}

const getPriceClass = (trade: any): string => {
  return trade.side === 'buy' ? 'text-up' : 'text-down'
}

const handleRowClick = (row: any) => {
  // 点击成交记录时，将价格填入交易面板
  store.dispatch('trading/updateOrderForm', { price: row.price })
}

const clearTrades = () => {
  // 清空成交记录
  store.commit('market/SET_TRADE_HISTORY', [])
}
</script>

<style lang="scss" scoped>
.trade-list {
  height: 100%;
  display: flex;
  flex-direction: column;
}

.trade-header {
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
}

.trade-table-container {
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
          cursor: pointer;
        }
        
        td {
          padding: 4px 0;
          font-size: $font-size-sm;
          border-bottom: 1px solid rgba(255, 255, 255, 0.05);
        }
      }
    }
  }
}

.time-cell {
  font-family: 'Courier New', monospace;
  font-size: $font-size-xs;
}

.price-cell {
  font-family: 'Courier New', monospace;
  font-weight: bold;
}

.volume-cell {
  font-family: 'Courier New', monospace;
}

.side-indicator {
  font-weight: bold;
  padding: 2px 6px;
  border-radius: 2px;
  font-size: $font-size-xs;
  
  &.buy {
    background: $buy-bg;
    color: $buy-color;
  }
  
  &.sell {
    background: $sell-bg;
    color: $sell-color;
  }
}
</style>