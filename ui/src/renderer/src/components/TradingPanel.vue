<template>
  <div class="trading-panel">
    <div class="panel-header">
      <div class="header-title">交易面板</div>
      <div class="header-actions">
        <el-button size="small" @click="resetForm">重置</el-button>
      </div>
    </div>
    
    <div class="panel-content">
      <el-form :model="orderForm" label-width="60px" size="small">
        <!-- 交易品种 -->
        <el-form-item label="品种">
          <el-select v-model="orderForm.symbol" style="width: 100%">
            <el-option label="BTC/USDT" value="BTC/USDT" />
            <el-option label="ETH/USDT" value="ETH/USDT" />
            <el-option label="BNB/USDT" value="BNB/USDT" />
          </el-select>
        </el-form-item>
        
        <!-- 订单类型 -->
        <el-form-item label="类型">
          <el-select v-model="orderForm.orderType" style="width: 100%">
            <el-option label="限价单" value="limit" />
            <el-option label="市价单" value="market" />
          </el-select>
        </el-form-item>
        
        <!-- 买卖方向 -->
        <el-form-item label="方向">
          <el-radio-group v-model="orderForm.side" style="width: 100%">
            <el-radio-button label="buy" style="width: 50%">
              <span style="color: #00c853">买入</span>
            </el-radio-button>
            <el-radio-button label="sell" style="width: 50%">
              <span style="color: #ff1744">卖出</span>
            </el-radio-button>
          </el-radio-group>
        </el-form-item>
        
        <!-- 价格 -->
        <el-form-item label="价格" v-if="orderForm.orderType === 'limit'">
          <div class="price-input-container">
            <el-input-number
              v-model="orderForm.price"
              :precision="2"
              :step="0.01"
              style="width: 100%"
              placeholder="请输入价格"
            />
            <div class="quick-price-buttons">
              <el-button size="small" @click="setMarketPrice">市价</el-button>
              <el-button size="small" @click="setBestBid" v-if="bestBid">买一</el-button>
              <el-button size="small" @click="setBestAsk" v-if="bestAsk">卖一</el-button>
            </div>
          </div>
        </el-form-item>
        
        <!-- 数量 -->
        <el-form-item label="数量">
          <div class="volume-input-container">
            <el-input-number
              v-model="orderForm.volume"
              :precision="4"
              :step="0.001"
              :min="0.001"
              style="width: 100%"
              placeholder="请输入数量"
            />
            <div class="quick-volume-buttons">
              <el-button size="small" @click="setVolumePercent(25)">25%</el-button>
              <el-button size="small" @click="setVolumePercent(50)">50%</el-button>
              <el-button size="small" @click="setVolumePercent(75)">75%</el-button>
              <el-button size="small" @click="setVolumePercent(100)">最大</el-button>
            </div>
          </div>
        </el-form-item>
        
        <!-- 预估金额 -->
        <el-form-item label="金额">
          <div class="amount-display">
            <span class="amount-value">{{ formatAmount(estimatedAmount) }}</span>
            <span class="amount-currency">USDT</span>
          </div>
        </el-form-item>
        
        <!-- 提交按钮 -->
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
            {{ orderForm.side === 'buy' ? '买入' : '卖出' }} {{ orderForm.symbol }}
          </el-button>
        </el-form-item>
      </el-form>
      
      <!-- 持仓信息快显 -->
      <div class="position-summary" v-if="currentPosition">
        <div class="summary-title">当前持仓</div>
        <div class="summary-content">
          <div class="position-item">
            <span class="label">数量:</span>
            <span class="value" :class="currentPosition.volume > 0 ? 'text-up' : 'text-down'">
              {{ formatVolume(currentPosition.volume) }}
            </span>
          </div>
          <div class="position-item">
            <span class="label">均价:</span>
            <span class="value">{{ formatPrice(currentPosition.avgPrice) }}</span>
          </div>
          <div class="position-item">
            <span class="label">盈亏:</span>
            <span class="value" :class="currentPosition.unrealizedPnl >= 0 ? 'text-up' : 'text-down'">
              {{ formatPnl(currentPosition.unrealizedPnl) }}
            </span>
          </div>
        </div>
      </div>
      
      <!-- 快速平仓按钮 -->
      <div class="quick-actions" v-if="currentPosition && currentPosition.volume !== 0">
        <el-button
          type="warning"
          style="width: 100%"
          @click="quickClose"
          :loading="isSubmitting"
        >
          快速平仓
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
    const confirmText = `确认${orderForm.value.side === 'buy' ? '买入' : '卖出'} ${orderForm.value.volume} ${orderForm.value.symbol}?`
    await ElMessageBox.confirm(confirmText, '确认交易', {
      confirmButtonText: '确认',
      cancelButtonText: '取消',
      type: 'warning'
    })
    
    const result = await store.dispatch('trading/submitOrder')
    ElMessage.success(`订单提交成功: ${result.orderId}`)
    
    // 重置表单
    resetForm()
  } catch (error: any) {
    if (error !== 'cancel') {
      ElMessage.error(`订单提交失败: ${error.message || error}`)
    }
  }
}

const quickClose = async () => {
  if (!currentPosition.value) return
  
  try {
    await ElMessageBox.confirm(`确认平仓 ${Math.abs(currentPosition.value.volume)} ${orderForm.value.symbol}?`, '确认平仓', {
      confirmButtonText: '确认',
      cancelButtonText: '取消',
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
    ElMessage.success(`平仓订单提交成功: ${result.orderId}`)
    
    resetForm()
  } catch (error: any) {
    if (error !== 'cancel') {
      ElMessage.error(`平仓失败: ${error.message || error}`)
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
}

.panel-header {
  height: 40px;
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 0 16px;
  background: $bg-tertiary;
  border-bottom: 1px solid $border-color;
  
  .header-title {
    font-weight: 600;
    color: $text-primary;
  }
}

.panel-content {
  flex: 1;
  padding: 16px;
  overflow-y: auto;
}

.price-input-container, .volume-input-container {
  .quick-price-buttons, .quick-volume-buttons {
    display: flex;
    gap: 4px;
    margin-top: 8px;
    
    .el-button {
      flex: 1;
      font-size: $font-size-xs;
    }
  }
}

.amount-display {
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 8px 12px;
  background: rgba(255, 255, 255, 0.05);
  border-radius: 4px;
  font-family: 'Courier New', monospace;
  
  .amount-value {
    font-size: $font-size-lg;
    font-weight: bold;
    color: $text-primary;
  }
  
  .amount-currency {
    font-size: $font-size-sm;
    color: $text-secondary;
  }
}

.position-summary {
  margin-top: 16px;
  padding: 12px;
  background: rgba(255, 255, 255, 0.02);
  border-radius: 4px;
  border: 1px solid $border-color;
  
  .summary-title {
    font-size: $font-size-sm;
    font-weight: bold;
    color: $text-primary;
    margin-bottom: 8px;
    border-bottom: 1px solid $border-color;
    padding-bottom: 4px;
  }
  
  .summary-content {
    .position-item {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 4px;
      font-size: $font-size-sm;
      
      .label {
        color: $text-secondary;
      }
      
      .value {
        font-family: 'Courier New', monospace;
        font-weight: bold;
      }
    }
  }
}

.quick-actions {
  margin-top: 12px;
}

:deep(.el-form-item) {
  margin-bottom: 16px;
  
  .el-form-item__label {
    font-size: $font-size-sm;
  }
}

:deep(.el-radio-button__inner) {
  width: 100%;
  text-align: center;
}
</style>