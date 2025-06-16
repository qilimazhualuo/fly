<template>
    <div class="map-panel">
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
        <div>卫星数: {{ flightData.satellites }}</div>
      </div>
    </div>
  </template>
  
  <script setup>
  defineProps({
    flightData: {
      type: Object,
      required: true
    }
  })
  </script>
  
  <style lang="less" scoped>
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
      
      &:last-child {
        margin-bottom: 0;
      }
    }
  }
  </style>