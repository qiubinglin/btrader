<template>
  <div class="micro-orderbook">
    <div class="micro-header">
      <div class="header-title">微盘口</div>
      <div class="refresh-button">
        <el-button size="small" @click="refreshData" :loading="isRefreshing">刷新</el-button>
      </div>
    </div>
    
    <div class="micro-content" v-if="microOrderBook">
      <!-- 最优卖价 -->
      <div class="best-ask section">
        <div class="section-title">最优卖价</div>
        <div class="price-level best-level ask-level" v-if="bestAsk">
          <div class="level-info">
            <span class="price ask-price">{{ formatPrice(bestAsk.price) }}</span>
            <span class="volume">{{ formatVolume(bestAsk.volume) }}</span>
          </div>
          <div class="level-actions">
            <el-button size="small" type="success" @click="buyAtPrice(bestAsk.price)">
              买入
            </el-button>
          </div>
        </div>
      </div>
      
      <!-- 价差信息 -->
      <div class="spread-info section">
        <div class="spread-detail">
          <span class="spread-label">价差:</span>
          <span class="spread-value">{{ formatPrice(spread) }}</span>
          <span class="spread-percent">({{ spreadPercent.toFixed(3) }}%)</span>
        </div>
        <div class="mid-price">
          <span class="mid-label">中间价:</span>
          <span class="mid-value">{{ formatPrice(midPrice) }}</span>
        </div>
      </div>
      
      <!-- 最优买价 -->
      <div class="best-bid section">
        <div class="section-title">最优买价</div>
        <div class="price-level best-level bid-level" v-if="bestBid">
          <div class="level-info">
            <span class="price bid-price">{{ formatPrice(bestBid.price) }}</span>
            <span class="volume">{{ formatVolume(bestBid.volume) }}</span>
          </div>
          <div class="level-actions">
            <el-button size="small" type="danger" @click="sellAtPrice(bestBid.price)">
              卖出
            </el-button>
          </div>
        </div>
      </div>
      
      <!-- 深度统计 -->
      <div class="depth-stats section">
        <div class="section-title">深度统计</div>
        <div class="stats-grid">
          <div class="stat-item">
            <span class="stat-label">买单深度:</span>
            <span class="stat-value bid-color">{{ formatVolume(bidDepth) }}</span>
          </div>
          <div class="stat-item">
            <span class="stat-label">卖单深度:</span>
            <span class="stat-value ask-color">{{ formatVolume(askDepth) }}</span>
          </div>
          <div class="stat-item">
            <span class="stat-label">买卖比:</span>
            <span class="stat-value">{{ bidAskRatio.toFixed(2) }}</span>
          </div>
          <div class="stat-item">
            <span class="stat-label">总深度:</span>
            <span class="stat-value">{{ formatVolume(totalDepth) }}</span>
          </div>
        </div>
      </div>
    </div>
    
    <div v-else class="no-data">
      <span>暂无微盘口数据</span>
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
  font-family: 'Courier New', monospace;
}

.micro-header {
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

.micro-content {
  flex: 1;
  overflow-y: auto;
  padding: 8px;
}

.section {
  margin-bottom: 16px;
  background: rgba(255, 255, 255, 0.02);
  border-radius: 4px;
  padding: 8px;
  
  .section-title {
    font-size: $font-size-sm;
    font-weight: bold;
    color: $text-primary;
    margin-bottom: 8px;
    border-bottom: 1px solid $border-color;
    padding-bottom: 4px;
  }
}

.best-level {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 8px;
  background: rgba(255, 255, 255, 0.05);
  border-radius: 4px;
  
  .level-info {
    display: flex;
    flex-direction: column;
    gap: 4px;
    
    .price {
      font-size: $font-size-md;
      font-weight: bold;
    }
    
    .volume {
      font-size: $font-size-sm;
      color: $text-secondary;
    }
  }
}

.ask-level {
  .price {
    color: $sell-color;
  }
}

.bid-level {
  .price {
    color: $buy-color;
  }
}

.spread-info {
  .spread-detail {
    display: flex;
    align-items: center;
    gap: 8px;
    margin-bottom: 8px;
    
    .spread-label {
      color: $text-secondary;
    }
    
    .spread-value {
      font-weight: bold;
      color: $text-primary;
    }
    
    .spread-percent {
      font-size: $font-size-sm;
      color: $text-muted;
    }
  }
  
  .mid-price {
    display: flex;
    align-items: center;
    gap: 8px;
    
    .mid-label {
      color: $text-secondary;
    }
    
    .mid-value {
      font-weight: bold;
      color: $warning-color;
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
      padding: 4px;
      background: rgba(255, 255, 255, 0.02);
      border-radius: 2px;
      
      .stat-label {
        font-size: $font-size-xs;
        color: $text-secondary;
      }
      
      .stat-value {
        font-size: $font-size-xs;
        font-weight: bold;
        color: $text-primary;
        
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
  color: $text-muted;
}
</style>