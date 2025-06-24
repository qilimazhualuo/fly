<script setup>
defineProps({
    flightData: {
        type: Object,
        required: true,
    },
    controls: {
        type: Object,
        required: true,
    }
})
</script>

<template>
    <div class="controller-display">
        <!-- 飞机可视化 -->
        <div class="aircraft-visual">
            <div class="aircraft">
                <!-- 机头 -->
                <div class="nose"></div>

                <!-- 驾驶舱 -->
                <div class="cockpit"></div>

                <!-- 机身 -->
                <div class="fuselage"></div>

                <!-- 主翼 -->
                <div class="main-wing">
                    <div class="wing left-wing" :class="{
                        'aileron-up': controls.aileron < 0,
                        'aileron-down': controls.aileron > 0
                    }">
                        <div class="wing-tip"></div>
                        <div class="aileron left-aileron" :class="{ active: controls.aileron < 0 }"></div>
                    </div>
                    <div class="wing right-wing" :class="{
                        'aileron-up': controls.aileron > 0,
                        'aileron-down': controls.aileron < 0
                    }">
                        <div class="wing-tip"></div>
                        <div class="aileron right-aileron" :class="{ active: controls.aileron > 0 }"></div>
                    </div>
                </div>

                <!-- 水平尾翼 -->
                <div class="horizontal-stabilizer">
                    <div class="elevator left-elevator" :class="{
                        'elevator-up': controls.elevator > 0,
                        'elevator-down': controls.elevator < 0
                    }"></div>
                    <div class="elevator right-elevator" :class="{
                        'elevator-up': controls.elevator > 0,
                        'elevator-down': controls.elevator < 0
                    }"></div>
                </div>

                <!-- 垂直尾翼 -->
                <div class="vertical-stabilizer">
                    <div class="rudder" :class="{
                        'rudder-left': controls.rudder < 0,
                        'rudder-right': controls.rudder > 0
                    }"></div>
                </div>

                <!-- 推进器指示 -->
                <div class="engine-flames" v-if="controls.throttle > 10">
                    <div class="flame" :style="{
                        width: (controls.throttle / 10) + 'px',
                        opacity: controls.throttle / 100
                    }"></div>
                </div>
            </div>
        </div>

        <!-- 控制参数显示 -->
        <div class="controls-grid">
            <div class="control-item">
                <div class="control-header">
                    <span class="label">高度</span>
                    <span class="value">{{ (flightData.altitude || 0).toFixed(0) }}m</span>
                </div>
                <div class="progress-bar">
                    <div class="progress-fill altitude"
                        :style="{ width: Math.min((flightData.altitude || 0) / 2, 100) + '%' }"></div>
                </div>
            </div>

            <div class="control-item">
                <div class="control-header">
                    <span class="label">油门</span>
                    <span class="value">{{ (controls.throttle || 0).toFixed(0) }}%</span>
                </div>
                <div class="progress-bar">
                    <div class="progress-fill throttle" :style="{ width: (controls.throttle || 0) + '%' }"></div>
                </div>
            </div>

            <div class="control-item">
                <div class="control-header">
                    <span class="label">副翼</span>
                    <span class="value">{{ (controls.aileron || 0).toFixed(0) }}</span>
                </div>
            </div>

            <div class="control-item">
                <div class="control-header">
                    <span class="label">水平舵</span>
                    <span class="value">{{ (controls.elevator || 0).toFixed(0) }}</span>
                </div>
            </div>

            <div class="control-item">
                <div class="control-header">
                    <span class="label">尾翼</span>
                    <span class="value">{{ (controls.rudder || 0).toFixed(0) }}</span>
                </div>
            </div>
        </div>
    </div>
</template>

<style lang="less" scoped>
.controller-display {
    background: rgba(42, 42, 42, 0.9);
    border-radius: 6px;
    padding: 8px;
    margin-bottom: 6px;
}

.aircraft-visual {
    display: flex;
    justify-content: center;
    align-items: center;
    padding: 10px;
    margin-bottom: 8px;
}

.aircraft {
    position: relative;
    width: 180px;
    height: 100px;
}

// 机头
.nose {
    position: absolute;
    left: 10px;
    top: 50%;
    transform: translateY(-50%);
    width: 18px;
    height: 8px;
    background: linear-gradient(135deg, #e0e0e0, #bdbdbd);
    border-radius: 50% 0 0 50%;
    z-index: 3;
    box-shadow: 0 1px 3px rgba(0, 0, 0, 0.3);
}

// 驾驶舱
.cockpit {
    position: absolute;
    left: 22px;
    top: 50%;
    transform: translateY(-50%);
    width: 16px;
    height: 10px;
    background: linear-gradient(135deg, #333, #666);
    border-radius: 4px;
    z-index: 3;
    box-shadow: inset 0 1px 2px rgba(255, 255, 255, 0.2);

    &::after {
        content: '';
        position: absolute;
        left: 50%;
        top: 50%;
        transform: translate(-50%, -50%);
        width: 8px;
        height: 4px;
        background: rgba(0, 100, 255, 0.3);
        border-radius: 2px;
    }
}

// 机身
.fuselage {
    position: absolute;
    left: 30px;
    top: 50%;
    transform: translateY(-50%);
    width: 80px;
    height: 12px;
    background: linear-gradient(135deg, #f5f5f5, #ddd);
    border-radius: 0 6px 6px 0;
    z-index: 2;
    box-shadow: 0 2px 5px rgba(0, 0, 0, 0.2);

    &::before {
        content: '';
        position: absolute;
        top: 50%;
        left: 10px;
        transform: translateY(-50%);
        width: 60px;
        height: 2px;
        background: linear-gradient(90deg, #2196f3, #03a9f4);
        border-radius: 1px;
    }
}

// 主翼
.main-wing {
    position: absolute;
    left: 50%;
    top: 50%;
    transform: translate(-50%, -50%);
}

.wing {
    position: absolute;
    width: 50px;
    height: 18px;
    background: linear-gradient(135deg, #e8e8e8, #c5c5c5);
    border-radius: 0 12px 12px 0;
    transition: all 0.3s ease;
    box-shadow: 0 3px 6px rgba(0, 0, 0, 0.3);

    &.left-wing {
        left: -60px;
        top: -9px;
        transform: rotate(-3deg);
        border-radius: 12px 0 0 12px;

        &.aileron-up {
            transform: rotate(-6deg);

            .aileron {
                transform: rotate(8deg);
                background: #2196f3;
                box-shadow: 0 0 10px #2196f3;
            }
        }

        &.aileron-down {
            transform: rotate(0deg);

            .aileron {
                transform: rotate(-8deg);
                background: #2196f3;
                box-shadow: 0 0 10px #2196f3;
            }
        }
    }

    &.right-wing {
        right: -60px;
        top: -9px;
        transform: rotate(3deg);

        &.aileron-up {
            transform: rotate(6deg);

            .aileron {
                transform: rotate(-8deg);
                background: #2196f3;
                box-shadow: 0 0 10px #2196f3;
            }
        }

        &.aileron-down {
            transform: rotate(0deg);

            .aileron {
                transform: rotate(8deg);
                background: #2196f3;
                box-shadow: 0 0 10px #2196f3;
            }
        }
    }
}

.wing-tip {
    position: absolute;
    top: 3px;
    width: 8px;
    height: 12px;
    background: linear-gradient(135deg, #ffeb3b, #ffc107);
    border-radius: 0 6px 6px 0;

    .left-wing & {
        left: -4px;
        border-radius: 6px 0 0 6px;
    }

    .right-wing & {
        right: -4px;
    }
}

.aileron {
    position: absolute;
    bottom: 0;
    width: 18px;
    height: 6px;
    background: #999;
    border-radius: 3px;
    transition: all 0.3s ease;
    transform-origin: center;

    &.left-aileron {
        right: 3px;
    }

    &.right-aileron {
        left: 3px;
    }

    &.active {
        background: #2196f3;
        box-shadow: 0 0 8px #2196f3;
    }
}

// 水平尾翼
.horizontal-stabilizer {
    position: absolute;
    right: 15px;
    top: 50%;
    transform: translateY(-50%);
    width: 20px;
    height: 8px;
    background: linear-gradient(135deg, #e0e0e0, #bdbdbd);
    border-radius: 4px;
    box-shadow: 0 1px 3px rgba(0, 0, 0, 0.2);
}

.elevator {
    position: absolute;
    width: 8px;
    height: 3px;
    background: #999;
    border-radius: 2px;
    transition: all 0.3s ease;
    transform-origin: center;

    &.left-elevator {
        left: 1px;
        bottom: -1px;

        &.elevator-up {
            transform: rotate(-15deg);
            background: #9c27b0;
            box-shadow: 0 0 6px #9c27b0;
        }

        &.elevator-down {
            transform: rotate(15deg);
            background: #9c27b0;
            box-shadow: 0 0 6px #9c27b0;
        }
    }

    &.right-elevator {
        right: 1px;
        bottom: -1px;

        &.elevator-up {
            transform: rotate(15deg);
            background: #9c27b0;
            box-shadow: 0 0 6px #9c27b0;
        }

        &.elevator-down {
            transform: rotate(-15deg);
            background: #9c27b0;
            box-shadow: 0 0 6px #9c27b0;
        }
    }
}

// 垂直尾翼
.vertical-stabilizer {
    position: absolute;
    right: 12px;
    top: 50%;
    transform: translateY(-50%);
    width: 6px;
    height: 20px;
    background: linear-gradient(135deg, #e0e0e0, #bdbdbd);
    border-radius: 3px 3px 0 0;
    box-shadow: 0 1px 3px rgba(0, 0, 0, 0.2);
}

.rudder {
    position: absolute;
    right: -2px;
    top: 2px;
    width: 3px;
    height: 12px;
    background: #999;
    border-radius: 2px;
    transition: all 0.3s ease;
    transform-origin: center;

    &.rudder-left {
        transform: rotate(-20deg);
        background: #607d8b;
        box-shadow: 0 0 6px #607d8b;
    }

    &.rudder-right {
        transform: rotate(20deg);
        background: #607d8b;
        box-shadow: 0 0 6px #607d8b;
    }
}

// 推进器火焰
.engine-flames {
    position: absolute;
    right: -5px;
    top: 50%;
    transform: translateY(-50%);
    z-index: 1;
}

.flame {
    height: 6px;
    background: linear-gradient(90deg, #ff5722, #ff9800, #ffeb3b);
    border-radius: 0 3px 3px 0;
    animation: flicker 0.1s infinite alternate;
    box-shadow: 0 0 8px rgba(255, 87, 34, 0.8);
}

@keyframes flicker {
    0% {
        opacity: 0.8;
        transform: scaleY(0.9);
    }

    100% {
        opacity: 1;
        transform: scaleY(1.1);
    }
}

.controls-grid {
    display: grid;
    grid-template-columns: repeat(1, 1fr);
    gap: 3px;
}

.control-item {
    display: flex;
    flex-direction: column;
    padding: 3px 6px;
    background: rgba(0, 0, 0, 0.2);
    border-radius: 3px;

    .control-header {
        display: flex;
        justify-content: space-between;
        align-items: center;

        .label {
            font-size: 10px;
            color: #ccc;
            font-weight: bold;
        }

        .value {
            font-size: 12px;
            font-weight: bold;
            color: #fff;
        }
    }

    .progress-bar {
        height: 3px;
        background: rgba(255, 255, 255, 0.1);
        border-radius: 2px;
        position: relative;
        overflow: hidden;
        margin-top: 2px;

        .progress-fill {
            height: 100%;
            border-radius: 2px;
            transition: all 0.1s ease;

            &.altitude {
                background: linear-gradient(90deg, #4caf50, #8bc34a);
            }

            &.throttle {
                background: linear-gradient(90deg, #ff9800, #ff5722);
            }
        }
    }
}
</style>
