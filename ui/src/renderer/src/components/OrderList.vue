<template>
  <div class="order-list">
    <div class="order-header">
      <div class="header-title">Orders</div>
      <div class="header-tabs">
        <el-radio-group v-model="activeFilter" size="small">
          <el-radio-button label="active">Active</el-radio-button>
          <el-radio-button label="completed">Completed</el-radio-button>
          <el-radio-button label="all">All</el-radio-button>
        </el-radio-group>
      </div>
      <div class="header-actions">
        <el-button size="small" @click="refreshOrders" :loading="isRefreshing">Refresh</el-button>
        <el-button size="small" @click="cancelAllOrders" type="danger" v-if="hasActiveOrders">
          Cancel All
        </el-button>
      </div>
    </div>
    
    <div class="order-table-container">
      <el-table
        :data="filteredOrders"
        height="100%"
        size="small"
        :show-header="true"
        stripe
        empty-text="No Orders"
      >
        <el-table-column prop="orderId" label="Order ID" width="120" align="center">
          <template #default="{ row }">
            <span class="order-id-cell">{{ row.orderId.slice(-8) }}</span>
          </template>
        </el-table-column>
        
        <el-table-column prop="symbol" label="Symbol" width="100" align="center">
          <template #default="{ row }">
            <span class="symbol-cell">{{ row.symbol }}</span>
          </template>
        </el-table-column>
        
        <el-table-column prop="side" label="Side" width="60" align="center">
          <template #default="{ row }">
            <span class="side-cell" :class="row.side">
              {{ row.side === 'buy' ? 'Buy' : 'Sell' }}
            </span>
          </template>
        </el-table-column>
        
        <el-table-column prop="volume" label="Volume" width="100" align="right">
          <template #default="{ row }">
            <span class="volume-cell">{{ formatVolume(row.volume) }}</span>
          </template>
        </el-table-column>
        
        <el-table-column prop="filledVolume" label="Filled" width="100" align="right">
          <template #default="{ row }">
            <span class="filled-cell" :class="getFilledClass(row)">
              {{ formatVolume(row.filledVolume) }}
            </span>
          </template>
        </el-table-column>
        
        <el-table-column prop="price" label="Price" width="100" align="right">
          <template #default="{ row }">
            <span class="price-cell">{{ formatPrice(row.price) }}</span>
          </template>
        </el-table-column>
        
        <el-table-column prop="status" label="Status" width="80" align="center">
          <template #default="{ row }">
            <el-tag :type="getStatusType(row.status)" size="small">
              {{ getStatusText(row.status) }}
            </el-tag>
          </template>
        </el-table-column>
        
        <el-table-column prop="timestamp" label="Time" width="140" align="center">
          <template #default="{ row }">
            <span class="time-cell">{{ formatTime(row.timestamp) }}</span>
          </template>
        </el-table-column>
        
        <el-table-column label="Actions" width="100" align="center" fixed="right">
          <template #default="{ row }">
            <div class="action-buttons">
              <el-button 
                size="small" 
                type="danger" 
                @click="cancelOrder(row)"
                :disabled="!canCancel(row.status)"
                v-if="canCancel(row.status)"
              >
                Cancel
              </el-button>
              <el-button 
                size="small" 
                type="primary" 
                @click="modifyOrder(row)"
                :disabled="!canModify(row.status)"
                v-if="canModify(row.status)"
              >
                Modify
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
const activeFilter = ref('active')

const orders = computed(() => store.state.account.orders)
const activeOrders = computed(() => store.getters['account/activeOrders'])
const completedOrders = computed(() => store.getters['account/completedOrders'])

const filteredOrders = computed(() => {
  switch (activeFilter.value) {
    case 'active':
      return activeOrders.value
    case 'completed':
      return completedOrders.value
    default:
      return orders.value
  }
})

const hasActiveOrders = computed(() => {
  return activeOrders.value.length > 0
})

const formatVolume = (volume: number): string => {
  return volume.toFixed(4)
}

const formatPrice = (price: number): string => {
  return price.toFixed(2)
}

const formatTime = (timestamp: number): string => {
  const date = new Date(timestamp)
  return `${date.getMonth() + 1}/${date.getDate()} ${date.getHours().toString().padStart(2, '0')}:${date.getMinutes().toString().padStart(2, '0')}:${date.getSeconds().toString().padStart(2, '0')}`
}

const getFilledClass = (row: any): string => {
  const fillPercent = row.volume > 0 ? (row.filledVolume / row.volume) : 0
  if (fillPercent === 0) return 'text-muted'
  if (fillPercent < 1) return 'text-warning'
  return 'text-success'
}

const getStatusType = (status: string): string => {
  switch (status) {
    case 'pending':
      return 'info'
    case 'partial':
      return 'warning'
    case 'filled':
      return 'success'
    case 'cancelled':
      return 'danger'
    default:
      return ''
  }
}

const getStatusText = (status: string): string => {
  switch (status) {
    case 'pending':
      return 'Waiting'
    case 'partial':
      return 'Partial'
    case 'filled':
      return 'Filled'
    case 'cancelled':
      return 'Cancelled'
    default:
      return status
  }
}

const canCancel = (status: string): boolean => {
  return status === 'pending' || status === 'partial'
}

const canModify = (status: string): boolean => {
  return status === 'pending'
}

const refreshOrders = async () => {
  isRefreshing.value = true
  try {
    await store.dispatch('account/refreshOrders')
    ElMessage.success('Order data refreshed')
  } catch (error: any) {
    ElMessage.error(`Failed to refresh: ${error.message}`)
  } finally {
    isRefreshing.value = false
  }
}

const cancelOrder = async (order: any) => {
  try {
    await ElMessageBox.confirm(
      `Confirm cancel order ${order.orderId.slice(-8)}?`,
      'Confirm Cancel',
      {
        confirmButtonText: 'Confirm',
        cancelButtonText: 'Cancel',
        type: 'warning'
      }
    )
    
    const result = await store.dispatch('trading/cancelOrder', order.orderId)
    ElMessage.success(`Cancelled successfully: ${result.orderId}`)
    
    // 刷新订单数据
    await refreshOrders()
  } catch (error: any) {
    if (error !== 'cancel') {
      ElMessage.error(`Failed to cancel: ${error.message}`)
    }
  }
}

const modifyOrder = (order: any) => {
  // 将订单信息填入交易面板，用于改单
  store.dispatch('trading/updateOrderForm', {
    symbol: order.symbol,
    side: order.side,
    volume: order.volume - order.filledVolume,
    price: order.price,
    orderType: 'limit'
  })
  
  ElMessage.info('Order information has been filled into the trading panel, please modify and resubmit')
  
  // 同时撤销原订单
  cancelOrder(order)
}

const cancelAllOrders = async () => {
  if (activeOrders.value.length === 0) return
  
  try {
    await ElMessageBox.confirm(
      `Confirm cancel all ${activeOrders.value.length} active orders?`,
      'Confirm All Cancel',
      {
        confirmButtonText: 'Confirm',
        cancelButtonText: 'Cancel',
        type: 'warning'
      }
    )
    
    // 逐个撤销订单
    for (const order of activeOrders.value) {
      try {
        await store.dispatch('trading/cancelOrder', order.orderId)
      } catch (error) {
        console.error(`Failed to cancel order ${order.orderId}:`, error)
      }
    }
    
    ElMessage.success('All cancel requests submitted')
    await refreshOrders()
  } catch (error: any) {
    if (error !== 'cancel') {
      ElMessage.error(`Failed to cancel all: ${error.message}`)
    }
  }
}
</script>

<style lang="scss" scoped>
.order-list {
  height: 100%;
  display: flex;
  flex-direction: column;
  background: $bg-secondary;
  border-radius: 4px;
  border: 1px solid $border-color;
  font-family: 'Times New Roman', Times, serif;
}

.order-header {
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
  
  .header-tabs {
    .el-radio-group {
      .el-radio-button__inner {
        font-family: 'Times New Roman', Times, serif;
      }
    }
  }
  
  .header-actions {
    display: flex;
    gap: 8px;
  }
}

.order-table-container {
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
          padding: 6px 0;
          font-size: $font-size-sm;
          border-bottom: 1px solid rgba(255, 255, 255, 0.05);
        }
      }
    }
  }
}

.order-id-cell {
  font-family: 'Courier New', monospace;
  font-size: $font-size-xs;
}

.symbol-cell {
  font-weight: bold;
  font-family: 'Times New Roman', Times, serif;
}

.side-cell {
  font-weight: bold;
  padding: 2px 6px;
  border-radius: 2px;
  font-size: $font-size-xs;
  font-family: 'Times New Roman', Times, serif;
  
  &.buy {
    background: $buy-bg;
    color: $buy-color;
  }
  
  &.sell {
    background: $sell-bg;
    color: $sell-color;
  }
}

.volume-cell, .filled-cell, .price-cell {
  font-family: 'Courier New', monospace;
}

.time-cell {
  font-family: 'Courier New', monospace;
  font-size: $font-size-xs;
}

.action-buttons {
  display: flex;
  gap: 4px;
  
  .el-button {
    font-size: $font-size-xs;
    font-family: 'Times New Roman', Times, serif;
  }
}
</style>