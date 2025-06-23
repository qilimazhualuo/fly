<script setup>
defineProps({
  flightData: {
    type: Object,
    required: true,
  },
})
</script>

<template>
  <div class="horizon-display">
    <div class="horizon-container">
      <div
        class="horizon-sky"
        :style="{
          transform: `rotate(${-flightData.roll}deg) translateY(${flightData.pitch * 1.5}px)`,
        }"
      ></div>
      <div
        class="horizon-ground"
        :style="{
          transform: `rotate(${-flightData.roll}deg) translateY(${flightData.pitch * 1.5}px)`,
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
</template>

<style lang="less" scoped>
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
    background: linear-gradient(to bottom, #87ceeb 0%, #4169e1 100%);
    transform-origin: center 150px;
  }

  .horizon-ground {
    position: absolute;
    width: 300px;
    height: 150px;
    left: -50px;
    top: 125px;
    background: linear-gradient(to bottom, #8b4513 0%, #654321 100%);
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
</style>
