<template>
  <div class="order-list">
    <div class="order-header">
      <div class="header-title">委托订单</div>
      <div class="header-tabs">
        <el-radio-group v-model="activeFilter" size="small">
          <el-radio-button label="active">活跃</el-radio-button>
          <el-radio-button label="completed">已完成</el-radio-button>
          <el-radio-button label="all">全部</el-radio-button>
        </el-radio-group>
      </div>
      <div class="header-actions">
        <el-button size="small" @click="refreshOrders" :loading="isRefreshing">刷新</el-button>
        <el-button size="small" @click="cancelAllOrders" type="danger" v-if="hasActiveOrders">
          全部撤单
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
        empty-text="暂无订单"
      >
        <el-table-column prop="orderId" label="订单号" width="120" align="center">
          <template #default="{ row }">
            <span class="order-id-cell">{{ row.orderId.slice(-8) }}</span>
          </template>
        </el-table-column>
        
        <el-table-column prop="symbol" label="品种" width="100" align="center">
          <template #default="{ row }">
            <span class="symbol-cell">{{ row.symbol }}</span>
          </template>
        </el-table-column>
        
        <el-table-column prop="side" label="方向" width="60" align="center">
          <template #default="{ row }">
            <span class="side-cell" :class="row.side">
              {{ row.side === 'buy' ? '买' : '卖' }}
            </span>
          </template>
        </el-table-column>
        
        <el-table-column prop="volume" label="数量" width="100" align="right">
          <template #default="{ row }">
            <span class="volume-cell">{{ formatVolume(row.volume) }}</span>
          </template>
        </el-table-column>
        
        <el-table-column prop="filledVolume" label="已成交" width="100" align="right">
          <template #default="{ row }">
            <span class="filled-cell" :class="getFilledClass(row)">
              {{ formatVolume(row.filledVolume) }}
            </span>
          </template>
        </el-table-column>
        
        <el-table-column prop="price" label="价格" width="100" align="right">
          <template #default="{ row }">
            <span class="price-cell">{{ formatPrice(row.price) }}</span>
          </template>
        </el-table-column>
        
        <el-table-column prop="status" label="状态" width="80" align="center">
          <template #default="{ row }">
            <el-tag :type="getStatusType(row.status)" size="small">
              {{ getStatusText(row.status) }}
            </el-tag>
          </template>
        </el-table-column>
        
        <el-table-column prop="timestamp" label="时间" width="140" align="center">
          <template #default="{ row }">
            <span class="time-cell">{{ formatTime(row.timestamp) }}</span>
          </template>
        </el-table-column>
        
        <el-table-column label="操作" width="100" align="center" fixed="right">
          <template #default="{ row }">
            <div class="action-buttons">
              <el-button 
                size="small" 
                type="danger" 
                @click="cancelOrder(row)"
                :disabled="!canCancel(row.status)"
                v-if="canCancel(row.status)"
              >
                撤单
              </el-button>
              <el-button 
                size="small" 
                type="primary" 
                @click="modifyOrder(row)"
                :disabled="!canModify(row.status)"
                v-if="canModify(row.status)"
              >
                改单
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
      return '等待'
    case 'partial':
      return '部分'
    case 'filled':
      return '已成'
    case 'cancelled':
      return '已撤'
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
    ElMessage.success('订单数据已刷新')
  } catch (error: any) {
    ElMessage.error(`刷新失败: ${error.message}`)
  } finally {
    isRefreshing.value = false
  }
}

const cancelOrder = async (order: any) => {
  try {
    await ElMessageBox.confirm(
      `确认撤销订单 ${order.orderId.slice(-8)}?`,
      '确认撤单',
      {
        confirmButtonText: '确认',
        cancelButtonText: '取消',
        type: 'warning'
      }
    )
    
    const result = await store.dispatch('trading/cancelOrder', order.orderId)
    ElMessage.success(`撤单成功: ${result.orderId}`)
    
    // 刷新订单数据
    await refreshOrders()
  } catch (error: any) {
    if (error !== 'cancel') {
      ElMessage.error(`撤单失败: ${error.message}`)
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
  
  ElMessage.info('订单信息已填入交易面板，请修改后重新提交')
  
  // 同时撤销原订单
  cancelOrder(order)
}

const cancelAllOrders = async () => {
  if (activeOrders.value.length === 0) return
  
  try {
    await ElMessageBox.confirm(
      `确认撤销所有 ${activeOrders.value.length} 个活跃订单?`,
      '确认全部撤单',
      {
        confirmButtonText: '确认',
        cancelButtonText: '取消',
        type: 'warning'
      }
    )
    
    // 逐个撤销订单
    for (const order of activeOrders.value) {
      try {
        await store.dispatch('trading/cancelOrder', order.orderId)
      } catch (error) {
        console.error(`撤销订单 ${order.orderId} 失败:`, error)
      }
    }
    
    ElMessage.success('全部撤单请求已提交')
    await refreshOrders()
  } catch (error: any) {
    if (error !== 'cancel') {
      ElMessage.error(`全部撤单失败: ${error.message}`)
    }
  }
}
</script>

<style lang="scss" scoped>
.order-list {
  height: 100%;
  display: flex;
  flex-direction: column;
}

.order-header {
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
  
  .header-tabs {
    .el-radio-group {
      :deep(.el-radio-button__inner) {
        font-size: $font-size-xs;
        padding: 4px 8px;
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
  color: $text-secondary;
}

.symbol-cell {
  font-weight: bold;
  color: $text-primary;
}

.side-cell {
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

.volume-cell, .filled-cell, .price-cell {
  font-family: 'Courier New', monospace;
  font-weight: bold;
}

.time-cell {
  font-family: 'Courier New', monospace;
  font-size: $font-size-xs;
}

.action-buttons {
  display: flex;
  gap: 4px;
  justify-content: center;
  
  .el-button {
    font-size: $font-size-xs;
    padding: 2px 6px;
  }
}
</style>