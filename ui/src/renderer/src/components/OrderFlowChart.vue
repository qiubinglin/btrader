<template>
  <div class="order-flow-chart">
    <div class="chart-header">
      <div class="symbol-selector">
        <el-select v-model="currentSymbol" @change="handleSymbolChange" size="small">
          <el-option label="BTC/USDT" value="BTC/USDT" />
          <el-option label="ETH/USDT" value="ETH/USDT" />
          <el-option label="BNB/USDT" value="BNB/USDT" />
        </el-select>
      </div>
      <div class="price-info">
        <span class="current-price" :class="priceChangeClass">
          {{ formatPrice(currentPrice) }}
        </span>
        <span class="price-change" :class="priceChangeClass">
          {{ formatPriceChange(priceChange) }} ({{ formatPercent(priceChangePercent) }})
        </span>
      </div>
      <div class="chart-controls">
        <el-button-group size="small">
          <el-button :type="timeframe === '1m' ? 'primary' : ''" @click="setTimeframe('1m')">1分</el-button>
          <el-button :type="timeframe === '5m' ? 'primary' : ''" @click="setTimeframe('5m')">5分</el-button>
          <el-button :type="timeframe === '15m' ? 'primary' : ''" @click="setTimeframe('15m')">15分</el-button>
          <el-button :type="timeframe === '1h' ? 'primary' : ''" @click="setTimeframe('1h')">1小时</el-button>
        </el-button-group>
      </div>
    </div>
    <div class="chart-container" ref="chartContainer">
      <v-chart 
        :option="chartOption" 
        :autoresize="true"
        @click="handleChartClick"
        style="height: 100%; width: 100%;"
      />
    </div>
  </div>
</template>

<script setup lang="ts">
import { ref, computed, onMounted, onUnmounted, nextTick, watch } from 'vue'
import { useStore } from 'vuex'
import VChart from 'vue-echarts'
import { use } from 'echarts/core'
import { CanvasRenderer } from 'echarts/renderers'
import { CandlestickChart, LineChart, BarChart } from 'echarts/charts'
import {
  TitleComponent,
  TooltipComponent,
  LegendComponent,
  GridComponent,
  DataZoomComponent,
  ToolboxComponent
} from 'echarts/components'

use([
  CanvasRenderer,
  CandlestickChart,
  LineChart,
  BarChart,
  TitleComponent,
  TooltipComponent,
  LegendComponent,
  GridComponent,
  DataZoomComponent,
  ToolboxComponent
])

const store = useStore()
const chartContainer = ref<HTMLElement>()
const timeframe = ref('1m')

const currentSymbol = computed({
  get: () => store.state.market.currentSymbol,
  set: (value) => store.dispatch('market/setCurrentSymbol', value)
})

const currentPrice = computed(() => store.getters['market/currentPrice'])
const priceChange = computed(() => store.getters['market/priceChange'])
const priceChangePercent = computed(() => store.getters['market/priceChangePercent'])
const klineData = computed(() => store.state.market.klineData)
const marketData = computed(() => store.state.market.marketData)

const priceChangeClass = computed(() => {
  if (priceChange.value > 0) return 'text-up'
  if (priceChange.value < 0) return 'text-down'
  return 'text-equal'
})

const chartOption = computed(() => {
  const klines = klineData.value.map(k => [
    k.timestamp,
    k.open,
    k.close,
    k.low,
    k.high,
    k.volume
  ])

  const volumes = klineData.value.map((k, index) => [
    index,
    k.volume,
    k.close >= k.open ? 1 : -1
  ])

  return {
    backgroundColor: 'transparent',
    animation: false,
    grid: [
      {
        left: '10%',
        right: '8%',
        top: '10%',
        height: '60%'
      },
      {
        left: '10%',
        right: '8%',
        top: '75%',
        height: '16%'
      }
    ],
    xAxis: [
      {
        type: 'category',
        data: klineData.value.map((_, index) => index),
        boundaryGap: false,
        axisLine: { onZero: false },
        splitLine: { show: false },
        min: 'dataMin',
        max: 'dataMax',
        axisLabel: {
          formatter: function(value: number) {
            if (klineData.value[value]) {
              return new Date(klineData.value[value].timestamp).toLocaleTimeString()
            }
            return ''
          },
          color: '#cccccc'
        }
      },
      {
        type: 'category',
        gridIndex: 1,
        data: klineData.value.map((_, index) => index),
        boundaryGap: false,
        axisLine: { onZero: false },
        axisTick: { show: false },
        splitLine: { show: false },
        axisLabel: { show: false },
        min: 'dataMin',
        max: 'dataMax'
      }
    ],
    yAxis: [
      {
        scale: true,
        position: 'right',
        axisLabel: {
          color: '#cccccc',
          formatter: (value: number) => formatPrice(value)
        },
        splitLine: {
          lineStyle: {
            color: '#404040'
          }
        }
      },
      {
        scale: true,
        gridIndex: 1,
        position: 'right',
        axisLabel: {
          color: '#cccccc'
        },
        splitLine: {
          lineStyle: {
            color: '#404040'
          }
        }
      }
    ],
    dataZoom: [
      {
        type: 'inside',
        xAxisIndex: [0, 1],
        start: 70,
        end: 100
      },
      {
        show: true,
        xAxisIndex: [0, 1],
        type: 'slider',
        bottom: '1%',
        start: 70,
        end: 100,
        height: 20
      }
    ],
    series: [
      {
        name: 'K线',
        type: 'candlestick',
        data: klines,
        itemStyle: {
          color: '#00c853',
          color0: '#ff1744',
          borderColor: '#00c853',
          borderColor0: '#ff1744'
        },
        markPoint: {
          label: {
            formatter: function(param: any) {
              return param != null ? Math.round(param.value) + '' : ''
            }
          },
          data: [
            {
              name: 'Mark',
              coord: ['2013/5/31', 2300],
              value: 2300,
              itemStyle: {
                color: 'rgb(41,60,85)'
              }
            }
          ],
          tooltip: {
            formatter: function(param: any) {
              return param.name + '<br>' + (param.data.coord || '')
            }
          }
        }
      },
      {
        name: '成交量',
        type: 'bar',
        xAxisIndex: 1,
        yAxisIndex: 1,
        data: volumes,
        itemStyle: {
          color: function(params: any) {
            return params.data[2] > 0 ? '#00c853' : '#ff1744'
          }
        }
      }
    ],
    tooltip: {
      trigger: 'axis',
      axisPointer: {
        type: 'cross'
      },
      backgroundColor: '#2d2d2d',
      borderColor: '#404040',
      textStyle: {
        color: '#ffffff'
      },
      formatter: function(params: any) {
        const klineParam = params[0]
        if (klineParam && klineParam.data) {
          const data = klineParam.data
          const timestamp = new Date(data[0]).toLocaleString()
          return `
            <div>时间: ${timestamp}</div>
            <div>开盘: ${formatPrice(data[1])}</div>
            <div>收盘: ${formatPrice(data[2])}</div>
            <div>最低: ${formatPrice(data[3])}</div>
            <div>最高: ${formatPrice(data[4])}</div>
            <div>成交量: ${data[5]}</div>
          `
        }
        return ''
      }
    }
  }
})

const formatPrice = (price: number): string => {
  return price.toFixed(2)
}

const formatPriceChange = (change: number): string => {
  const sign = change >= 0 ? '+' : ''
  return `${sign}${change.toFixed(2)}`
}

const formatPercent = (percent: number): string => {
  const sign = percent >= 0 ? '+' : ''
  return `${sign}${percent.toFixed(2)}%`
}

const handleSymbolChange = (symbol: string) => {
  store.dispatch('market/setCurrentSymbol', symbol)
}

const setTimeframe = (tf: string) => {
  timeframe.value = tf
  // 这里可以发送请求到后端更新K线数据的时间周期
}

const handleChartClick = (params: any) => {
  if (params.componentType === 'series' && params.seriesType === 'candlestick') {
    const price = params.data[2] // 收盘价
    store.dispatch('trading/updateOrderForm', { price })
  }
}

onMounted(() => {
  nextTick(() => {
    // 图表初始化完成后的处理
  })
})
</script>

<style lang="scss" scoped>
.order-flow-chart {
  height: 100%;
  display: flex;
  flex-direction: column;
  background: $bg-secondary;
}

.chart-header {
  height: 40px;
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 0 16px;
  background: $bg-tertiary;
  border-bottom: 1px solid $border-color;
  
  .symbol-selector {
    min-width: 120px;
  }
  
  .price-info {
    display: flex;
    align-items: center;
    gap: 12px;
    
    .current-price {
      font-size: $font-size-lg;
      font-weight: bold;
      font-family: 'Courier New', monospace;
    }
    
    .price-change {
      font-size: $font-size-sm;
      font-family: 'Courier New', monospace;
    }
  }
  
  .chart-controls {
    display: flex;
    align-items: center;
  }
}

.chart-container {
  flex: 1;
  padding: 8px;
}
</style>