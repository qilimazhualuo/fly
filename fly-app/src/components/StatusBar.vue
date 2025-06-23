<script setup>
defineProps({
  flightData: {
    type: Object,
    required: true,
  },
  currentMode: {
    type: String,
    required: true,
  },
  flightModes: {
    type: Array,
    default: () => ['MANUAL', 'STABILIZE', 'AUTO', 'RTL'],
  },
  activeTab: {
    type: String,
    required: true,
  },
  tabs: {
    type: Array,
    default: () => [
      { id: 'flight', name: '飞行', icon: '✈️' },
      { id: 'config', name: '配置', icon: '⚙️' },
      { id: 'map', name: '地图', icon: '🗺️' },
    ],
  },
})

defineEmits(['tabChange', 'setFlightMode'])
</script>

<template>
  <div class="status-bar">
    <div class="status-left">
      <span :class="['connection-status', flightData.connected ? 'connected' : 'disconnected']">
        {{ flightData.connected ? '●' : '○' }}
      </span>
      <div class="mode-selector">
        <button v-for="mode in flightModes" :key="mode" :class="['mode-btn', { active: currentMode === mode }]"
          @click="$emit('setFlightMode', mode)">
          {{ mode }}
        </button>
      </div>
    </div>

    <!-- 中间的Tab选择器 -->
    <div class="status-center">
      <select :value="activeTab" @change="$emit('tabChange', $event.target.value)" class="tab-select">
        <option v-for="tab in tabs" :key="tab.id" :value="tab.id">
          {{ tab.icon }} {{ tab.name }}
        </option>
      </select>
    </div>

    <div class="status-right">
      <span class="battery">🔋{{ flightData.battery.toFixed(0) }}%</span>
      <span class="satellites">📡{{ flightData.satellites }}</span>
    </div>
  </div>
</template>

<style lang="less" scoped>
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
    gap: 8px;
    align-items: center;
    flex: 1;

    .connection-status {
      &.connected {
        color: #4caf50;
      }

      &.disconnected {
        color: #f44336;
      }
    }

    .mode-selector {
      display: flex;
      gap: 8px;

      .mode-btn {
        background: rgba(68, 68, 68, 0.9);
        color: #fff;
        border: 1px solid rgba(255, 255, 255, 0.2);
        border-radius: 4px;
        padding: 3px 8px;
        font-size: 11px;
        cursor: pointer;
        outline: none;
        backdrop-filter: blur(5px);
        transition: all 0.3s ease;

        &:hover {
          background: rgba(33, 150, 243, 0.8);
          border-color: rgba(33, 150, 243, 0.8);
        }

        &:focus {
          border-color: #2196f3;
          box-shadow: 0 0 4px rgba(33, 150, 243, 0.5);
        }

        &.active {
          background: rgba(33, 150, 243, 0.8);
          border-color: rgba(33, 150, 243, 0.8);
        }
      }
    }
  }

  .status-center {
    flex: 0 0 auto;

    .tab-select {
      background: rgba(68, 68, 68, 0.9);
      color: #fff;
      border: 1px solid rgba(255, 255, 255, 0.2);
      border-radius: 4px;
      padding: 3px 8px;
      font-size: 11px;
      cursor: pointer;
      outline: none;
      backdrop-filter: blur(5px);
      transition: all 0.3s ease;

      &:hover {
        background: rgba(33, 150, 243, 0.8);
        border-color: rgba(33, 150, 243, 0.8);
      }

      &:focus {
        border-color: #2196f3;
        box-shadow: 0 0 4px rgba(33, 150, 243, 0.5);
      }

      option {
        background: rgba(26, 26, 26, 0.95);
        color: #fff;
        padding: 4px;
      }
    }
  }

  .status-right {
    display: flex;
    gap: 8px;
    font-size: 11px;
    flex: 1;
    justify-content: flex-end;
  }
}
</style>
