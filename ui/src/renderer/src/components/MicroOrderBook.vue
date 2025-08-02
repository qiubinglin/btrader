<template>
  <div class="micro-orderbook">
    <div class="micro-header">
      <div class="header-title">Micro Order Book</div>
      <div class="refresh-button">
        <el-button size="small" @click="refreshData" :loading="isRefreshing">Refresh</el-button>
      </div>
    </div>
    
    <div class="micro-content" v-if="microOrderBook">
      <!-- Best Ask -->
      <div class="best-ask section">
        <div class="section-title">Best Ask</div>
        <div class="price-level best-level ask-level" v-if="bestAsk">
          <div class="level-info">
            <span class="price ask-price">{{ formatPrice(bestAsk.price) }}</span>
            <span class="volume">{{ formatVolume(bestAsk.volume) }}</span>
          </div>
          <div class="level-actions">
            <el-button size="small" type="success" @click="buyAtPrice(bestAsk.price)">
              Buy
            </el-button>
          </div>
        </div>
      </div>
      
      <!-- Spread Info -->
      <div class="spread-info section">
        <div class="spread-detail">
          <span class="spread-label">Spread:</span>
          <span class="spread-value">{{ formatPrice(spread) }}</span>
          <span class="spread-percent">({{ spreadPercent.toFixed(3) }}%)</span>
        </div>
        <div class="mid-price">
          <span class="mid-label">Mid Price:</span>
          <span class="mid-value">{{ formatPrice(midPrice) }}</span>
        </div>
      </div>
      
      <!-- Best Bid -->
      <div class="best-bid section">
        <div class="section-title">Best Bid</div>
        <div class="price-level best-level bid-level" v-if="bestBid">
          <div class="level-info">
            <span class="price bid-price">{{ formatPrice(bestBid.price) }}</span>
            <span class="volume">{{ formatVolume(bestBid.volume) }}</span>
          </div>
          <div class="level-actions">
            <el-button size="small" type="danger" @click="sellAtPrice(bestBid.price)">
              Sell
            </el-button>
          </div>
        </div>
      </div>
      
      <!-- Depth Statistics -->
      <div class="depth-stats section">
        <div class="section-title">Depth Statistics</div>
        <div class="stats-grid">
          <div class="stat-item">
            <span class="stat-label">Bid Depth:</span>
            <span class="stat-value bid-color">{{ formatVolume(bidDepth) }}</span>
          </div>
          <div class="stat-item">
            <span class="stat-label">Ask Depth:</span>
            <span class="stat-value ask-color">{{ formatVolume(askDepth) }}</span>
          </div>
          <div class="stat-item">
            <span class="stat-label">Bid/Ask Ratio:</span>
            <span class="stat-value">{{ bidAskRatio.toFixed(2) }}</span>
          </div>
          <div class="stat-item">
            <span class="stat-label">Total Depth:</span>
            <span class="stat-value">{{ formatVolume(totalDepth) }}</span>
          </div>
        </div>
      </div>
    </div>
    
    <div v-else class="no-data">
      <span>No Micro Order Book Data</span>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed } from 'vue'
import { useStore } from 'vuex'
import { ElMessage } from 'element-plus'

const store = useStore()
const isRefreshing = ref(false)

const microOrderBook = computed(() => store.state.market.microOrderBook)

const bestAsk = computed(() => {
  if (!microOrderBook.value || !microOrderBook.value.asks.length) return null
  return microOrderBook.value.asks[0]
})

const bestBid = computed(() => {
  if (!microOrderBook.value || !microOrderBook.value.bids.length) return null
  return microOrderBook.value.bids[0]
})

const spread = computed(() => {
  if (!bestAsk.value || !bestBid.value) return 0
  return bestAsk.value.price - bestBid.value.price
})

const midPrice = computed(() => {
  if (!bestAsk.value || !bestBid.value) return 0
  return (bestAsk.value.price + bestBid.value.price) / 2
})

const spreadPercent = computed(() => {
  if (!bestAsk.value || !bestBid.value || bestBid.value.price === 0) return 0
  return (spread.value / bestBid.value.price) * 100
})

const bidDepth = computed(() => {
  if (!microOrderBook.value) return 0
  return microOrderBook.value.bids.slice(0, 5).reduce((sum, bid) => sum + bid.volume, 0)
})

const askDepth = computed(() => {
  if (!microOrderBook.value) return 0
  return microOrderBook.value.asks.slice(0, 5).reduce((sum, ask) => sum + ask.volume, 0)
})

const totalDepth = computed(() => bidDepth.value + askDepth.value)

const bidAskRatio = computed(() => {
  if (askDepth.value === 0) return bidDepth.value > 0 ? 999 : 0
  return bidDepth.value / askDepth.value
})

const formatPrice = (price: number): string => {
  return price.toFixed(2)
}

const formatVolume = (volume: number): string => {
  if (volume >= 1000000) {
    return (volume / 1000000).toFixed(1) + 'M'
  } else if (volume >= 1000) {
    return (volume / 1000).toFixed(1) + 'K'
  }
  return volume.toFixed(0)
}

const buyAtPrice = (price: number) => {
  store.dispatch('trading/updateOrderForm', { 
    price, 
    side: 'buy',
    orderType: 'limit'
  })
  ElMessage.success(`已设置买入价格: ${formatPrice(price)}`)
}

const sellAtPrice = (price: number) => {
  store.dispatch('trading/updateOrderForm', { 
    price, 
    side: 'sell',
    orderType: 'limit'
  })
  ElMessage.success(`已设置卖出价格: ${formatPrice(price)}`)
}

const refreshData = async () => {
  isRefreshing.value = true
  try {
    // 这里可以添加刷新微盘口数据的逻辑
    await new Promise(resolve => setTimeout(resolve, 500)) // 模拟异步操作
    ElMessage.success('微盘口数据已刷新')
  } catch (error) {
    ElMessage.error('刷新失败')
  } finally {
    isRefreshing.value = false
  }
}
</script>

<style lang="scss" scoped>
.micro-orderbook {
  height: 100%;
  display: flex;
  flex-direction: column;
  background: $bg-secondary;
  border-radius: 4px;
  border: 1px solid $border-color;
  font-family: 'Times New Roman', Times, serif;
}

.micro-header {
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
  
  .refresh-button {
    .el-button {
      font-family: 'Times New Roman', Times, serif;
    }
  }
}

.micro-content {
  flex: 1;
  padding: 12px;
  display: flex;
  flex-direction: column;
  gap: 12px;
  overflow-y: auto;
}

.section {
  .section-title {
    font-weight: 600;
    color: $text-primary;
    margin-bottom: 8px;
    font-family: 'Times New Roman', Times, serif;
  }
}

.best-ask, .best-bid {
  .price-level {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: 8px 12px;
    border-radius: 4px;
    border: 1px solid $border-color;
    
    .level-info {
      display: flex;
      flex-direction: column;
      gap: 4px;
      
      .price {
        font-size: $font-size-lg;
        font-weight: bold;
        font-family: 'Courier New', monospace;
        
        &.ask-price {
          color: $sell-color;
        }
        
        &.bid-price {
          color: $buy-color;
        }
      }
      
      .volume {
        font-size: $font-size-sm;
        color: $text-secondary;
        font-family: 'Courier New', monospace;
      }
    }
    
    .level-actions {
      .el-button {
        font-family: 'Times New Roman', Times, serif;
      }
    }
  }
  
  &.ask-level {
    .price-level {
      background: rgba(244, 67, 54, 0.05);
      border-color: $sell-color;
    }
  }
  
  &.bid-level {
    .price-level {
      background: rgba(76, 175, 80, 0.05);
      border-color: $buy-color;
    }
  }
}

.spread-info {
  .spread-detail {
    display: flex;
    align-items: center;
    gap: 8px;
    margin-bottom: 4px;
    
    .spread-label {
      color: $text-secondary;
      font-family: 'Times New Roman', Times, serif;
    }
    
    .spread-value {
      color: $text-primary;
      font-weight: bold;
      font-family: 'Courier New', monospace;
    }
    
    .spread-percent {
      color: $text-secondary;
      font-size: $font-size-xs;
      font-family: 'Times New Roman', Times, serif;
    }
  }
  
  .mid-price {
    display: flex;
    align-items: center;
    gap: 8px;
    
    .mid-label {
      color: $text-secondary;
      font-family: 'Times New Roman', Times, serif;
    }
    
    .mid-value {
      color: $text-primary;
      font-weight: bold;
      font-family: 'Courier New', monospace;
    }
  }
}

.depth-stats {
  .stats-grid {
    display: grid;
    grid-template-columns: 1fr 1fr;
    gap: 8px;
    
    .stat-item {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 4px 8px;
      background: $bg-tertiary;
      border-radius: 4px;
      
      .stat-label {
        color: $text-secondary;
        font-size: $font-size-sm;
        font-family: 'Times New Roman', Times, serif;
      }
      
      .stat-value {
        font-weight: bold;
        font-family: 'Courier New', monospace;
        
        &.bid-color {
          color: $buy-color;
        }
        
        &.ask-color {
          color: $sell-color;
        }
      }
    }
  }
}

.no-data {
  flex: 1;
  display: flex;
  align-items: center;
  justify-content: center;
  color: $text-secondary;
  font-family: 'Times New Roman', Times, serif;
}
</style>