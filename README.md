# Fly-C 飞行器控制系统

一个基于ESP32的无人飞行器控制系统，包含硬件控制端和手机端控制应用。

## 项目概述

Fly-C是一个完整的飞行器控制解决方案，由两个主要组件组成：

- **🚁 飞行器控制端 (`fly/`)**: 基于ESP32的飞行器硬件控制系统
- **📱 手机控制端 (`flyC/`)**: 基于Tauri的跨平台手机控制应用

## 系统架构

```
    📱 手机控制端 (flyC)                   🚁 ESP32飞行器 (fly)
    ━━━━━━━━━━━━━━━━━━━━                    ━━━━━━━━━━━━━━━━━━━━
    Tauri + Vue3                         Arduino C++
    虚拟摇杆界面                          电机/舵机控制
    实时状态监控                          TCP服务器
    参数配置                             PWM信号输出
                    ┃                ┃
                    ┃   WiFi/TCP     ┃
                    ┃ ◄────────────► ┃
                    ┃                ┃
                    ▼                ▼
            [ 控制指令传输 ]    [ 状态数据反馈 ]
```

**工作流程**: 
🎮 摇杆操作 → 📶 WiFi传输 → 🔧 ESP32解析 → ⚡ PWM控制 → ✈️ 飞行器动作

## 快速开始

### 1. 硬件准备

#### 必需组件
- ESP32开发板 × 1
- 电调 (ESC) × 1  
- 舵机 × 3 (偏航、俯仰、翻滚)
- 无刷电机 × 1
- LED指示灯 × 1
- 锂电池及相关配件

#### 引脚连接
| 组件 | ESP32引脚 | 功能 |
|------|-----------|------|
| 电机电调 | GPIO 11 | PWM油门控制 |
| 偏航舵机 | GPIO 5 | PWM角度控制 |
| 俯仰舵机 | GPIO 6 | PWM角度控制 |
| 翻滚舵机 | GPIO 7 | PWM角度控制 |
| 状态LED | GPIO 10 | 状态指示 |

### 2. 飞行器控制端设置

#### 环境要求
- Arduino IDE 1.8.x+
- ESP32 Arduino核心包
- ESP32Servo库

#### 安装步骤
```bash
# 1. 配置Arduino IDE
# 添加ESP32板管理器URL: https://dl.espressif.com/dl/package_esp32_index.json

# 2. 安装必需库
# 在库管理器中安装: ESP32Servo

# 3. 配置WiFi (编辑 fly/fly.ino)
const char *ssid = "你的WiFi名称";
const char *password = "你的WiFi密码";

# 4. 编译上传到ESP32
```

详细说明请参考: [fly/README.md](fly/README.md)

### 3. 手机控制端设置

#### 环境要求
- Node.js 16.x+
- Rust 1.70+
- Yarn 或 npm
- Android SDK (用于Android构建)
- Xcode (用于iOS构建，仅macOS)

#### 安装步骤
```bash
# 进入手机应用目录
cd flyC

# 安装依赖
yarn install

# 开发模式运行 (桌面预览)
yarn tauri dev

# 构建Android版本
yarn tauri android dev

# 构建iOS版本
yarn tauri ios dev

# 构建生产版本
yarn tauri build
```

## 功能特性

### 🚁 飞行器控制端
- ✈️ 三轴飞行控制 (偏航/俯仰/翻滚)
- 🔧 自动电调校准
- 📶 WiFi TCP通信
- 💡 LED状态指示
- ⚡ 低延迟响应

### 📱 手机控制端  
- 🎮 直观的虚拟摇杆界面
- 📊 实时飞行状态监控
- ⚙️ 参数配置和调试
- 🔒 安全控制机制
- 📱 跨平台支持 (Android/iOS)
- 🌐 便携式无线控制

## 使用说明

### 基本操作流程
1. **硬件连接**: 按照引脚图连接所有硬件组件
2. **固件烧录**: 将控制程序上传到ESP32
3. **启动系统**: 给飞行器上电，等待WiFi连接
4. **启动控制端**: 运行手机控制应用
5. **建立连接**: 在控制应用中连接到飞行器
6. **安全测试**: 在安全环境下进行测试飞行

### 控制界面说明
- **左摇杆**: 油门控制和偏航控制
- **右摇杆**: 俯仰和翻滚控制  
- **状态面板**: 显示连接状态、电池电压等信息
- **参数面板**: 调整PID参数、控制灵敏度等

## 安全注意事项

⚠️ **重要安全提醒**

1. **🚫 首次测试必须移除螺旋桨**
2. **🏞️ 在开阔安全的环境中操作**
3. **📏 保持安全距离，准备随时断电**
4. **🔧 检查所有连接的牢固性**
5. **🔋 确认电池电量充足**
6. **📜 遵守当地无人机飞行法规**
7. **👥 避免在人群密集区域操作**

## 故障排除

### 常见问题

#### WiFi连接失败
- 检查SSID和密码配置
- 确认WiFi信号强度
- 重启ESP32设备

#### 控制无响应
- 检查TCP连接状态
- 验证引脚连接
- 重新校准电调

#### 舵机异常抖动
- 检查电源供应稳定性
- 确认PWM信号质量
- 调整控制参数

### 调试模式
```bash
# 查看ESP32串口输出 (115200波特率)
# 查看手机应用开发者控制台
yarn tauri dev

# Android设备调试
yarn tauri android dev

# iOS设备调试  
yarn tauri ios dev
```

## 开发者指南

### 项目结构
```
fly-c/
├── fly/                 # ESP32飞行器控制端
│   ├── fly.ino         # 主程序
│   ├── TCPManager.h    # TCP通信模块
│   └── README.md       # 硬件端说明
├── flyC/               # Tauri手机控制端
│   ├── src/           # Vue3源码
│   ├── src-tauri/     # Rust后端
│   ├── package.json   # 项目配置
│   └── README.md      # 手机端说明
└── README.md          # 项目总览
```

### 贡献代码
1. Fork项目
2. 创建功能分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送分支 (`git push origin feature/AmazingFeature`)
5. 开启Pull Request

### 技术栈
- **硬件端**: Arduino C++, ESP32, FreeRTOS
- **手机端**: TypeScript, Vue3, Tauri, Rust, Ant Design Vue
- **通信**: TCP/IP, WiFi, PWM

## 版本历史

- **v0.1.0**: 初始版本
  - 基础飞行控制功能
  - 手机控制应用原型
  - TCP通信协议

## 许可证

本项目采用 MIT 许可证 - 详情请查看 [LICENSE](LICENSE) 文件

## 致谢

感谢所有为这个项目贡献代码和建议的开发者们！

## 联系我们

如有问题或建议，请通过以下方式联系：
- 提交 [Issue](../../issues)
- 发起 [Discussion](../../discussions)

---

**⚠️ 免责声明**: 本项目仅供学习和研究使用。使用者须承担所有操作风险，项目作者不承担任何安全责任。请务必遵守当地法律法规和安全操作规程。 