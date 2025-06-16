<template>
    <div class="map-container">
      <div class="map-header">
        <h3>飞行地图</h3>
        <div class="map-controls">
          <button @click="centerOnAircraft" class="map-btn">
            居中飞机
          </button>
          <button @click="togglePath" class="map-btn">
            {{ showPath ? '隐藏航线' : '显示航线' }}
          </button>
        </div>
      </div>
      
      <div class="map-display">
        <div class="map-grid">
          <!-- 简化的地图网格 -->
          <div 
            v-for="i in 20" 
            :key="`h-${i}`" 
            class="grid-line horizontal"
            :style="{ top: `${i * 5}%` }"
          ></div>
          <div 
            v-for="i in 20" 
            :key="`v-${i}`" 
            class="grid-line vertical"
            :style="{ left: `${i * 5}%` }"
          ></div>
        </div>
        
        <!-- 飞机位置 -->
        <div 
          class="aircraft-position"
          :style="{
            left: `${aircraftX}%`,
            top: `${aircraftY}%`,
            transform: `translate(-50%, -50%) rotate(${heading}deg)`
          }"
        >
          <div class="aircraft-icon">✈</div>
        </div>
        
        <!-- 飞行路径 -->
        <svg v-if="showPath" class="flight-path">
          <polyline 
            :points="pathPoints" 
            stroke="#00ff00" 
            stroke-width="2" 
            fill="none"
          />
        </svg>
        
        <!-- 航点 -->
        <div 
          v-for="(waypoint, index) in waypoints" 
          :key="`wp-${index}`"
          class="waypoint"
          :style="{
            left: `${waypoint.x}%`,
            top: `${waypoint.y}%`
          }"
        >
          <div class="waypoint-marker">{{ index + 1 }}</div>
        </div>
      </div>
      
      <div class="map-info">
        <div class="coord-display">
          <span>纬度: {{ latitude.toFixed(6) }}°</span>
          <span>经度: {{ longitude.toFixed(6) }}°</span>
          <span>航向: {{ heading.toFixed(0) }}°</span>
        </div>
      </div>
    </div>
  </template>
  
  <script setup>
  import { ref, computed, watch } from 'vue'
  
  const props = defineProps({
    latitude: Number,
    longitude: Number,
    heading: Number
  })
  
  const showPath = ref(true)
  const flightPath = ref([])
  const waypoints = ref([
    { x: 25, y: 25, name: 'WP1' },
    { x: 50, y: 40, name: 'WP2' },
    { x: 75, y: 30, name: 'WP3' },
    { x: 60, y: 70, name: 'WP4' }
  ])
  
  // 将GPS坐标转换为屏幕坐标（简化版）
  const aircraftX = computed(() => {
    return ((props.longitude + 180) / 360) * 100
  })
  
  const aircraftY = computed(() => {
    return ((90 - props.latitude) / 180) * 100
  })
  
  // 记录飞行路径
  watch([() => props.latitude, () => props.longitude], () => {
    flightPath.value.push({
      x: aircraftX.value,
      y: aircraftY.value
    })
    
    // 限制路径点数量
    if (flightPath.value.length > 100) {
      flightPath.value.shift()
    }
  })
  
  const pathPoints = computed(() => {
    return flightPath.value.map(point => `${point.x},${point.y}`).join(' ')
  })
  
  const centerOnAircraft = () => {
    // 模拟地图居中功能
    console.log('居中到飞机位置')
  }
  
  const togglePath = () => {
    showPath.value = !showPath.value
  }
  </script>
  
  <style lang="less" scoped>
  .map-container {
    height: 100%;
    display: flex;
    flex-direction: column;
    background: #2a2a2a;
    border-radius: 10px;
    overflow: hidden;
  }
  
  .map-header {
    padding: 15px;
    background: #333;
    display: flex;
    justify-content: space-between;
    align-items: center;
    
    h3 {
      margin: 0;
      color: #4CAF50;
      font-size: 16px;
    }
    
    .map-controls {
      display: flex;
      gap: 10px;
      
      .map-btn {
        padding: 6px 12px;
        background: #444;
        color: white;
        border: none;
        border-radius: 4px;
        font-size: 12px;
        cursor: pointer;
        transition: all 0.3s;
        
        &:hover {
          background: #555;
        }
      }
    }
  }
  
  .map-display {
    flex: 1;
    position: relative;
    background: #1a1a1a;
    overflow: hidden;
  }
  
  .map-grid {
    position: absolute;
    width: 100%;
    height: 100%;
    
    .grid-line {
      position: absolute;
      background: rgba(255, 255, 255, 0.1);
      
      &.horizontal {
        width: 100%;
        height: 1px;
      }
      
      &.vertical {
        height: 100%;
        width: 1px;
      }
    }
  }
  
  .aircraft-position {
    position: absolute;
    z-index: 10;
    
    .aircraft-icon {
      font-size: 20px;
      color: #ff4444;
      text-shadow: 0 0 10px #ff4444;
    }
  }
  
  .flight-path {
    position: absolute;
    width: 100%;
    height: 100%;
    top: 0;
    left: 0;
    pointer-events: none;
  }
  
  .waypoint {
    position: absolute;
    z-index: 5;
    
    .waypoint-marker {
      width: 20px;
      height: 20px;
      background: #2196F3;
      border-radius: 50%;
      display: flex;
      align-items: center;
      justify-content: center;
      color: white;
      font-size: 12px;
      font-weight: bold;
      border: 2px solid #fff;
      transform: translate(-50%, -50%);
    }
  }
  
  .map-info {
    padding: 10px 15px;
    background: #333;
    border-top: 1px solid #555;
    
    .coord-display {
      display: flex;
      justify-content: space-between;
      font-size: 12px;
      color: #ccc;
      
      span {
        font-family: 'Courier New', monospace;
      }
    }
  }
  </style>