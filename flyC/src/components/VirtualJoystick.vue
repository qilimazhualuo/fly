<script setup>
import { ref, onMounted, onUnmounted } from 'vue'
import nipplejs from 'nipplejs'

const props = defineProps({
    position: {
        type: String,
        default: 'left' // 'left' or 'right'
    },
    labels: {
        type: Object,
        default: () => ({})
    },
    displayValues: {
        type: Object,
        default: () => ({})
    }
})

const emit = defineEmits(['joystickMove'])

const joystickRef = ref(null)
let manager = null

onMounted(() => {
    if (joystickRef.value) {
        // 根据摇杆位置确定配置
        const isLeftJoystick = props.position === 'left'

        // 创建nipplejs管理器
        manager = nipplejs.create({
            zone: joystickRef.value,
            mode: 'static',
            position: { left: '50%', top: '50%' },
            color: 'rgba(33, 150, 243, 0.9)',
            size: 90,
            threshold: 0.1,
            fadeTime: 250,
            multitouch: false,
            maxNumberOfNipples: 1,
            dataOnly: false,
            restJoystick: true,
            restOpacity: 0.5,
            lockX: isLeftJoystick,
            lockY: false, // 左摇杆锁定Y轴，只允许X轴(左右)移动
            catchDistance: 200,
            dynamicPage: false
        })

        // 监听摇杆移动事件
        manager.on('move', (evt, data) => {
            if (data.vector) {
                // nipplejs返回的坐标系：x轴向右为正，y轴向上为正
                // 转换为我们需要的格式：x: -100到100, y: -100到100
                const x = (data.vector.x * 100)
                const y = isLeftJoystick ? 0 : (data.vector.y * 100) // 左摇杆不输出Y值

                emit('joystickMove', { x, y })
            }
        })

        // 监听摇杆结束事件
        manager.on('end', () => {
            // 所有摇杆都回中
            emit('joystickMove', { x: 0, y: 0 })
        })

        // 监听摇杆开始事件
        manager.on('start', () => {
            // 可以在这里添加震动反馈等
        })
    }
})

onUnmounted(() => {
    if (manager) {
        manager.destroy()
        manager = null
    }
})
</script>

<template>
    <div class="joystick-container" :class="position">
        <div ref="joystickRef" class="joystick-zone"></div>
        <div class="joystick-labels">
            <span v-if="labels.top" class="label-top">{{ labels.top }}</span>
            <span v-if="labels.bottom" class="label-bottom">{{ labels.bottom }}</span>
            <span v-if="labels.left" class="label-left">{{ labels.left }}</span>
            <span v-if="labels.right" class="label-right">{{ labels.right }}</span>
        </div>
        <div class="control-values">
            <div v-for="(value, key) in displayValues" :key="key">
                {{ key }}: {{ value }}
            </div>
        </div>
    </div>
</template>

<style lang="less" scoped>
.joystick-container {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: 3px;
    position: relative;
    z-index: 10;
    background-color: transparent;
    pointer-events: all;

    .joystick-zone {
        width: 90px;
        height: 90px;
        border-radius: 50%;
        background: rgba(255, 255, 255, 0.05);
        border: 1px solid rgba(255, 255, 255, 0.15);
        backdrop-filter: blur(10px);
        box-shadow: 0 4px 20px rgba(0, 0, 0, 0.3);
        position: relative;

        // nipplejs会在这个区域内创建摇杆
        // :deep(.nipple) {
        //   background: rgba(33, 150, 243, 0.9) !important;
        //   border: 2px solid rgba(255, 255, 255, 0.9) !important;
        //   box-shadow: 0 0 15px rgba(33, 150, 243, 0.6) !important;
        // }

        // :deep(.front) {
        //   background: rgba(33, 150, 243, 0.9) !important;
        //   border: 2px solid rgba(255, 255, 255, 0.9) !important;
        // }

        // :deep(.back) {
        //   background: rgba(255, 255, 255, 0.1) !important;
        //   border: 1px solid rgba(255, 255, 255, 0.2) !important;
        // }
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
</style>