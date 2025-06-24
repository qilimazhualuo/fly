<script setup>
import { ref } from 'vue'

const props = defineProps({
    controls: {
        type: Object,
        required: true,
    },
})

const emit = defineEmits(['controlChange'])

// 滑块值
const rollValue = ref(0)
const pitchValue = ref(0)
const yawValue = ref(0)

// 滑块变化处理（移除防抖，在父组件处理）
const handleRollChange = (value) => {
    rollValue.value = value
    emit('controlChange', { type: 'roll', value })
}

const handlePitchChange = (value) => {
    pitchValue.value = value
    emit('controlChange', { type: 'pitch', value })
}

const handleYawChange = (value) => {
    yawValue.value = value
    emit('controlChange', { type: 'yaw', value })
}

// 一键归中
const resetToCenter = () => {
    rollValue.value = 0
    pitchValue.value = 0
    yawValue.value = 0

    emit('controlChange', { type: 'roll', value: 0 })
    emit('controlChange', { type: 'pitch', value: 0 })
    emit('controlChange', { type: 'yaw', value: 0 })
}
</script>

<template>
    <div class="slider-control">
        <div class="slider-header">
            <h4>🎚️ 滑块控制</h4>
            <button @click="resetToCenter" class="reset-btn">归中</button>
        </div>

        <div class="sliders-container">
            <!-- 翻滚控制 -->
            <div class="slider-item">
                <div class="slider-label">
                    <span class="label">翻滚 (Roll)</span>
                    <span class="value">{{ rollValue.toFixed(0) }}</span>
                </div>
                <div class="slider-wrapper">
                    <span class="range-label">左</span>
                    <input type="range" min="-100" max="100" step="1" v-model="rollValue"
                        @input="handleRollChange($event.target.value)" class="slider roll-slider" />
                    <span class="range-label">右</span>
                </div>
            </div>

            <!-- 俯仰控制 -->
            <div class="slider-item">
                <div class="slider-label">
                    <span class="label">俯仰 (Pitch)</span>
                    <span class="value">{{ pitchValue.toFixed(0) }}</span>
                </div>
                <div class="slider-wrapper">
                    <span class="range-label">俯</span>
                    <input type="range" min="-100" max="100" step="1" v-model="pitchValue"
                        @input="handlePitchChange($event.target.value)" class="slider pitch-slider" />
                    <span class="range-label">仰</span>
                </div>
            </div>

            <!-- 偏航控制 -->
            <div class="slider-item">
                <div class="slider-label">
                    <span class="label">偏航 (Yaw)</span>
                    <span class="value">{{ yawValue.toFixed(0) }}</span>
                </div>
                <div class="slider-wrapper">
                    <span class="range-label">左</span>
                    <input type="range" min="-100" max="100" step="1" v-model="yawValue"
                        @input="handleYawChange($event.target.value)" class="slider yaw-slider" />
                    <span class="range-label">右</span>
                </div>
            </div>
        </div>
    </div>
</template>

<style lang="less" scoped>
.slider-control {
    background: rgba(255, 255, 255, 0.1);
    border-radius: 10px;
    padding: 15px;
    margin-top: 10px;
    border: 1px solid rgba(255, 255, 255, 0.2);
}

.slider-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 15px;

    h4 {
        margin: 0;
        color: #fff;
        font-size: 16px;
    }

    .reset-btn {
        padding: 6px 12px;
        background: linear-gradient(135deg, #ff9800, #f57c00);
        color: white;
        border: none;
        border-radius: 6px;
        cursor: pointer;
        font-size: 12px;
        transition: all 0.2s;

        &:hover {
            background: linear-gradient(135deg, #ffb74d, #ff9800);
            transform: translateY(-1px);
        }

        &:active {
            transform: translateY(0);
        }
    }
}

.sliders-container {
    display: flex;
    flex-direction: column;
    gap: 15px;
}

.slider-item {
    .slider-label {
        display: flex;
        justify-content: space-between;
        align-items: center;
        margin-bottom: 8px;

        .label {
            color: #fff;
            font-size: 14px;
            font-weight: bold;
        }

        .value {
            color: #4caf50;
            font-family: 'Courier New', monospace;
            font-size: 14px;
            font-weight: bold;
            min-width: 40px;
            text-align: right;
        }
    }

    .slider-wrapper {
        display: flex;
        align-items: center;
        gap: 10px;

        .range-label {
            color: rgba(255, 255, 255, 0.7);
            font-size: 12px;
            min-width: 20px;
            text-align: center;
        }

        .slider {
            flex: 1;
            -webkit-appearance: none;
            appearance: none;
            height: 8px;
            border-radius: 4px;
            outline: none;
            cursor: pointer;
            transition: all 0.3s ease;

            &::-webkit-slider-track {
                height: 8px;
                border-radius: 4px;
                background: linear-gradient(to right,
                        rgba(255, 255, 255, 0.1),
                        rgba(255, 255, 255, 0.2),
                        rgba(255, 255, 255, 0.1));
                border: 1px solid rgba(255, 255, 255, 0.1);
            }

            &::-webkit-slider-thumb {
                -webkit-appearance: none;
                appearance: none;
                width: 20px;
                height: 20px;
                border-radius: 50%;
                border: 2px solid #fff;
                cursor: pointer;
                transition: all 0.3s ease;
                box-shadow: 0 2px 8px rgba(0, 0, 0, 0.3);
            }

            &::-moz-range-track {
                height: 8px;
                border-radius: 4px;
                background: linear-gradient(to right,
                        rgba(255, 255, 255, 0.1),
                        rgba(255, 255, 255, 0.2),
                        rgba(255, 255, 255, 0.1));
                border: 1px solid rgba(255, 255, 255, 0.1);
            }

            &::-moz-range-thumb {
                width: 20px;
                height: 20px;
                border-radius: 50%;
                border: 2px solid #fff;
                cursor: pointer;
                background: #2196f3;
                box-shadow: 0 2px 8px rgba(0, 0, 0, 0.3);
            }

            &.roll-slider {
                &::-webkit-slider-thumb {
                    background: linear-gradient(135deg, #2196f3, #1976d2);
                }

                &:hover::-webkit-slider-thumb {
                    background: linear-gradient(135deg, #42a5f5, #2196f3);
                    transform: scale(1.1);
                    box-shadow: 0 0 15px rgba(33, 150, 243, 0.6);
                }
            }

            &.pitch-slider {
                &::-webkit-slider-thumb {
                    background: linear-gradient(135deg, #9c27b0, #7b1fa2);
                }

                &:hover::-webkit-slider-thumb {
                    background: linear-gradient(135deg, #ba68c8, #9c27b0);
                    transform: scale(1.1);
                    box-shadow: 0 0 15px rgba(156, 39, 176, 0.6);
                }
            }

            &.yaw-slider {
                &::-webkit-slider-thumb {
                    background: linear-gradient(135deg, #ff9800, #f57c00);
                }

                &:hover::-webkit-slider-thumb {
                    background: linear-gradient(135deg, #ffb74d, #ff9800);
                    transform: scale(1.1);
                    box-shadow: 0 0 15px rgba(255, 152, 0, 0.6);
                }
            }
        }
    }
}
</style>
