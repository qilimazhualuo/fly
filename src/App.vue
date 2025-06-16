<script setup>
import { ref, reactive, onMounted, onUnmounted } from 'vue'
import StatusBar from './components/StatusBar.vue'
import HorizonDisplay from './components/HorizonDisplay.vue'
import FlightControls from './components/FlightControls.vue'
import VirtualJoystick from './components/VirtualJoystick.vue'
import ConfigPanel from './components/ConfigPanel.vue'
import MapPanel from './components/MapPanel.vue'

// 飞行数据状态
const flightData = reactive({
  // 姿态数据
  pitch: 0,        // 俯仰角
  roll: 0,         // 横滚角
  yaw: 0,          // 偏航角
  
  // 飞行参数
  altitude: 0,     // 高度 (米)
  speed: 0,        // 速度 (米/秒)
  heading: 0,      // 航向角
  
  // GPS数据
  latitude: 0,
  longitude: 0,
  satellites: 0,
  
  // 系统状态
  battery: 100,    // 电池电量 (%)
  signal: 100,     // 信号强度 (%)
  armed: false,    // 解锁状态
  connected: false // 连接状态
})

// 控制输入
const controls = reactive({
  throttle: 0,     // 油门 (0 to 100)
  aileron: 0,      // 副翼 (-100 to 100)
  elevator: 0,     // 升降舵 (-100 to 100)
  rudder: 0        // 方向舵 (-100 to 100)
})

// Tab 切换
const activeTab = ref('flight')
const tabs = [
  { id: 'flight', name: '飞行', icon: '✈️' },
  { id: 'config', name: '配置', icon: '⚙️' },
  { id: 'map', name: '地图', icon: '🗺️' }
]

// 飞行模式
const flightModes = ['MANUAL', 'STABILIZE', 'AUTO', 'RTL']
const currentMode = ref('MANUAL')

// 模拟数据更新
let dataInterval = null

onMounted(() => {
  // 模拟连接
  setTimeout(() => {
    flightData.connected = true
  }, 1000)

  // 模拟实时数据更新
  dataInterval = setInterval(() => {
    if (flightData.connected) {
      // 模拟飞行数据变化
      flightData.pitch += (Math.random() - 0.5) * 2
      flightData.roll += (Math.random() - 0.5) * 2
      flightData.yaw += (Math.random() - 0.5) * 1
      
      flightData.altitude += (Math.random() - 0.5) * 5
      flightData.speed = Math.max(0, flightData.speed + (Math.random() - 0.5) * 2)
      flightData.heading = (flightData.heading + (Math.random() - 0.5) * 2 + 360) % 360
      
      flightData.satellites = Math.floor(Math.random() * 12) + 4
      flightData.battery = Math.max(0, flightData.battery - 0.01)
      
      // 限制角度范围
      flightData.pitch = Math.max(-90, Math.min(90, flightData.pitch))
      flightData.roll = Math.max(-180, Math.min(180, flightData.roll))
    }
  }, 100)
})

onUnmounted(() => {
  if (dataInterval) {
    clearInterval(dataInterval)
  }
})

// 事件处理函数
const setFlightMode = (mode) => {
  currentMode.value = mode
}

const toggleArmed = () => {
  flightData.armed = !flightData.armed
}

const handleTabChange = (tabId) => {
  activeTab.value = tabId
}

// 摇杆控制事件
const handleLeftJoystick = ({ x, y }) => {
  // 左摇杆：x轴控制方向舵(rudder)，y轴控制油门(throttle)
  // nipplejs: x(-100到100), y(-100到100，上为正)
  controls.rudder = Math.max(-100, Math.min(100, x))
  controls.throttle = Math.max(0, Math.min(100, y)) // 油门只能是0-100
}

const handleRightJoystick = ({ x, y }) => {
  // 右摇杆：x轴控制副翼(aileron)，y轴控制升降舵(elevator)
  // nipplejs: x(-100到100), y(-100到100，上为正)
  controls.aileron = Math.max(-100, Math.min(100, x))
  controls.elevator = Math.max(-100, Math.min(100, -y)) // 反转y轴，因为飞控中上推是负值
}
</script>

<template>
  <main class="mobile-container">
    <!-- 顶部状态栏(包含Tab选择器) -->
    <StatusBar 
      :flight-data="flightData" 
      :current-mode="currentMode"
      :active-tab="activeTab"
      :tabs="tabs"
      @tab-change="handleTabChange"
    />

    <!-- 主要内容区域 -->
    <div class="main-content">
      <!-- 飞行数据面板 -->
      <div v-if="activeTab === 'flight'" class="flight-panel">
        <HorizonDisplay :flight-data="flightData" />
        <FlightControls 
          :flight-data="flightData"
          :current-mode="currentMode"
          :flight-modes="flightModes"
          @set-flight-mode="setFlightMode"
          @toggle-armed="toggleArmed"
        />
      </div>

      <!-- 配置面板 -->
      <ConfigPanel v-if="activeTab === 'config'" />

      <!-- 地图面板 -->
      <MapPanel v-if="activeTab === 'map'" :flight-data="flightData" />
    </div>

    <!-- 虚拟摇杆控制区域 -->
    <div class="joystick-area">
      <!-- 左摇杆 (油门/偏航) -->
      <VirtualJoystick
        position="left"
        :labels="{
          top: '油门',
          left: '左偏航',
          right: '右偏航'
        }"
        :display-values="{
          'THR': `${controls.throttle.toFixed(0)}%`,
          'YAW': controls.rudder.toFixed(0)
        }"
        @joystick-move="handleLeftJoystick"
      />

      <!-- 右摇杆 (副翼/升降舵) -->
      <VirtualJoystick
        position="right"
        :labels="{
          top: '拉升',
          bottom: '俯冲',
          left: '左滚',
          right: '右滚'
        }"
        :display-values="{
          'AIL': controls.aileron.toFixed(0),
          'ELE': controls.elevator.toFixed(0)
        }"
        @joystick-move="handleRightJoystick"
      />
    </div>
  </main>
</template>

<style lang="less">
body {
  margin: 0;
  padding: 0;
  font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
  background: #000;
  color: #ffffff;
  overflow: hidden;
  user-select: none;
  -webkit-user-select: none;
  touch-action: manipulation;
}

.mobile-container {
  position: relative;
  width: 100vw;
  height: 100vh;
  display: flex;
  flex-direction: column;
  background: linear-gradient(135deg, #1a1a1a 0%, #2d2d2d 100%);
  overflow: hidden;
}

.main-content {
  flex: 1;
  padding: 10px;
  overflow: hidden;
  padding-bottom: 20px; // 增加底部间距为摇杆留空间
}

.flight-panel {
  height: 100%;
  display: flex;
  flex-direction: column;
  gap: 8px;
  overflow-y: auto;
}

.joystick-area {
  position: fixed;
  bottom: 20px; // 移到真正的底部
  left: 0;
  right: 0;
  height: 140px;
  display: flex;
  justify-content: space-between;
  align-items: flex-end;
  padding: 10px 20px;
  background: transparent;
  pointer-events: none;
  z-index: 100;
}
</style>