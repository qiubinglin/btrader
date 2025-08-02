<template>
  <div class="trade-list">
    <div class="list-header">
      <div class="header-title">Trade Details</div>
      <div class="header-actions">
        <el-button size="small" @click="clearTrades">Clear</el-button>
      </div>
    </div>
    
    <div class="list-content">
      <el-table
        :data="tradeList"
        style="width: 100%"
        size="small"
        :show-header="true"
        :stripe="true"
        :border="false"
        height="100%"
        @row-click="handleTradeClick"
      >
        <el-table-column prop="timestamp" label="Time" width="80" align="center">
          <template #default="{ row }">
            <span class="time-text">{{ formatTime(row.timestamp) }}</span>
          </template>
        </el-table-column>
        
        <el-table-column prop="price" label="Price" width="90" align="right">
          <template #default="{ row }">
            <span class="price-text" :class="row.side === 'buy' ? 'text-up' : 'text-down'">
              {{ formatPrice(row.price) }}
            </span>
          </template>
        </el-table-column>
        
        <el-table-column prop="volume" label="Volume" width="80" align="right">
          <template #default="{ row }">
            <span class="volume-text">{{ formatVolume(row.volume) }}</span>
          </template>
        </el-table-column>
        
        <el-table-column prop="side" label="Side" width="50" align="center">
          <template #default="{ row }">
            <span class="side-text" :class="row.side === 'buy' ? 'text-up' : 'text-down'">
              {{ row.side === 'buy' ? 'Buy' : 'Sell' }}
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
import { ElMessage } from 'element-plus'

const store = useStore()

const tradeList = computed(() => store.state.trading.tradeList || [])

const formatTime = (timestamp: number) => {
  const date = new Date(timestamp)
  return date.toLocaleTimeString('en-US', { 
    hour12: false,
    hour: '2-digit',
    minute: '2-digit',
    second: '2-digit'
  })
}

const formatPrice = (price: number) => {
  return price.toFixed(2)
}

const formatVolume = (volume: number) => {
  return volume.toFixed(4)
}

const clearTrades = () => {
  store.dispatch('trading/clearTrades')
  ElMessage.success('Trade list cleared')
}

const handleTradeClick = (row: any) => {
  // When clicking on a trade record, fill the price into the trading panel
  store.dispatch('trading/setOrderPrice', row.price)
}
</script>

<style lang="scss" scoped>
.trade-list {
  height: 100%;
  display: flex;
  flex-direction: column;
  background: $bg-secondary;
  border-radius: 4px;
  border: 1px solid $border-color;
  font-family: 'Times New Roman', Times, serif;
}

.list-header {
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
  
  .header-actions {
    display: flex;
    gap: 8px;
  }
}

.list-content {
  flex: 1;
  overflow: hidden;
}

.time-text {
  font-family: 'Courier New', monospace;
  font-size: $font-size-xs;
}

.price-text {
  font-family: 'Courier New', monospace;
  font-weight: bold;
}

.volume-text {
  font-family: 'Courier New', monospace;
}

.side-text {
  font-weight: bold;
  padding: 2px 6px;
  border-radius: 2px;
  font-size: $font-size-xs;
  font-family: 'Times New Roman', Times, serif;
  
  &.text-up {
    background: rgba(76, 175, 80, 0.1);
    color: $price-up;
  }
  
  &.text-down {
    background: rgba(244, 67, 54, 0.1);
    color: $price-down;
  }
}
</style>