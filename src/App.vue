<script setup>
import { ref, reactive, onMounted, onUnmounted, computed } from 'vue'

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

// 虚拟摇杆状态
const leftJoystick = reactive({
  x: 0,  // 油门/偏航
  y: 0   // 油门
})

const rightJoystick = reactive({
  x: 0,  // 副翼
  y: 0   // 升降舵
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

// 控制函数
const setFlightMode = (mode) => {
  currentMode.value = mode
}

const toggleArmed = () => {
  flightData.armed = !flightData.armed
}

// 虚拟摇杆控制
const handleJoystickMove = (stick, event) => {
  const rect = event.currentTarget.getBoundingClientRect()
  const centerX = rect.width / 2
  const centerY = rect.height / 2
  
  const touch = event.touches ? event.touches[0] : event
  const x = touch.clientX - rect.left - centerX
  const y = touch.clientY - rect.top - centerY
  
  const distance = Math.sqrt(x * x + y * y)
  const maxDistance = Math.min(centerX, centerY) - 20
  
  if (distance <= maxDistance) {
    stick.x = (x / maxDistance) * 100
    stick.y = -(y / maxDistance) * 100
  } else {
    const angle = Math.atan2(y, x)
    stick.x = Math.cos(angle) * 100
    stick.y = -Math.sin(angle) * 100
  }
  
  // 更新控制值
  if (stick === leftJoystick) {
    controls.throttle = Math.max(0, stick.y)
    controls.rudder = stick.x
  } else {
    controls.aileron = stick.x
    controls.elevator = stick.y
  }
}

const resetJoystick = (stick) => {
  stick.x = 0
  stick.y = 0
  
  if (stick === leftJoystick) {
    controls.rudder = 0
    // 油门保持当前值
  } else {
    controls.aileron = 0
    controls.elevator = 0
  }
}

// 计算摇杆手柄位置
const getJoystickKnobStyle = (stick) => {
  const maxDistance = 45 // 最大移动距离，适配90px的摇杆
  const x = (stick.x / 100) * maxDistance
  const y = -(stick.y / 100) * maxDistance
  
  return {
    transform: `translate(${x}px, ${y}px)`
  }
}
</script>

<template>
    <main class="mobile-container">
        <!-- 顶部状态栏 -->
        <div class="status-bar">
            <div class="status-left">
                <span :class="['connection-status', flightData.connected ? 'connected' : 'disconnected']">
                    {{ flightData.connected ? '●' : '○' }}
                </span>
                <span class="mode-display">{{ currentMode }}</span>
                <span :class="['arm-status', flightData.armed ? 'armed' : 'disarmed']">
                    {{ flightData.armed ? 'ARMED' : 'SAFE' }}
                </span>
            </div>
            <div class="status-right">
                <span class="battery">🔋{{ flightData.battery.toFixed(0) }}%</span>
                <span class="satellites">📡{{ flightData.satellites }}</span>
            </div>
        </div>

        <!-- 主要内容区域 -->
        <div class="main-content">
            <!-- 飞行数据面板 -->
            <div v-if="activeTab === 'flight'" class="flight-panel">
                <!-- 人工地平仪 -->
                <div class="horizon-display">
                    <div class="horizon-container">
                        <div 
                            class="horizon-sky" 
                            :style="{
                                transform: `rotate(${-flightData.roll}deg) translateY(${flightData.pitch * 1.5}px)`
                            }"
                        ></div>
                        <div 
                            class="horizon-ground" 
                            :style="{
                                transform: `rotate(${-flightData.roll}deg) translateY(${flightData.pitch * 1.5}px)`
                            }"
                        ></div>
                        <div class="aircraft-indicator">
                            <div class="aircraft-symbol"></div>
                        </div>
                    </div>
                    
                    <!-- 飞行数据覆盖 -->
                    <div class="flight-data-overlay">
                        <div class="data-item">
                            <span class="label">ALT</span>
                            <span class="value">{{ flightData.altitude.toFixed(0) }}m</span>
                        </div>
                        <div class="data-item">
                            <span class="label">SPD</span>
                            <span class="value">{{ flightData.speed.toFixed(1) }}m/s</span>
                        </div>
                        <div class="data-item">
                            <span class="label">HDG</span>
                            <span class="value">{{ flightData.heading.toFixed(0) }}°</span>
                        </div>
                    </div>
                </div>

                <!-- 飞行模式选择 -->
                <div class="mode-selector">
                    <button 
                        v-for="mode in flightModes" 
                        :key="mode"
                        :class="['mode-btn', { active: currentMode === mode }]"
                        @click="setFlightMode(mode)"
                    >
                        {{ mode }}
                    </button>
                </div>

                <!-- 解锁按钮 -->
                <button 
                    :class="['arm-button', flightData.armed ? 'disarm' : 'arm']"
                    @click="toggleArmed"
                >
                    {{ flightData.armed ? '锁定' : '解锁' }}
                </button>
            </div>

            <!-- 配置面板 -->
            <div v-if="activeTab === 'config'" class="config-panel">
                <div class="config-section">
                    <h4>舵面设置</h4>
                    <div class="config-grid">
                        <div class="config-item">
                            <label>副翼行程</label>
                            <input type="range" min="0" max="100" value="75">
                            <span>75%</span>
                        </div>
                        <div class="config-item">
                            <label>升降舵行程</label>
                            <input type="range" min="0" max="100" value="80">
                            <span>80%</span>
                        </div>
                        <div class="config-item">
                            <label>方向舵行程</label>
                            <input type="range" min="0" max="100" value="70">
                            <span>70%</span>
                        </div>
                        <div class="config-item">
                            <label>襟翼角度</label>
                            <input type="range" min="-30" max="60" value="0">
                            <span>0°</span>
                        </div>
                    </div>
                </div>

                <div class="config-section">
                    <h4>飞行参数</h4>
                    <div class="config-grid">
                        <div class="config-item">
                            <label>失速速度</label>
                            <input type="number" value="12" class="number-input">
                            <span>m/s</span>
                        </div>
                        <div class="config-item">
                            <label>巡航速度</label>
                            <input type="number" value="25" class="number-input">
                            <span>m/s</span>
                        </div>
                    </div>
                </div>
            </div>

            <!-- 地图面板 -->
            <div v-if="activeTab === 'map'" class="map-panel">
                <div class="mini-map">
                    <div class="map-grid">
                        <div v-for="i in 10" :key="`h-${i}`" class="grid-line horizontal" :style="{ top: `${i * 10}%` }"></div>
                        <div v-for="i in 10" :key="`v-${i}`" class="grid-line vertical" :style="{ left: `${i * 10}%` }"></div>
                    </div>
                    <div class="aircraft-pos" :style="{ transform: `translate(-50%, -50%) rotate(${flightData.heading}deg)` }">✈</div>
                </div>
                <div class="gps-info">
                    <div>纬度: {{ flightData.latitude.toFixed(6) }}°</div>
                    <div>经度: {{ flightData.longitude.toFixed(6) }}°</div>
                </div>
            </div>
        </div>

        <!-- 虚拟摇杆控制区域 -->
        <div class="joystick-area">
            <!-- 左摇杆 (油门/偏航) -->
            <div class="joystick-container left">
                <div 
                    class="joystick"
                    @touchstart="event => event.preventDefault()"
                    @touchmove="event => handleJoystickMove(leftJoystick, event)"
                    @touchend="() => resetJoystick(leftJoystick)"
                    @mousedown="event => event.preventDefault()"
                    @mousemove="event => event.buttons && handleJoystickMove(leftJoystick, event)"
                    @mouseup="() => resetJoystick(leftJoystick)"
                    @mouseleave="() => resetJoystick(leftJoystick)"
                >
                    <div class="joystick-knob" :style="getJoystickKnobStyle(leftJoystick)"></div>
                </div>
                <div class="joystick-labels">
                    <span class="label-top">油门</span>
                    <span class="label-left">左偏航</span>
                    <span class="label-right">右偏航</span>
                </div>
                <div class="control-values">
                    <div>THR: {{ controls.throttle.toFixed(0) }}%</div>
                    <div>YAW: {{ controls.rudder.toFixed(0) }}</div>
                </div>
            </div>

            <!-- 右摇杆 (副翼/升降舵) -->
            <div class="joystick-container right">
                <div 
                    class="joystick"
                    @touchstart="event => event.preventDefault()"
                    @touchmove="event => handleJoystickMove(rightJoystick, event)"
                    @touchend="() => resetJoystick(rightJoystick)"
                    @mousedown="event => event.preventDefault()"
                    @mousemove="event => event.buttons && handleJoystickMove(rightJoystick, event)"
                    @mouseup="() => resetJoystick(rightJoystick)"
                    @mouseleave="() => resetJoystick(rightJoystick)"
                >
                    <div class="joystick-knob" :style="getJoystickKnobStyle(rightJoystick)"></div>
                </div>
                <div class="joystick-labels">
                    <span class="label-top">拉升</span>
                    <span class="label-bottom">俯冲</span>
                    <span class="label-left">左滚</span>
                    <span class="label-right">右滚</span>
                </div>
                <div class="control-values">
                    <div>AIL: {{ controls.aileron.toFixed(0) }}</div>
                    <div>ELE: {{ controls.elevator.toFixed(0) }}</div>
                </div>
            </div>
        </div>

        <!-- 底部Tab导航 -->
        <div class="tab-navigation">
            <button 
                v-for="tab in tabs" 
                :key="tab.id"
                :class="['tab-btn', { active: activeTab === tab.id }]"
                @click="activeTab = tab.id"
            >
                <span class="tab-icon">{{ tab.icon }}</span>
                <span class="tab-label">{{ tab.name }}</span>
            </button>
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

// 状态栏
.status-bar {
    height: 40px;
    background: rgba(0, 0, 0, 0.8);
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 0 15px;
    font-size: 12px;
    border-bottom: 1px solid #333;
    
    .status-left {
        display: flex;
        gap: 15px;
        align-items: center;
        
        .connection-status {
            &.connected { color: #4CAF50; }
            &.disconnected { color: #f44336; }
        }
        
        .mode-display {
            color: #2196F3;
            font-weight: bold;
        }
        
        .arm-status {
            &.armed { color: #ff9800; }
            &.disarmed { color: #4CAF50; }
        }
    }
    
    .status-right {
        display: flex;
        gap: 10px;
        font-size: 11px;
    }
}

// 主内容区域
.main-content {
    flex: 1;
    padding: 10px;
    overflow: hidden;
    padding-bottom: 10px;
}

// 飞行面板
.flight-panel {
    height: 100%;
    display: flex;
    flex-direction: column;
    gap: 8px;
    overflow-y: auto;
}

.horizon-display {
    position: relative;
    height: 180px;
    margin-bottom: 8px;
    display: flex;
    flex-direction: column;
    align-items: center;
}

.horizon-container {
    position: relative;
    width: 200px;
    height: 200px;
    border-radius: 50%;
    overflow: hidden;
    border: 2px solid #555;
    flex-shrink: 0;
    
    .horizon-sky {
        position: absolute;
        width: 300px;
        height: 150px;
        left: -50px;
        top: -75px;
        background: linear-gradient(to bottom, #87CEEB 0%, #4169E1 100%);
        transform-origin: center 150px;
    }
    
    .horizon-ground {
        position: absolute;
        width: 300px;
        height: 150px;
        left: -50px;
        top: 125px;
        background: linear-gradient(to bottom, #8B4513 0%, #654321 100%);
        transform-origin: center 0px;
    }
    
    .aircraft-indicator {
        position: absolute;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
        z-index: 10;
        
        .aircraft-symbol {
            width: 30px;
            height: 3px;
            background: #ffff00;
            position: relative;
            
            &::before {
                content: '';
                position: absolute;
                left: 50%;
                top: -10px;
                transform: translateX(-50%);
                width: 3px;
                height: 15px;
                background: #ffff00;
            }
        }
    }
}

.flight-data-overlay {
    position: absolute;
    top: 10px;
    right: 10px;
    display: flex;
    flex-direction: column;
    gap: 5px;
    z-index: 15;
    
    .data-item {
        background: rgba(0, 0, 0, 0.7);
        padding: 5px 8px;
        border-radius: 4px;
        font-size: 11px;
        display: flex;
        justify-content: space-between;
        min-width: 60px;
        
        .label {
            color: #ccc;
        }
        
        .value {
            color: #fff;
            font-weight: bold;
        }
    }
}

.mode-selector {
    display: flex;
    gap: 4px;
    margin-bottom: 8px;
    flex-shrink: 0;
    
    .mode-btn {
        flex: 1;
        padding: 6px 4px;
        border: none;
        border-radius: 4px;
        background: rgba(68, 68, 68, 0.8);
        color: #fff;
        font-size: 10px;
        cursor: pointer;
        transition: all 0.3s;
        backdrop-filter: blur(5px);
        
        &:hover, &.active {
            background: rgba(33, 150, 243, 0.8);
        }
    }
}

.arm-button {
    padding: 10px;
    border: none;
    border-radius: 6px;
    font-size: 13px;
    font-weight: bold;
    cursor: pointer;
    transition: all 0.3s;
    margin-bottom: 8px;
    backdrop-filter: blur(5px);
    flex-shrink: 0;
    
    &.arm {
        background: rgba(76, 175, 80, 0.8);
        color: white;
    }
    
    &.disarm {
        background: rgba(244, 67, 54, 0.8);
        color: white;
    }
}

// 配置面板
.config-panel {
    height: 100%;
    overflow-y: auto;
    padding-bottom: 20px;
    
    .config-section {
        margin-bottom: 15px;
        
        h4 {
            margin: 0 0 8px 0;
            color: #4CAF50;
            font-size: 13px;
            border-bottom: 1px solid #444;
            padding-bottom: 4px;
        }
    }
    
    .config-grid {
        display: grid;
        grid-template-columns: 1fr;
        gap: 8px;
    }
    
    .config-item {
        display: flex;
        align-items: center;
        gap: 8px;
        background: rgba(51, 51, 51, 0.8);
        padding: 6px;
        border-radius: 4px;
        backdrop-filter: blur(5px);
        
        label {
            flex: 0 0 70px;
            font-size: 10px;
            color: #ccc;
        }
        
        input[type="range"] {
            flex: 1;
        }
        
        .number-input {
            flex: 1;
            padding: 3px 6px;
            background: rgba(68, 68, 68, 0.8);
            border: 1px solid #666;
            border-radius: 3px;
            color: white;
            font-size: 10px;
        }
        
        span {
            flex: 0 0 35px;
            font-size: 10px;
            color: #fff;
            text-align: right;
        }
    }
}

// 地图面板
.map-panel {
    height: 100%;
    display: flex;
    flex-direction: column;
    gap: 8px;
    padding-bottom: 20px;
}

.mini-map {
    position: relative;
    height: 160px;
    background: rgba(26, 26, 26, 0.8);
    border-radius: 8px;
    overflow: hidden;
    border: 1px solid #444;
    backdrop-filter: blur(5px);
    
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
    
    .aircraft-pos {
        position: absolute;
        top: 50%;
        left: 50%;
        font-size: 16px;
        color: #ff4444;
        z-index: 10;
        text-shadow: 0 0 10px #ff4444;
    }
}

.gps-info {
    background: rgba(51, 51, 51, 0.8);
    padding: 8px;
    border-radius: 4px;
    font-size: 10px;
    color: #ccc;
    backdrop-filter: blur(5px);
    
    div {
        margin-bottom: 4px;
    }
}

// 虚拟摇杆区域
.joystick-area {
    position: fixed;
    bottom: 70px;
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

.joystick-container {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 3px;
    position: relative;
    z-index: 10;
    background-color: transparent;
    pointer-events: all;
    
    .joystick {
        pointer-events: all;
        position: relative;
        width: 90px;
        height: 90px;
        border-radius: 50%;
        background: rgba(255, 255, 255, 0.05);
        border: 1px solid rgba(255, 255, 255, 0.15);
        cursor: pointer;
        backdrop-filter: blur(10px);
        box-shadow: 0 4px 20px rgba(0, 0, 0, 0.3);
        
        .joystick-knob {
            position: absolute;
            top: 50%;
            left: 50%;
            width: 22px;
            height: 22px;
            border-radius: 50%;
            background: rgba(33, 150, 243, 0.9);
            transform: translate(-50%, -50%);
            transition: all 0.1s ease;
            border: 2px solid rgba(255, 255, 255, 0.9);
            box-shadow: 0 0 15px rgba(33, 150, 243, 0.6);
        }
    }
    
    .joystick-labels {
        position: relative;
        width: 90px;
        height: 25px;
        font-size: 7px;
        color: rgba(255, 255, 255, 0.6);
        text-shadow: 0 1px 2px rgba(0, 0, 0, 0.8);
        
        .label-top {
            position: absolute;
            top: 0;
            left: 50%;
            transform: translateX(-50%);
        }
        
        .label-bottom {
            position: absolute;
            bottom: 0;
            left: 50%;
            transform: translateX(-50%);
        }
        
        .label-left {
            position: absolute;
            left: 0;
            top: 50%;
            transform: translateY(-50%);
        }
        
        .label-right {
            position: absolute;
            right: 0;
            top: 50%;
            transform: translateY(-50%);
        }
    }
    
    .control-values {
        display: flex;
        gap: 6px;
        font-size: 8px;
        color: rgba(76, 175, 80, 1);
        font-family: 'Courier New', monospace;
        background: rgba(0, 0, 0, 0.8);
        padding: 2px 6px;
        border-radius: 6px;
        backdrop-filter: blur(10px);
        box-shadow: 0 2px 10px rgba(0, 0, 0, 0.5);
        border: 1px solid rgba(76, 175, 80, 0.3);
    }
}

// Tab导航
.tab-navigation {
    height: 60px;
    background: rgba(0, 0, 0, 0.9);
    display: flex;
    border-top: 1px solid #333;
    
    .tab-btn {
        flex: 1;
        border: none;
        background: transparent;
        color: #666;
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        gap: 2px;
        cursor: pointer;
        transition: all 0.3s;
        
        &.active {
            color: #2196F3;
        }
        
        .tab-icon {
            font-size: 18px;
        }
        
        .tab-label {
            font-size: 10px;
        }
    }
}

// 滑块样式
input[type="range"] {
    -webkit-appearance: none;
    appearance: none;
    background: transparent;
    cursor: pointer;
    
    &::-webkit-slider-track {
        background: #555;
        height: 4px;
        border-radius: 2px;
    }
    
    &::-webkit-slider-thumb {
        -webkit-appearance: none;
        appearance: none;
        background: #2196F3;
        height: 16px;
        width: 16px;
        border-radius: 50%;
        cursor: pointer;
    }
    
    &::-moz-range-track {
        background: #555;
        height: 4px;
        border-radius: 2px;
        border: none;
    }
    
    &::-moz-range-thumb {
        background: #2196F3;
        height: 16px;
        width: 16px;
        border-radius: 50%;
        cursor: pointer;
        border: none;
    }
}
</style>
