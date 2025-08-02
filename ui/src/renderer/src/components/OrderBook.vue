<template>
  <div class="order-book">
    <div class="orderbook-header">
      <div class="header-title">Order Book</div>
      <div class="spread-info" v-if="orderBook">
        <span class="spread-label">Spread:</span>
        <span class="spread-value">{{ formatPrice(spread) }}</span>
      </div>
    </div>
    
    <div class="orderbook-content" v-if="orderBook">
      <!-- Asks Section -->
      <div class="asks-section">
        <div class="section-header sell">
          <span>Price</span>
          <span>Volume</span>
          <span>Total</span>
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
      
      <!-- Spread Section -->
      <div class="spread-section">
        <div class="current-price" :class="priceChangeClass">
          {{ formatPrice(currentPrice) }}
        </div>
      </div>
      
      <!-- Bids Section -->
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
          <span>Price</span>
          <span>Volume</span>
          <span>Total</span>
        </div>
      </div>
    </div>
    
    <div v-else class="no-data">
      <span>No Data</span>
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
  background: $bg-secondary;
  border-radius: 4px;
  border: 1px solid $border-color;
  font-family: 'Times New Roman', Times, serif;
}

.orderbook-header {
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
  
  .spread-info {
    display: flex;
    align-items: center;
    gap: 8px;
    font-size: $font-size-sm;
    
    .spread-label {
      color: $text-secondary;
      font-family: 'Times New Roman', Times, serif;
    }
    
    .spread-value {
      color: $text-primary;
      font-weight: bold;
      font-family: 'Times New Roman', Times, serif;
    }
  }
}

.orderbook-content {
  flex: 1;
  display: flex;
  flex-direction: column;
  overflow: hidden;
}

.asks-section {
  flex: 1;
  display: flex;
  flex-direction: column;
  
  .section-header {
    display: grid;
    grid-template-columns: 1fr 1fr 1fr;
    padding: 8px 12px;
    font-size: $font-size-xs;
    font-weight: 600;
    color: $text-secondary;
    background: $bg-tertiary;
    border-bottom: 1px solid $border-color;
    font-family: 'Times New Roman', Times, serif;
    
    &.sell {
      color: $sell-color;
    }
  }
  
  .order-levels {
    flex: 1;
    overflow-y: auto;
  }
}

.bids-section {
  flex: 1;
  display: flex;
  flex-direction: column;
  
  .order-levels {
    flex: 1;
    overflow-y: auto;
  }
  
  .section-header {
    display: grid;
    grid-template-columns: 1fr 1fr 1fr;
    padding: 8px 12px;
    font-size: $font-size-xs;
    font-weight: 600;
    color: $text-secondary;
    background: $bg-tertiary;
    border-top: 1px solid $border-color;
    font-family: 'Times New Roman', Times, serif;
    
    &.buy {
      color: $buy-color;
    }
  }
}

.order-level {
  display: grid;
  grid-template-columns: 1fr 1fr 1fr;
  padding: 4px 12px;
  font-size: $font-size-sm;
  position: relative;
  cursor: pointer;
  transition: background-color 0.2s;
  font-family: 'Times New Roman', Times, serif;
  
  &:hover {
    background: rgba(255, 255, 255, 0.05);
  }
  
  .level-bg {
    position: absolute;
    top: 0;
    left: 0;
    height: 100%;
    opacity: 0.1;
    z-index: 0;
  }
  
  .ask-bg {
    background: $sell-color;
  }
  
  .bid-bg {
    background: $buy-color;
  }
  
  span {
    position: relative;
    z-index: 1;
    font-family: 'Courier New', monospace;
  }
  
  .price {
    font-weight: bold;
  }
  
  .volume, .cumulative {
    text-align: right;
  }
}

.spread-section {
  padding: 8px 12px;
  background: $bg-tertiary;
  border-top: 1px solid $border-color;
  border-bottom: 1px solid $border-color;
  text-align: center;
  
  .current-price {
    font-size: $font-size-lg;
    font-weight: bold;
    font-family: 'Times New Roman', Times, serif;
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