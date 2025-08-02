<template>
  <div class="footprint-chart">
    <div class="footprint-header">
      <div class="header-title">Footprint Chart</div>
      <div class="header-controls">
        <el-select v-model="aggregation" size="small" style="width: 80px">
          <el-option label="0.01" :value="0.01" />
          <el-option label="0.1" :value="0.1" />
          <el-option label="1" :value="1" />
          <el-option label="10" :value="10" />
        </el-select>
      </div>
    </div>
    
    <div class="footprint-content">
      <div class="price-levels" ref="priceContainer">
        <div 
          v-for="level in displayLevels" 
          :key="level.price"
          class="price-level"
          @click="handlePriceClick(level.price)"
        >
          <div class="price-info">
            <span class="price-value" :class="getPriceClass(level.price)">
              {{ formatPrice(level.price) }}
            </span>
          </div>
          <div class="volume-info">
            <div class="volume-bars">
              <div 
                class="buy-bar" 
                :style="{ width: level.buyPercentage + '%' }"
                :title="`Buy: ${level.buyVolume}`"
              ></div>
              <div 
                class="sell-bar" 
                :style="{ width: level.sellPercentage + '%' }"
                :title="`Sell: ${level.sellVolume}`"
              ></div>
            </div>
            <div class="volume-text">
              <span class="buy-volume">{{ formatVolume(level.buyVolume) }}</span>
              <span class="total-volume">{{ formatVolume(level.totalVolume) }}</span>
              <span class="sell-volume">{{ formatVolume(level.sellVolume) }}</span>
            </div>
          </div>
          <div class="delta-info" :class="getDeltaClass(level.delta)">
            {{ formatDelta(level.delta) }}
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, watch } from 'vue'
import { useStore } from 'vuex'

const store = useStore()
const aggregation = ref(0.1)
const priceContainer = ref<HTMLElement>()

const footprintData = computed(() => store.state.market.footprintData)
const currentPrice = computed(() => store.getters['market/currentPrice'])

// 根据聚合度处理足迹数据
const displayLevels = computed(() => {
  if (!footprintData.value || !Array.isArray(footprintData.value)) {
    return generateMockData()
  }
  
  const aggregatedData = aggregateByPrice(footprintData.value, aggregation.value)
  const maxVolume = Math.max(...aggregatedData.map(level => level.totalVolume))
  
  return aggregatedData.map(level => ({
    ...level,
    buyPercentage: maxVolume > 0 ? (level.buyVolume / maxVolume) * 100 : 0,
    sellPercentage: maxVolume > 0 ? (level.sellVolume / maxVolume) * 100 : 0
  })).sort((a, b) => b.price - a.price).slice(0, 50) // 显示最近50个价格档位
})

// 生成模拟数据用于演示
const generateMockData = () => {
  const basePrice = currentPrice.value || 50000
  const levels = []
  
  for (let i = -25; i <= 25; i++) {
    const price = basePrice + (i * aggregation.value)
    const buyVolume = Math.random() * 1000
    const sellVolume = Math.random() * 1000
    const totalVolume = buyVolume + sellVolume
    const maxVolume = 1000
    
    levels.push({
      price,
      buyVolume,
      sellVolume,
      totalVolume,
      delta: buyVolume - sellVolume,
      buyPercentage: (buyVolume / maxVolume) * 100,
      sellPercentage: (sellVolume / maxVolume) * 100
    })
  }
  
  return levels.sort((a, b) => b.price - a.price)
}

const aggregateByPrice = (data: any[], step: number) => {
  const aggregated = new Map()
  
  data.forEach(trade => {
    const priceLevel = Math.round(trade.price / step) * step
    const key = priceLevel.toFixed(2)
    
    if (!aggregated.has(key)) {
      aggregated.set(key, {
        price: priceLevel,
        buyVolume: 0,
        sellVolume: 0,
        totalVolume: 0,
        delta: 0
      })
    }
    
    const level = aggregated.get(key)
    if (trade.side === 'buy') {
      level.buyVolume += trade.volume
    } else {
      level.sellVolume += trade.volume
    }
    level.totalVolume = level.buyVolume + level.sellVolume
    level.delta = level.buyVolume - level.sellVolume
  })
  
  return Array.from(aggregated.values())
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
  return Math.round(volume).toString()
}

const formatDelta = (delta: number): string => {
  const sign = delta >= 0 ? '+' : ''
  return sign + Math.round(delta).toString()
}

const getPriceClass = (price: number): string => {
  if (price > currentPrice.value) return 'text-up'
  if (price < currentPrice.value) return 'text-down'
  return 'text-equal'
}

const getDeltaClass = (delta: number): string => {
  if (delta > 0) return 'positive-delta'
  if (delta < 0) return 'negative-delta'
  return 'neutral-delta'
}

const handlePriceClick = (price: number) => {
  store.dispatch('trading/updateOrderForm', { price })
}

// 当当前价格变化时，滚动到对应位置
watch(currentPrice, (newPrice) => {
  if (priceContainer.value && newPrice > 0) {
    // 找到最接近当前价格的元素并滚动到视图中
    setTimeout(() => {
      const levels = priceContainer.value?.querySelectorAll('.price-level')
      if (levels) {
        const currentIndex = displayLevels.value.findIndex(level => 
          Math.abs(level.price - newPrice) < aggregation.value / 2
        )
        if (currentIndex >= 0 && levels[currentIndex]) {
          levels[currentIndex].scrollIntoView({ 
            behavior: 'smooth', 
            block: 'center' 
          })
        }
      }
    }, 100)
  }
}, { immediate: true })
</script>

<style lang="scss" scoped>
.footprint-chart {
  height: 100%;
  display: flex;
  flex-direction: column;
  background: $bg-secondary;
  border-radius: 4px;
  border: 1px solid $border-color;
  font-family: 'Times New Roman', Times, serif;
}

.footprint-header {
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
  
  .header-controls {
    display: flex;
    gap: 8px;
  }
}

.footprint-content {
  flex: 1;
  overflow-y: auto;
}

.price-levels {
  .price-level {
    display: flex;
    align-items: center;
    padding: 4px 12px;
    border-bottom: 1px solid rgba(255, 255, 255, 0.05);
    cursor: pointer;
    transition: background-color 0.2s;
    font-family: 'Times New Roman', Times, serif;
    
    &:hover {
      background: rgba(255, 255, 255, 0.05);
    }
    
    .price-info {
      width: 80px;
      
      .price-value {
        font-weight: bold;
        font-family: 'Courier New', monospace;
        
        &.text-up {
          color: $price-up;
        }
        
        &.text-down {
          color: $price-down;
        }
      }
    }
    
    .volume-info {
      flex: 1;
      margin: 0 12px;
      
      .volume-bars {
        display: flex;
        height: 16px;
        margin-bottom: 2px;
        
        .buy-bar {
          background: $buy-color;
          height: 100%;
          transition: width 0.2s ease;
        }
        
        .sell-bar {
          background: $sell-color;
          height: 100%;
          transition: width 0.2s ease;
        }
      }
      
      .volume-text {
        display: flex;
        justify-content: space-between;
        font-size: $font-size-xs;
        
        .buy-volume {
          color: $buy-color;
          font-family: 'Courier New', monospace;
        }
        
        .total-volume {
          color: $text-secondary;
          font-family: 'Courier New', monospace;
        }
        
        .sell-volume {
          color: $sell-color;
          font-family: 'Courier New', monospace;
        }
      }
    }
    
    .delta-info {
      width: 60px;
      text-align: right;
      font-weight: bold;
      font-family: 'Courier New', monospace;
      
      &.text-up {
        color: $price-up;
      }
      
      &.text-down {
        color: $price-down;
      }
    }
  }
}
</style>