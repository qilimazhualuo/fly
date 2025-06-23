<script setup>
import { ref, watch } from 'vue'
import { Slider } from 'ant-design-vue'

const props = defineProps({
  value: {
    type: Number,
    default: 0,
  },
})

const emit = defineEmits(['throttleChange'])

const throttleValue = ref(props.value)

// 监听外部value变化
watch(
  () => props.value,
  (newValue) => {
    throttleValue.value = newValue
  },
)

const handleThrottleChange = (value) => {
  emit('throttleChange', value)
}

const decreaseThrottle = () => {
  if (throttleValue.value > 0) {
    throttleValue.value = Math.max(0, throttleValue.value - 1)
    emit('throttleChange', throttleValue.value)
  }
}

const increaseThrottle = () => {
  if (throttleValue.value < 100) {
    throttleValue.value = Math.min(100, throttleValue.value + 1)
    emit('throttleChange', throttleValue.value)
  }
}
</script>

<template>
  <div class="throttle-control">
    <div class="throttle-label">
      <span>油门</span>
      <span class="throttle-value">{{ throttleValue.toFixed(0) }}%</span>
    </div>
    <button class="throttle-btn decrease" @click="decreaseThrottle" :disabled="throttleValue <= 0">
      -
    </button>
    <div class="throttle-slider-container">
      <Slider
        v-model:value="throttleValue"
        :min="0"
        :max="100"
        :step="1"
        :tooltip-formatter="(value) => `${value}%`"
        @change="handleThrottleChange"
        class="throttle-slider"
      />
    </div>
    <button
      class="throttle-btn increase"
      @click="increaseThrottle"
      :disabled="throttleValue >= 100"
    >
      +
    </button>
  </div>
</template>

<style lang="less" scoped>
.throttle-control {
  display: flex;
  align-items: center;
  gap: 12px;
  background: linear-gradient(135deg, rgba(30, 30, 30, 0.95), rgba(45, 45, 45, 0.95));
  border: 1px solid rgba(255, 255, 255, 0.15);
  border-radius: 12px;
  padding: 12px 18px;
  backdrop-filter: blur(15px);
  box-shadow:
    0 8px 32px rgba(0, 0, 0, 0.4),
    0 0 0 1px rgba(255, 255, 255, 0.05) inset;
  margin: 10px;
  position: relative;

  &::before {
    content: '';
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background: linear-gradient(
      135deg,
      rgba(76, 175, 80, 0.1) 0%,
      transparent 50%,
      rgba(33, 150, 243, 0.1) 100%
    );
    border-radius: 12px;
    pointer-events: none;
  }
}

.throttle-label {
  display: flex;
  flex-direction: column;
  align-items: center;
  font-size: 11px;
  color: rgba(255, 255, 255, 0.9);
  text-align: center;
  white-space: nowrap;
  min-width: 40px;
  font-weight: 500;
  text-shadow: 0 1px 2px rgba(0, 0, 0, 0.5);

  .throttle-value {
    color: #4caf50;
    font-family: 'Courier New', monospace;
    font-weight: bold;
    margin-top: 3px;
    font-size: 12px;
    text-shadow: 0 0 8px rgba(76, 175, 80, 0.3);
  }
}

.throttle-btn {
  width: 32px;
  height: 32px;
  border: none;
  border-radius: 8px;
  color: #fff;
  font-size: 16px;
  font-weight: bold;
  cursor: pointer;
  transition: all 0.3s ease;
  display: flex;
  align-items: center;
  justify-content: center;
  position: relative;
  overflow: hidden;
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.3);

  &::before {
    content: '';
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background: linear-gradient(135deg, rgba(255, 255, 255, 0.1), transparent);
    border-radius: 8px;
    opacity: 0;
    transition: opacity 0.3s ease;
  }

  &:hover:not(:disabled) {
    transform: translateY(-2px);
    box-shadow: 0 6px 20px rgba(0, 0, 0, 0.4);

    &::before {
      opacity: 1;
    }
  }

  &:active:not(:disabled) {
    transform: translateY(0);
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.3);
  }

  &:disabled {
    background: linear-gradient(135deg, rgba(68, 68, 68, 0.3), rgba(50, 50, 50, 0.3));
    color: rgba(255, 255, 255, 0.2);
    cursor: not-allowed;
    transform: none;
    box-shadow: none;
  }

  &.decrease {
    background: linear-gradient(135deg, #f44336, #d32f2f);

    &:hover:not(:disabled) {
      background: linear-gradient(135deg, #ff5722, #f44336);
    }
  }

  &.increase {
    background: linear-gradient(135deg, #4caf50, #388e3c);

    &:hover:not(:disabled) {
      background: linear-gradient(135deg, #66bb6a, #4caf50);
    }
  }
}

.throttle-slider-container {
  flex: 1;
  position: relative;

  .throttle-slider {
    width: 100%;
    margin: 0;

    :deep(.ant-slider-rail) {
      background: linear-gradient(
        to right,
        rgba(255, 255, 255, 0.08),
        rgba(255, 255, 255, 0.15),
        rgba(255, 255, 255, 0.08)
      );
      height: 8px;
      border-radius: 4px;
      box-shadow: 0 0 0 1px rgba(0, 0, 0, 0.1) inset;
    }

    :deep(.ant-slider-track) {
      background: linear-gradient(to right, #4caf50, #66bb6a, #8bc34a);
      height: 8px;
      border-radius: 4px;
      box-shadow:
        0 0 12px rgba(76, 175, 80, 0.4),
        0 2px 4px rgba(0, 0, 0, 0.2) inset;
    }

    :deep(.ant-slider-handle) {
      border: 3px solid #fff;
      background: linear-gradient(135deg, #4caf50, #66bb6a);
      width: 20px;
      height: 20px;
      margin-top: -6px;
      box-shadow:
        0 0 0 2px rgba(76, 175, 80, 0.3),
        0 4px 12px rgba(0, 0, 0, 0.3);
      transition: all 0.3s ease;

      &:hover,
      &:focus {
        border-color: #fff;
        background: linear-gradient(135deg, #66bb6a, #8bc34a);
        box-shadow:
          0 0 0 3px rgba(76, 175, 80, 0.4),
          0 0 20px rgba(76, 175, 80, 0.6),
          0 6px 16px rgba(0, 0, 0, 0.4);
        transform: scale(1.1);
      }
    }

    :deep(.ant-slider-handle::after) {
      box-shadow: none;
    }
  }
}
</style>
