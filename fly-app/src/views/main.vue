<script setup>
import { ref, reactive, onMounted, onUnmounted } from 'vue'
import StatusBar from '@/components/StatusBar.vue'
import Aircraft3D from '@/components/Aircraft3D.vue'
import VirtualJoystick from '@/components/VirtualJoystick.vue'
import ConfigPanel from '@/components/ConfigPanel.vue'
import MapPanel from '@/components/MapPanel.vue'
import ThrottleControl from '@/components/ThrottleControl.vue'
import { request } from '@/common/request'

const props = defineProps({
    deviceInfo: {
        type: Object,
        required: true,
    },
})

// 飞行数据状态
const flightData = reactive({
    // 基础飞行参数（不依赖传感器）
    altitude: 0, // 高度 (米) - 可通过气压计获得
    speed: 0, // 速度 (米/秒) - 可通过GPS计算
    heading: 0, // 航向角 - 可通过GPS计算

    // GPS数据
    latitude: 0,
    longitude: 0,
    satellites: 0,

    // 系统状态
    battery: 100, // 电池电量 (%)
    signal: 100, // 信号强度 (%)
    connected: false, // 连接状态
})

// 控制输入
const controls = reactive({
    throttle: 0, // 油门 (0 to 100)
    aileron: 0, // 副翼 (-100 to 100)
    elevator: 0, // 升降舵 (-100 to 100)
    rudder: 0, // 方向舵 (-100 to 100)
})

// Tab 切换
const activeTab = ref('flight')
const tabs = [
    { id: 'flight', name: '飞行', icon: '✈️' },
    { id: 'config', name: '配置', icon: '⚙️' },
    { id: 'map', name: '地图', icon: '🗺️' },
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
            // 模拟基础飞行数据变化（不涉及姿态传感器）
            flightData.altitude += (Math.random() - 0.5) * 5
            flightData.speed = Math.max(0, flightData.speed + (Math.random() - 0.5) * 2)
            flightData.heading = (flightData.heading + (Math.random() - 0.5) * 2 + 360) % 360

            flightData.satellites = Math.floor(Math.random() * 12) + 4
            flightData.battery = Math.max(0, flightData.battery - 0.01)

            // 保证合理的数据范围
            flightData.altitude = Math.max(0, flightData.altitude)
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

const handleTabChange = (tabId) => {
    activeTab.value = tabId
}

// 测试翻滚舵机的函数
const testRollServo = async (value) => {
    console.log('Testing roll servo with value:', value)
    try {
        const response = await request({
            url: 'http://' + props.deviceInfo.ip + '/roll',
            method: 'get',
            params: { value },
        })
        console.log('Roll test response:', response)
    } catch (error) {
        console.error('Roll test failed:', error)
    }
}

// 油门控制事件
const handleThrottleChange = (value) => {
    controls.throttle = value
    // 发送油门数据到设备
    // 直接调用接口
    request({
        url: 'http://' + props.deviceInfo.ip + '/throttle',
        method: 'get',
        params: { value },
    })
}

// 摇杆控制事件
const handleLeftJoystick = ({ x, y }) => {
    // 左摇杆：只有x轴控制方向舵(rudder/yaw)，水平转向
    controls.rudder = Math.max(-100, Math.min(100, x))
    request({
        url: 'http://' + props.deviceInfo.ip + '/yaw',
        method: 'get',
        params: { value: controls.rudder },
    })
}

const handleRightJoystick = ({ x, y }) => {
    // 右摇杆：x轴控制副翼(aileron/roll)，y轴控制升降舵(elevator/pitch)
    controls.aileron = Math.max(-100, Math.min(100, x))
    controls.elevator = Math.max(-100, Math.min(100, -y)) // 反转y轴，因为飞控中上推是负值

    // 调试输出
    console.log('Roll control:', controls.aileron, 'Pitch control:', controls.elevator)

    // 发送副翼(横滚)控制数据到设备
    request({
        url: 'http://' + props.deviceInfo.ip + '/roll',
        method: 'get',
        params: { value: controls.aileron },
    }).then((response) => {
        console.log('Roll response:', response)
    }).catch((error) => {
        console.error('Roll request failed:', error)
    })

    // 发送升降舵(俯仰)控制数据到设备
    request({
        url: 'http://' + props.deviceInfo.ip + '/pitch',
        method: 'get',
        params: { value: controls.elevator },
    }).then((response) => {
        console.log('Pitch response:', response)
    }).catch((error) => {
        console.error('Pitch request failed:', error)
    })
}
</script>

<template>
    <main class="mobile-container">
        <!-- 顶部状态栏(包含Tab选择器) -->
        <StatusBar :flight-data="flightData" :current-mode="currentMode" :flight-modes="flightModes"
            :active-tab="activeTab" :tabs="tabs" @tab-change="handleTabChange" @set-flight-mode="setFlightMode" />

        <!-- 主要内容区域 -->
        <div class="main-content">
            <!-- 飞行数据面板 -->
            <div v-if="activeTab === 'flight'" class="flight-panel">
                <!-- 控制器状态显示 -->
                <Aircraft3D :flight-data="flightData" :controls="controls" />

                <!-- 翻滚舵机测试按钮 -->
                <div class="servo-test-panel">
                    <h4>🔧 翻滚舵机测试</h4>
                    <div class="test-buttons">
                        <button @click="testRollServo(-100)" class="test-btn">左滚最大</button>
                        <button @click="testRollServo(-50)" class="test-btn">左滚50%</button>
                        <button @click="testRollServo(0)" class="test-btn">中立位置</button>
                        <button @click="testRollServo(50)" class="test-btn">右滚50%</button>
                        <button @click="testRollServo(100)" class="test-btn">右滚最大</button>
                    </div>
                    <div class="test-info">
                        <p>当前翻滚值: {{ controls.aileron.toFixed(0) }}</p>
                        <p>检查ESP32串口监视器查看"设置翻滚"信息</p>
                    </div>
                </div>
            </div>

            <!-- 配置面板 -->
            <ConfigPanel v-if="activeTab === 'config'" />

            <!-- 地图面板 -->
            <MapPanel v-if="activeTab === 'map'" :flight-data="flightData" />
        </div>

        <!-- 油门控制 -->
        <ThrottleControl :value="controls.throttle" @throttle-change="handleThrottleChange" />

        <!-- 虚拟摇杆控制区域 -->
        <div class="joystick-area">
            <!-- 左摇杆 (只控制偏航) -->
            <VirtualJoystick position="left" :labels="{
                top: '',
                bottom: '',
                left: '左偏航',
                right: '右偏航',
            }" :display-values="{
                YAW: controls.rudder.toFixed(0),
            }" @joystick-move="handleLeftJoystick" />

            <!-- 右摇杆 (翻滚和俯仰) -->
            <VirtualJoystick position="right" :labels="{
                top: '拉升',
                bottom: '俯冲',
                left: '左滚',
                right: '右滚',
            }" :display-values="{
                ROLL: controls.aileron.toFixed(0),
                PITCH: controls.elevator.toFixed(0),
            }" @joystick-move="handleRightJoystick" />
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
    padding-bottom: 10px; // 减少底部间距，为油门控制留空间
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
    bottom: 60px; // 向上移动，为油门控制留空间
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

.servo-test-panel {
    background: rgba(255, 255, 255, 0.1);
    border-radius: 10px;
    padding: 15px;
    margin-top: 10px;
    border: 1px solid rgba(255, 255, 255, 0.2);

    h4 {
        margin: 0 0 10px 0;
        color: #fff;
        font-size: 16px;
    }

    .test-buttons {
        display: flex;
        gap: 8px;
        flex-wrap: wrap;
        margin-bottom: 10px;
    }

    .test-btn {
        padding: 8px 12px;
        background: linear-gradient(135deg, #007bff, #0056b3);
        color: white;
        border: none;
        border-radius: 6px;
        cursor: pointer;
        font-size: 12px;
        transition: all 0.2s;

        &:hover {
            background: linear-gradient(135deg, #0056b3, #003d82);
            transform: translateY(-1px);
        }

        &:active {
            transform: translateY(0);
        }
    }

    .test-info {
        p {
            margin: 4px 0;
            font-size: 12px;
            color: rgba(255, 255, 255, 0.8);
        }
    }
}
</style>
