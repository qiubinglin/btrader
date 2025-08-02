<template>
  <div class="trading-panel">
    <div class="panel-header">
      <div class="header-title">Trading Panel</div>
      <div class="header-actions">
        <el-button size="small" @click="resetForm">Reset</el-button>
      </div>
    </div>
    
    <div class="panel-content">
      <el-form :model="orderForm" label-width="60px" size="small">
        <!-- Trading Symbol -->
        <el-form-item label="Symbol">
          <el-select v-model="orderForm.symbol" style="width: 100%">
            <el-option label="BTC/USDT" value="BTC/USDT" />
            <el-option label="ETH/USDT" value="ETH/USDT" />
            <el-option label="BNB/USDT" value="BNB/USDT" />
          </el-select>
        </el-form-item>
        
        <!-- Order Type -->
        <el-form-item label="Type">
          <el-select v-model="orderForm.orderType" style="width: 100%">
            <el-option label="Limit" value="limit" />
            <el-option label="Market" value="market" />
          </el-select>
        </el-form-item>
        
        <!-- Buy/Sell Direction -->
        <el-form-item label="Side">
          <el-radio-group v-model="orderForm.side" style="width: 100%">
            <el-radio-button label="buy" style="width: 50%">
              <span style="color: #00c853">Buy</span>
            </el-radio-button>
            <el-radio-button label="sell" style="width: 50%">
              <span style="color: #ff1744">Sell</span>
            </el-radio-button>
          </el-radio-group>
        </el-form-item>
        
        <!-- Price -->
        <el-form-item label="Price" v-if="orderForm.orderType === 'limit'">
          <div class="price-input-container">
            <el-input-number
              v-model="orderForm.price"
              :precision="2"
              :step="0.01"
              style="width: 100%"
              placeholder="Enter price"
            />
            <div class="quick-price-buttons">
              <el-button size="small" @click="setMarketPrice">Market</el-button>
              <el-button size="small" @click="setBestBid" v-if="bestBid">Bid</el-button>
              <el-button size="small" @click="setBestAsk" v-if="bestAsk">Ask</el-button>
            </div>
          </div>
        </el-form-item>
        
        <!-- Volume -->
        <el-form-item label="Volume">
          <div class="volume-input-container">
            <el-input-number
              v-model="orderForm.volume"
              :precision="4"
              :step="0.001"
              :min="0.001"
              style="width: 100%"
              placeholder="Enter volume"
            />
            <div class="quick-volume-buttons">
              <el-button size="small" @click="setVolumePercent(25)">25%</el-button>
              <el-button size="small" @click="setVolumePercent(50)">50%</el-button>
              <el-button size="small" @click="setVolumePercent(75)">75%</el-button>
              <el-button size="small" @click="setVolumePercent(100)">Max</el-button>
            </div>
          </div>
        </el-form-item>
        
        <!-- Estimated Amount -->
        <el-form-item label="Amount">
          <div class="amount-display">
            <span class="amount-value">{{ formatAmount(estimatedAmount) }}</span>
            <span class="amount-currency">USDT</span>
          </div>
        </el-form-item>
        
        <!-- Submit Button -->
        <el-form-item>
          <el-button
            :type="orderForm.side === 'buy' ? 'success' : 'danger'"
            :class="orderForm.side === 'buy' ? 'buy-button' : 'sell-button'"
            style="width: 100%"
            size="large"
            :loading="isSubmitting"
            @click="submitOrder"
            :disabled="!canSubmit"
          >
            {{ orderForm.side === 'buy' ? 'Buy' : 'Sell' }} {{ orderForm.symbol }}
          </el-button>
        </el-form-item>
      </el-form>
      
      <!-- Position Summary -->
      <div class="position-summary" v-if="currentPosition">
        <div class="summary-title">Current Position</div>
        <div class="summary-content">
          <div class="position-item">
            <span class="label">Volume:</span>
            <span class="value" :class="currentPosition.volume > 0 ? 'text-up' : 'text-down'">
              {{ formatVolume(currentPosition.volume) }}
            </span>
          </div>
          <div class="position-item">
            <span class="label">Average Price:</span>
            <span class="value">{{ formatPrice(currentPosition.avgPrice) }}</span>
          </div>
          <div class="position-item">
            <span class="label">PnL:</span>
            <span class="value" :class="currentPosition.unrealizedPnl >= 0 ? 'text-up' : 'text-down'">
              {{ formatPnl(currentPosition.unrealizedPnl) }}
            </span>
          </div>
        </div>
      </div>
      
      <!-- Quick Close Button -->
      <div class="quick-actions" v-if="currentPosition && currentPosition.volume !== 0">
        <el-button
          type="warning"
          style="width: 100%"
          @click="quickClose"
          :loading="isSubmitting"
        >
          Quick Close
        </el-button>
      </div>
    </div>
  </div>
</template>

<script setup lang="ts">
import { computed } from 'vue'
import { useStore } from 'vuex'
import { ElMessage, ElMessageBox } from 'element-plus'

const store = useStore()

const orderForm = computed(() => store.state.trading.orderForm)
const isSubmitting = computed(() => store.state.trading.isSubmitting)
const orderBook = computed(() => store.state.market.orderBook)
const currentPrice = computed(() => store.getters['market/currentPrice'])
const availableBalance = computed(() => store.state.account.availableBalance)
const positions = computed(() => store.state.account.positions)

const bestBid = computed(() => {
  return orderBook.value?.bids?.[0] || null
})

const bestAsk = computed(() => {
  return orderBook.value?.asks?.[0] || null
})

const currentPosition = computed(() => {
  return positions.value.find(p => p.symbol === orderForm.value.symbol) || null
})

const estimatedAmount = computed(() => {
  if (orderForm.value.orderType === 'market') {
    return orderForm.value.volume * currentPrice.value
  }
  return orderForm.value.volume * orderForm.value.price
})

const canSubmit = computed(() => {
  if (orderForm.value.volume <= 0) return false
  if (orderForm.value.orderType === 'limit' && orderForm.value.price <= 0) return false
  return true
})

const formatPrice = (price: number): string => {
  return price.toFixed(2)
}

const formatVolume = (volume: number): string => {
  return volume.toFixed(4)
}

const formatAmount = (amount: number): string => {
  return amount.toFixed(2)
}

const formatPnl = (pnl: number): string => {
  const sign = pnl >= 0 ? '+' : ''
  return `${sign}${pnl.toFixed(2)}`
}

const setMarketPrice = () => {
  if (currentPrice.value > 0) {
    store.dispatch('trading/updateOrderForm', { price: currentPrice.value })
  }
}

const setBestBid = () => {
  if (bestBid.value) {
    store.dispatch('trading/updateOrderForm', { price: bestBid.value.price })
  }
}

const setBestAsk = () => {
  if (bestAsk.value) {
    store.dispatch('trading/updateOrderForm', { price: bestAsk.value.price })
  }
}

const setVolumePercent = (percent: number) => {
  // 根据可用余额和价格计算最大可买入数量
  const price = orderForm.value.orderType === 'market' ? currentPrice.value : orderForm.value.price
  if (price <= 0 || availableBalance.value <= 0) return
  
  let maxVolume = 0
  if (orderForm.value.side === 'buy') {
    maxVolume = availableBalance.value / price * 0.95 // 预留5%手续费
  } else {
    // 卖出时基于当前持仓
    maxVolume = currentPosition.value ? Math.abs(currentPosition.value.volume) : 0
  }
  
  const targetVolume = maxVolume * (percent / 100)
  store.dispatch('trading/updateOrderForm', { volume: targetVolume })
}

const submitOrder = async () => {
  try {
    // 确认对话框
    const confirmText = `Confirm ${orderForm.value.side === 'buy' ? 'Buy' : 'Sell'} ${orderForm.value.volume} ${orderForm.value.symbol}?`
    await ElMessageBox.confirm(confirmText, 'Confirm Trade', {
      confirmButtonText: 'Confirm',
      cancelButtonText: 'Cancel',
      type: 'warning'
    })
    
    const result = await store.dispatch('trading/submitOrder')
    ElMessage.success(`Order submitted successfully: ${result.orderId}`)
    
    // 重置表单
    resetForm()
  } catch (error: any) {
    if (error !== 'cancel') {
      ElMessage.error(`Order submission failed: ${error.message || error}`)
    }
  }
}

const quickClose = async () => {
  if (!currentPosition.value) return
  
  try {
    await ElMessageBox.confirm(`Confirm Close ${Math.abs(currentPosition.value.volume)} ${orderForm.value.symbol}?`, 'Confirm Close', {
      confirmButtonText: 'Confirm',
      cancelButtonText: 'Cancel',
      type: 'warning'
    })
    
    // 设置平仓订单
    const closeSide = currentPosition.value.volume > 0 ? 'sell' : 'buy'
    const closeVolume = Math.abs(currentPosition.value.volume)
    
    store.dispatch('trading/updateOrderForm', {
      side: closeSide,
      volume: closeVolume,
      orderType: 'market'
    })
    
    const result = await store.dispatch('trading/submitOrder')
    ElMessage.success(`Close order submitted successfully: ${result.orderId}`)
    
    resetForm()
  } catch (error: any) {
    if (error !== 'cancel') {
      ElMessage.error(`Close failed: ${error.message || error}`)
    }
  }
}

const resetForm = () => {
  store.dispatch('trading/updateOrderForm', {
    volume: 0,
    price: 0,
    orderType: 'limit'
  })
}
</script>

<style lang="scss" scoped>
.trading-panel {
  height: 100%;
  display: flex;
  flex-direction: column;
  background: $bg-secondary;
  border-radius: 4px;
  border: 1px solid $border-color;
  font-family: 'Times New Roman', Times, serif;
}

.panel-header {
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

.panel-content {
  flex: 1;
  padding: 12px;
  overflow-y: auto;
  
  .el-form {
    .el-form-item {
      margin-bottom: 12px;
    }
    
    .el-form-item__label {
      font-family: 'Times New Roman', Times, serif;
    }
  }
}

.price-input-container, .volume-input-container {
  .quick-price-buttons, .quick-volume-buttons {
    display: flex;
    gap: 4px;
    margin-top: 4px;
    
    .el-button {
      flex: 1;
      font-size: $font-size-xs;
      font-family: 'Times New Roman', Times, serif;
    }
  }
}

.amount-display {
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 8px 12px;
  background: $bg-tertiary;
  border-radius: 4px;
  border: 1px solid $border-color;
  
  .amount-value {
    font-size: $font-size-lg;
    font-weight: bold;
    color: $text-primary;
    font-family: 'Courier New', monospace;
  }
  
  .amount-currency {
    color: $text-secondary;
    font-family: 'Times New Roman', Times, serif;
  }
}

.position-summary {
  margin-top: 12px;
  padding: 12px;
  background: $bg-tertiary;
  border-radius: 4px;
  border: 1px solid $border-color;
  
  .summary-title {
    font-weight: 600;
    color: $text-primary;
    margin-bottom: 8px;
    font-family: 'Times New Roman', Times, serif;
  }
  
  .summary-content {
    display: flex;
    flex-direction: column;
    gap: 4px;
    
    .position-item {
      display: flex;
      justify-content: space-between;
      align-items: center;
      
      .label {
        color: $text-secondary;
        font-size: $font-size-sm;
        font-family: 'Times New Roman', Times, serif;
      }
      
      .value {
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
}

.quick-actions {
  margin-top: 12px;
  
  .el-button {
    width: 100%;
    font-family: 'Times New Roman', Times, serif;
  }
}
</style>