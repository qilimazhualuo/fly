# FlyC - 飞行器手机控制应用

基于Tauri + Vue3开发的跨平台飞行器控制手机应用，通过WiFi与ESP32飞行器进行实时通信控制。

## 应用简介

FlyC是Fly-C飞行器控制系统的手机端控制应用，提供直观的触屏操作界面，让用户可以通过手机轻松控制ESP32飞行器。

### 主要功能

- 🎮 **虚拟摇杆控制** - 双摇杆操作，支持油门、偏航、俯仰、翻滚控制
- 📶 **WiFi连接管理** - 自动发现并连接飞行器WiFi热点
- 📊 **实时状态监控** - 显示飞行器状态、电池电压、连接状态等
- ⚙️ **参数配置** - 控制灵敏度、PID参数调节
- 🔒 **安全控制** - 紧急停止、安全锁定等安全功能
- 📱 **响应式界面** - 适配不同尺寸手机屏幕

## 技术栈

- **前端框架**: Vue 3 + Composition API
- **构建工具**: Vite
- **桌面框架**: Tauri 2.x
- **UI组件**: Ant Design Vue
- **摇杆控件**: nipplejs
- **开发语言**: TypeScript + Rust
- **平台支持**: Android / iOS

## 开发环境搭建

### 前置要求

- Node.js 16.x 或更高版本
- Rust 1.70 或更高版本
- Yarn 或 npm
- **Android开发**: Android Studio + Android SDK
- **iOS开发**: Xcode (仅限macOS)

### 安装依赖

```bash
# 安装Node.js依赖
yarn install

# 或使用npm
npm install
```

### 开发调试

```bash
# 桌面端开发预览
yarn tauri dev

# Android设备调试
yarn tauri android dev

# iOS设备调试 (仅macOS)
yarn tauri ios dev
```

### 构建发布

```bash
# 构建桌面版本
yarn tauri build

# 构建Android APK
yarn tauri android build

# 构建iOS应用 (仅macOS)
yarn tauri ios build
```

## 项目结构

```
flyC/
├── src/
│   ├── components/          # Vue组件
│   │   ├── JoystickController.vue  # 摇杆控制器
│   │   ├── StatusPanel.vue         # 状态面板
│   │   └── SettingsPanel.vue       # 设置面板
│   ├── common/             # 公共模块
│   │   ├── tcp-client.js   # TCP通信客户端
│   │   └── utils.js        # 工具函数
│   ├── assets/             # 静态资源
│   ├── App.vue             # 主应用组件
│   └── main.js             # 入口文件
├── src-tauri/              # Tauri后端
│   ├── src/
│   │   └── lib.rs          # Rust主程序
│   ├── Cargo.toml          # Rust依赖配置
│   └── tauri.conf.json     # Tauri配置
├── public/                 # 公共资源
├── package.json            # Node.js配置
├── vite.config.js          # Vite配置
└── README.md               # 项目说明
```

## 使用说明

### 连接飞行器

1. 确保ESP32飞行器已启动并创建WiFi热点
2. 手机连接到飞行器的WiFi网络
3. 打开FlyC应用，点击"连接飞行器"
4. 等待连接成功提示

### 控制操作

#### 摇杆控制
- **左摇杆**: 
  - 上下: 油门控制 (向上增加油门，向下减少油门)
  - 左右: 偏航控制 (左转/右转)
- **右摇杆**:
  - 上下: 俯仰控制 (前进/后退)
  - 左右: 翻滚控制 (左倾/右倾)

#### 安全功能
- **紧急停止**: 点击紧急停止按钮立即停止所有电机
- **安全锁定**: 启用后防止误操作
- **低电量保护**: 电池电压过低时自动降落

### 参数设置

在设置面板中可以调整：
- 控制灵敏度 (0.1 - 2.0)
- 最大油门限制 (0% - 100%)
- PID参数微调
- 连接超时设置

## 通信协议

应用通过TCP协议与ESP32飞行器通信：

```json
{
  "type": "control",
  "data": {
    "throttle": 1500,    // 油门值 (1000-2000)
    "yaw": 1500,         // 偏航值 (1000-2000)  
    "pitch": 1500,       // 俯仰值 (1000-2000)
    "roll": 1500,        // 翻滚值 (1000-2000)
    "timestamp": 1234567890
  }
}
```

## 安全注意事项

⚠️ **重要提醒**

1. **首次使用前务必移除螺旋桨进行测试**
2. **确保手机电量充足，避免控制中断**
3. **在开阔安全的环境中操作**
4. **保持WiFi连接稳定，避免信号干扰**
5. **熟悉紧急停止操作**

## 故障排除

### 连接问题
- 检查WiFi连接状态
- 确认飞行器IP地址设置
- 重启应用和飞行器

### 控制延迟
- 检查WiFi信号强度
- 减少网络干扰源
- 调整控制频率设置

### 应用崩溃
- 查看开发者控制台错误信息
- 重新安装应用
- 检查设备兼容性

## 开发者说明

### 推荐IDE设置

- [VS Code](https://code.visualstudio.com/) + 以下扩展:
  - [Volar](https://marketplace.visualstudio.com/items?itemName=Vue.volar) - Vue支持
  - [Tauri](https://marketplace.visualstudio.com/items?itemName=tauri-apps.tauri-vscode) - Tauri支持  
  - [rust-analyzer](https://marketplace.visualstudio.com/items?itemName=rust-lang.rust-analyzer) - Rust支持

### 调试技巧

```bash
# 启用详细日志
yarn tauri dev -- --log-level debug

# 查看应用日志
yarn tauri logs

# 清理构建缓存
yarn tauri clean
```

### 贡献指南

1. Fork项目
2. 创建功能分支
3. 编写测试用例
4. 提交代码并创建Pull Request

## 许可证

本项目遵循 MIT 许可证

## 更新日志

### v0.1.0 (当前版本)
- 基础摇杆控制功能
- TCP通信实现
- 安全控制机制
- 跨平台支持

---

**注意**: 使用本应用控制飞行器时请务必遵守安全操作规程和当地法律法规。
