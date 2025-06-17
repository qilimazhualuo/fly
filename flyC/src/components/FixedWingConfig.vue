<script setup>
import { reactive } from 'vue'

const config = reactive({
    aileron: {
        travel: 75
    },
    elevator: {
        travel: 80
    },
    rudder: {
        travel: 70
    },
    flaps: {
        angle: 0
    },
    flight: {
        stallSpeed: 12,
        cruiseSpeed: 25,
        maxSpeed: 45,
        cruiseAltitude: 100
    },
    pid: {
        pitch: {
            p: 0.5,
            i: 0.01,
            d: 0.05
        },
        roll: {
            p: 0.6,
            i: 0.02,
            d: 0.08
        },
        yaw: {
            p: 0.3,
            i: 0.005,
            d: 0.02
        }
    }
})

const saveConfig = () => {
    localStorage.setItem('fixedWingConfig', JSON.stringify(config))
    console.log('配置已保存')
}

const loadConfig = () => {
    const saved = localStorage.getItem('fixedWingConfig')
    if (saved) {
        Object.assign(config, JSON.parse(saved))
        console.log('配置已加载')
    }
}

const resetConfig = () => {
    config.aileron.travel = 75
    config.elevator.travel = 80
    config.rudder.travel = 70
    config.flaps.angle = 0
    config.flight.stallSpeed = 12
    config.flight.cruiseSpeed = 25
    config.flight.maxSpeed = 45
    config.flight.cruiseAltitude = 100
    config.pid.pitch = { p: 0.5, i: 0.01, d: 0.05 }
    config.pid.roll = { p: 0.6, i: 0.02, d: 0.08 }
    config.pid.yaw = { p: 0.3, i: 0.005, d: 0.02 }
    console.log('配置已重置')
}
</script>

<template>
    <div class="config-panel">
        <h3>固定翼配置</h3>

        <!-- 舵面配置 -->
        <div class="config-section">
            <h4>舵面配置</h4>
            <div class="control-surfaces">
                <div class="surface-item">
                    <label>副翼行程</label>
                    <div class="range-input">
                        <input type="range" min="0" max="100" v-model="config.aileron.travel">
                        <span>{{ config.aileron.travel }}%</span>
                    </div>
                </div>

                <div class="surface-item">
                    <label>升降舵行程</label>
                    <div class="range-input">
                        <input type="range" min="0" max="100" v-model="config.elevator.travel">
                        <span>{{ config.elevator.travel }}%</span>
                    </div>
                </div>

                <div class="surface-item">
                    <label>方向舵行程</label>
                    <div class="range-input">
                        <input type="range" min="0" max="100" v-model="config.rudder.travel">
                        <span>{{ config.rudder.travel }}%</span>
                    </div>
                </div>

                <div class="surface-item">
                    <label>襟翼角度</label>
                    <div class="range-input">
                        <input type="range" min="-30" max="60" v-model="config.flaps.angle">
                        <span>{{ config.flaps.angle }}°</span>
                    </div>
                </div>
            </div>
        </div>

        <!-- 飞行参数 -->
        <div class="config-section">
            <h4>飞行参数</h4>
            <div class="flight-params">
                <div class="param-item">
                    <label>失速速度</label>
                    <input type="number" v-model="config.flight.stallSpeed" class="param-input">
                    <span>m/s</span>
                </div>

                <div class="param-item">
                    <label>巡航速度</label>
                    <input type="number" v-model="config.flight.cruiseSpeed" class="param-input">
                    <span>m/s</span>
                </div>

                <div class="param-item">
                    <label>最大速度</label>
                    <input type="number" v-model="config.flight.maxSpeed" class="param-input">
                    <span>m/s</span>
                </div>

                <div class="param-item">
                    <label>巡航高度</label>
                    <input type="number" v-model="config.flight.cruiseAltitude" class="param-input">
                    <span>m</span>
                </div>
            </div>
        </div>

        <!-- PID控制器 -->
        <div class="config-section">
            <h4>PID控制器</h4>
            <div class="pid-controls">
                <div class="pid-group">
                    <h5>俯仰控制</h5>
                    <div class="pid-params">
                        <div class="pid-param">
                            <label>P</label>
                            <input type="number" v-model="config.pid.pitch.p" step="0.01">
                        </div>
                        <div class="pid-param">
                            <label>I</label>
                            <input type="number" v-model="config.pid.pitch.i" step="0.001">
                        </div>
                        <div class="pid-param">
                            <label>D</label>
                            <input type="number" v-model="config.pid.pitch.d" step="0.001">
                        </div>
                    </div>
                </div>

                <div class="pid-group">
                    <h5>横滚控制</h5>
                    <div class="pid-params">
                        <div class="pid-param">
                            <label>P</label>
                            <input type="number" v-model="config.pid.roll.p" step="0.01">
                        </div>
                        <div class="pid-param">
                            <label>I</label>
                            <input type="number" v-model="config.pid.roll.i" step="0.001">
                        </div>
                        <div class="pid-param">
                            <label>D</label>
                            <input type="number" v-model="config.pid.roll.d" step="0.001">
                        </div>
                    </div>
                </div>

                <div class="pid-group">
                    <h5>偏航控制</h5>
                    <div class="pid-params">
                        <div class="pid-param">
                            <label>P</label>
                            <input type="number" v-model="config.pid.yaw.p" step="0.01">
                        </div>
                        <div class="pid-param">
                            <label>I</label>
                            <input type="number" v-model="config.pid.yaw.i" step="0.001">
                        </div>
                        <div class="pid-param">
                            <label>D</label>
                            <input type="number" v-model="config.pid.yaw.d" step="0.001">
                        </div>
                    </div>
                </div>
            </div>
        </div>

        <!-- 配置按钮 -->
        <div class="config-actions">
            <button @click="saveConfig" class="action-btn save">
                保存配置
            </button>
            <button @click="loadConfig" class="action-btn load">
                加载配置
            </button>
            <button @click="resetConfig" class="action-btn reset">
                恢复默认
            </button>
        </div>
    </div>
</template>

<style lang="less" scoped>
.config-panel {
    background: #2a2a2a;
    border-radius: 10px;
    padding: 20px;
    height: 100%;
    overflow-y: auto;

    h3 {
        margin: 0 0 20px 0;
        color: #4CAF50;
        font-size: 18px;
    }
}

.config-section {
    margin-bottom: 25px;

    h4 {
        margin: 0 0 15px 0;
        color: #2196F3;
        font-size: 14px;
        border-bottom: 1px solid #444;
        padding-bottom: 5px;
    }

    h5 {
        margin: 0 0 10px 0;
        color: #ccc;
        font-size: 12px;
    }
}

.control-surfaces {
    display: grid;
    grid-template-columns: 1fr;
    gap: 15px;
}

.surface-item {
    display: flex;
    flex-direction: column;

    label {
        margin-bottom: 8px;
        color: #ccc;
        font-size: 12px;
    }

    .range-input {
        display: flex;
        align-items: center;
        gap: 10px;

        input {
            flex: 1;
        }

        span {
            font-size: 12px;
            color: #fff;
            min-width: 40px;
        }
    }
}

.flight-params {
    display: grid;
    grid-template-columns: repeat(2, 1fr);
    gap: 15px;
}

.param-item {
    display: flex;
    flex-direction: column;

    label {
        margin-bottom: 5px;
        color: #ccc;
        font-size: 12px;
    }

    .param-input {
        padding: 6px 8px;
        background: #333;
        border: 1px solid #555;
        border-radius: 4px;
        color: white;
        font-size: 12px;

        &:focus {
            outline: none;
            border-color: #2196F3;
        }
    }

    span {
        margin-top: 2px;
        font-size: 10px;
        color: #888;
    }
}

.pid-controls {
    display: flex;
    flex-direction: column;
    gap: 15px;
}

.pid-group {
    background: #333;
    padding: 15px;
    border-radius: 6px;
}

.pid-params {
    display: grid;
    grid-template-columns: repeat(3, 1fr);
    gap: 10px;
}

.pid-param {
    display: flex;
    flex-direction: column;

    label {
        margin-bottom: 5px;
        color: #ccc;
        font-size: 11px;
        text-align: center;
    }

    input {
        padding: 4px 6px;
        background: #444;
        border: 1px solid #666;
        border-radius: 3px;
        color: white;
        font-size: 11px;
        text-align: center;

        &:focus {
            outline: none;
            border-color: #2196F3;
        }
    }
}

.config-actions {
    display: flex;
    gap: 10px;
    margin-top: 20px;

    .action-btn {
        flex: 1;
        padding: 10px;
        border: none;
        border-radius: 6px;
        font-size: 12px;
        font-weight: bold;
        cursor: pointer;
        transition: all 0.3s;

        &.save {
            background: #4CAF50;
            color: white;

            &:hover {
                background: #45a049;
            }
        }

        &.load {
            background: #2196F3;
            color: white;

            &:hover {
                background: #1976D2;
            }
        }

        &.reset {
            background: #ff9800;
            color: white;

            &:hover {
                background: #f57c00;
            }
        }
    }
}

// 自定义滑块样式
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