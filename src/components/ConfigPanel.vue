<template>
  <div class="config-panel">
    <div class="config-section">
      <h4>舵面设置</h4>
      <div class="config-grid">
        <div class="config-item">
          <label>副翼行程</label>
          <input type="range" min="0" max="100" value="75">
          <span>75%</span>
        </div>
        <div class="config-item">
          <label>升降舵行程</label>
          <input type="range" min="0" max="100" value="80">
          <span>80%</span>
        </div>
        <div class="config-item">
          <label>方向舵行程</label>
          <input type="range" min="0" max="100" value="70">
          <span>70%</span>
        </div>
        <div class="config-item">
          <label>襟翼角度</label>
          <input type="range" min="-30" max="60" value="0">
          <span>0°</span>
        </div>
      </div>
    </div>

    <div class="config-section">
      <h4>飞行参数</h4>
      <div class="config-grid">
        <div class="config-item">
          <label>失速速度</label>
          <input type="number" value="12" class="number-input">
          <span>m/s</span>
        </div>
        <div class="config-item">
          <label>巡航速度</label>
          <input type="number" value="25" class="number-input">
          <span>m/s</span>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
// 配置面板不需要props，因为目前是静态配置
</script>

<style lang="less" scoped>
.config-panel {
  height: 100%;
  overflow-y: auto;
  padding-bottom: 20px;
  
  .config-section {
    margin-bottom: 15px;
    
    h4 {
      margin: 0 0 8px 0;
      color: #4CAF50;
      font-size: 13px;
      border-bottom: 1px solid #444;
      padding-bottom: 4px;
    }
  }
  
  .config-grid {
    display: grid;
    grid-template-columns: 1fr;
    gap: 8px;
  }
  
  .config-item {
    display: flex;
    align-items: center;
    gap: 8px;
    background: rgba(51, 51, 51, 0.8);
    padding: 6px;
    border-radius: 4px;
    backdrop-filter: blur(5px);
    
    label {
      flex: 0 0 70px;
      font-size: 10px;
      color: #ccc;
    }
    
    input[type="range"] {
      flex: 1;
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
    
    .number-input {
      flex: 1;
      padding: 3px 6px;
      background: rgba(68, 68, 68, 0.8);
      border: 1px solid #666;
      border-radius: 3px;
      color: white;
      font-size: 10px;
    }
    
    span {
      flex: 0 0 35px;
      font-size: 10px;
      color: #fff;
      text-align: right;
    }
  }
}
</style> 