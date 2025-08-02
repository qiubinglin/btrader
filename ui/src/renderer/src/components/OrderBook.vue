<template>
  <div class="order-book">
    <div class="orderbook-header">
      <div class="header-title">买卖档位</div>
      <div class="spread-info" v-if="orderBook">
        <span class="spread-label">价差:</span>
        <span class="spread-value">{{ formatPrice(spread) }}</span>
      </div>
    </div>
    
    <div class="orderbook-content" v-if="orderBook">
      <!-- 卖单区域 -->
      <div class="asks-section">
        <div class="section-header sell">
          <span>价格</span>
          <span>数量</span>
          <span>累计</span>
        </div>
        <div class="order-levels">
          <div 
            v-for="(ask, index) in displayAsks" 
            :key="`ask-${index}`"
            class="order-level ask-level"
            @click="handleLevelClick(ask)"
          >
            <div class="level-bg ask-bg" :style="{ width: ask.percentage + '%' }"></div>
            <span class="price">{{ formatPrice(ask.price) }}</span>
            <span class="volume">{{ formatVolume(ask.volume) }}</span>
            <span class="cumulative">{{ formatVolume(ask.cumulative) }}</span>
          </div>
        </div>
      </div>
      
      <!-- 中间价差显示 -->
      <div class="spread-section">
        <div class="current-price" :class="priceChangeClass">
          {{ formatPrice(currentPrice) }}
        </div>
      </div>
      
      <!-- 买单区域 -->
      <div class="bids-section">
        <div class="order-levels">
          <div 
            v-for="(bid, index) in displayBids" 
            :key="`bid-${index}`"
            class="order-level bid-level"
            @click="handleLevelClick(bid)"
          >
            <div class="level-bg bid-bg" :style="{ width: bid.percentage + '%' }"></div>
            <span class="price">{{ formatPrice(bid.price) }}</span>
            <span class="volume">{{ formatVolume(bid.volume) }}</span>
            <span class="cumulative">{{ formatVolume(bid.cumulative) }}</span>
          </div>
        </div>
        <div class="section-header buy">
          <span>价格</span>
          <span>数量</span>
          <span>累计</span>
        </div>
      </div>
    </div>
    
    <div v-else class="no-data">
      <span>暂无数据</span>
    </div>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import { useStore } from 'vuex'

const store = useStore()

const orderBook = computed(() => store.state.market.orderBook)
const currentPrice = computed(() => store.getters['market/currentPrice'])
const priceChange = computed(() => store.getters['market/priceChange'])

const priceChangeClass = computed(() => {
  if (priceChange.value > 0) return 'text-up'
  if (priceChange.value < 0) return 'text-down'
  return 'text-equal'
})

const spread = computed(() => {
  if (!orderBook.value || !orderBook.value.asks.length || !orderBook.value.bids.length) {
    return 0
  }
  return orderBook.value.asks[0].price - orderBook.value.bids[0].price
})

const displayAsks = computed(() => {
  if (!orderBook.value || !orderBook.value.asks.length) return []
  
  const asks = [...orderBook.value.asks].slice(0, 10).reverse()
  const maxVolume = Math.max(...asks.map(ask => ask.volume))
  let cumulative = 0
  
  return asks.map(ask => {
    cumulative += ask.volume
    return {
      ...ask,
      percentage: (ask.volume / maxVolume) * 100,
      cumulative
    }
  })
})

const displayBids = computed(() => {
  if (!orderBook.value || !orderBook.value.bids.length) return []
  
  const bids = orderBook.value.bids.slice(0, 10)
  const maxVolume = Math.max(...bids.map(bid => bid.volume))
  let cumulative = 0
  
  return bids.map(bid => {
    cumulative += bid.volume
    return {
      ...bid,
      percentage: (bid.volume / maxVolume) * 100,
      cumulative
    }
  })
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

const handleLevelClick = (level: any) => {
  // 点击档位时，将价格填入交易面板
  store.dispatch('trading/updateOrderForm', { price: level.price })
}
</script>

<style lang="scss" scoped>
.order-book {
  height: 100%;
  display: flex;
  flex-direction: column;
  font-family: 'Courier New', monospace;
}

.orderbook-header {
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
  
  .spread-info {
    font-size: $font-size-sm;
    
    .spread-label {
      color: $text-secondary;
    }
    
    .spread-value {
      color: $text-primary;
      font-weight: bold;
      margin-left: 4px;
    }
  }
}

.orderbook-content {
  flex: 1;
  display: flex;
  flex-direction: column;
}

.asks-section, .bids-section {
  flex: 1;
  display: flex;
  flex-direction: column;
}

.section-header {
  height: 24px;
  display: flex;
  align-items: center;
  padding: 0 8px;
  font-size: $font-size-xs;
  font-weight: bold;
  background: rgba(255, 255, 255, 0.05);
  
  span {
    flex: 1;
    text-align: right;
    
    &:first-child {
      text-align: left;
    }
  }
  
  &.sell {
    color: $sell-color;
  }
  
  &.buy {
    color: $buy-color;
  }
}

.order-levels {
  flex: 1;
  overflow: hidden;
}

.order-level {
  position: relative;
  height: 20px;
  display: flex;
  align-items: center;
  padding: 0 8px;
  font-size: $font-size-xs;
  cursor: pointer;
  
  &:hover {
    background: rgba(255, 255, 255, 0.05);
  }
  
  .level-bg {
    position: absolute;
    top: 0;
    right: 0;
    height: 100%;
    opacity: 0.3;
    transition: width 0.2s ease;
  }
  
  span {
    position: relative;
    z-index: 1;
    flex: 1;
    text-align: right;
    
    &:first-child {
      text-align: left;
    }
    
    &.price {
      font-weight: bold;
    }
  }
}

.ask-level {
  .level-bg {
    background: $sell-color;
  }
  
  .price {
    color: $sell-color;
  }
}

.bid-level {
  .level-bg {
    background: $buy-color;
  }
  
  .price {
    color: $buy-color;
  }
}

.spread-section {
  height: 30px;
  display: flex;
  align-items: center;
  justify-content: center;
  background: rgba(255, 255, 255, 0.02);
  border-top: 1px solid $border-color;
  border-bottom: 1px solid $border-color;
  
  .current-price {
    font-size: $font-size-md;
    font-weight: bold;
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