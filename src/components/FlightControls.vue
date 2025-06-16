<template>
    <div class="flight-controls">
      <!-- 飞行模式选择 -->
      <div class="mode-selector">
        <button 
          v-for="mode in flightModes" 
          :key="mode"
          :class="['mode-btn', { active: currentMode === mode }]"
          @click="$emit('setFlightMode', mode)"
        >
          {{ mode }}
        </button>
      </div>
  
      <!-- 解锁按钮 -->
      <button 
        :class="['arm-button', flightData.armed ? 'disarm' : 'arm']"
        @click="$emit('toggleArmed')"
      >
        {{ flightData.armed ? '锁定' : '解锁' }}
      </button>
    </div>
  </template>
  
  <script setup>
  defineProps({
    flightData: {
      type: Object,
      required: true
    },
    currentMode: {
      type: String,
      required: true
    },
    flightModes: {
      type: Array,
      default: () => ['MANUAL', 'STABILIZE', 'AUTO', 'RTL']
    }
  })
  
  defineEmits(['setFlightMode', 'toggleArmed'])
  </script>
  
  <style lang="less" scoped>
  .flight-controls {
    display: flex;
    flex-direction: column;
    gap: 8px;
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
  </style>