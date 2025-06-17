# ESP32 飞行器控制系统

基于ESP32开发的无人飞行器控制系统，支持通过WiFi进行远程控制，包含电机调速、舵机控制等功能。

## 功能特性

- ✈️ **飞行器控制**: 支持偏航、俯仰、翻滚三轴控制
- 🔧 **电调管理**: 自动电调校准，精确油门控制
- 📶 **WiFi通信**: 通过TCP协议进行远程控制
- 💡 **LED指示**: 状态指示灯
- 🎮 **实时控制**: 低延迟控制响应

## 硬件要求

### 主控板
- ESP32开发板

### 执行器
- 电调 (ESC) × 1
- 舵机 × 3 (偏航、俯仰、翻滚)
- 电机
- LED指示灯

### 连接引脚

| 组件 | 引脚 | 说明 |
|------|------|------|
| 电机电调 | GPIO 11 | PWM控制信号 |
| 偏航舵机 | GPIO 5 | PWM控制信号 |
| 俯仰舵机 | GPIO 6 | PWM控制信号 |
| 翻滚舵机 | GPIO 7 | PWM控制信号 |
| 状态LED | GPIO 10 | 数字输出 |

## 软件依赖

### Arduino库
- `ESP32Servo` - ESP32舵机控制库
- 自定义 `TCPManager` 库 - TCP通信管理

### 开发环境
- Arduino IDE 1.8.x 或更高版本
- ESP32 Arduino核心包

## 安装步骤

### 1. 环境准备
```bash
# 安装Arduino IDE
# 添加ESP32开发板支持
# 板管理器URL: https://dl.espressif.com/dl/package_esp32_index.json
```

### 2. 库安装
```bash
# 在Arduino IDE库管理器中搜索并安装：
- ESP32Servo
```

### 3. WiFi配置
在 `fly.ino` 文件中修改WiFi设置：
```cpp
const char *ssid = "你的WiFi名称";
const char *password = "你的WiFi密码";
```

### 4. 上传代码
1. 连接ESP32到电脑
2. 选择正确的开发板和端口
3. 编译并上传代码

## 使用说明

### 首次使用
1. **安全检查**: 确保螺旋桨已拆卸或飞行器处于安全环境
2. **上电初始化**: 系统会自动进行电调校准
3. **WiFi连接**: 等待WiFi连接成功
4. **TCP服务启动**: 系统将启动TCP服务等待连接

### 电调校准流程
系统启动时会自动执行电调校准：
1. 发送最大油门信号 (2000μs)
2. 发送最小油门信号 (1000μs)
3. 校准完成，设置为0%油门

### 控制协议
通过TCP连接发送控制指令：
- **油门控制**: 1000-2000μs PWM信号
- **舵机控制**: 标准舵机PWM信号 (50Hz)

## API接口

### 设备发现接口
系统提供了多个便于前端扫描和发现的HTTP接口：

#### 1. 设备信息接口
```
GET /device
GET /info
```
返回完整的设备信息，包括：
- 设备名称、类型、版本
- IP地址、MAC地址、端口
- WiFi信号强度、SSID
- 可用功能和API端点列表
- 当前控制状态

#### 2. 设备状态接口
```
GET /status
```
返回实时状态信息：
- 系统时间戳和运行时长
- 控制激活状态
- 最后信号时间
- WiFi连接状态
- 内存使用情况

#### 3. 设备扫描接口（简化版）
```
GET /scan
```
返回设备发现所需的基本信息：
- 设备名称和类型
- 网络信息（IP、MAC、端口）
- 信号强度

#### 4. 控制接口
```
GET /throttle?value=0-100      # 油门控制 (0-100%)
GET /yaw?value=-100-100        # 偏航控制 (-100到100)
GET /pitch?value=-100-100      # 俯仰控制 (-100到100)
GET /roll?value=-100-100       # 翻滚控制 (-100到100)
GET /ping                      # 心跳检测
```

### 前端扫描示例

#### JavaScript扫描代码
```javascript
// 扫描局域网中的飞行器设备
async function scanForDevices(ipRange = '192.168.1') {
    const devices = [];
    const promises = [];
    
    for (let i = 1; i <= 254; i++) {
        const ip = `${ipRange}.${i}`;
        const promise = fetch(`http://${ip}/scan`, {
            method: 'GET',
            timeout: 1000
        })
        .then(response => response.json())
        .then(data => {
            if (data.device_found) {
                devices.push({
                    ip: ip,
                    name: data.name,
                    type: data.type,
                    mac: data.mac,
                    signal: data.signal
                });
            }
        })
        .catch(() => {}); // 忽略连接失败的IP
        
        promises.push(promise);
    }
    
    await Promise.all(promises);
    return devices;
}

// 获取设备详细信息
async function getDeviceInfo(ip) {
    try {
        const response = await fetch(`http://${ip}/device`);
        return await response.json();
    } catch (error) {
        console.error('获取设备信息失败:', error);
        return null;
    }
}

// 控制飞行器
async function controlAircraft(ip, command, value) {
    try {
        const response = await fetch(`http://${ip}/${command}?value=${value}`);
        return await response.json();
    } catch (error) {
        console.error('控制失败:', error);
        return null;
    }
}
```

#### Python扫描代码
```python
import requests
import threading
import time

def scan_device(ip):
    """扫描单个IP地址"""
    try:
        response = requests.get(f'http://{ip}/scan', timeout=1)
        if response.status_code == 200:
            data = response.json()
            if data.get('device_found'):
                return {
                    'ip': ip,
                    'name': data.get('name'),
                    'type': data.get('type'),
                    'mac': data.get('mac'),
                    'signal': data.get('signal')
                }
    except:
        pass
    return None

def scan_network(ip_range='192.168.1'):
    """扫描整个网段"""
    devices = []
    threads = []
    results = []
    
    def worker(ip):
        device = scan_device(ip)
        if device:
            results.append(device)
    
    # 创建线程扫描
    for i in range(1, 255):
        ip = f'{ip_range}.{i}'
        thread = threading.Thread(target=worker, args=(ip,))
        threads.append(thread)
        thread.start()
    
    # 等待所有线程完成
    for thread in threads:
        thread.join()
    
    return results

# 使用示例
devices = scan_network()
for device in devices:
    print(f"发现设备: {device['name']} at {device['ip']}")
```

## 安全注意事项

⚠️ **重要安全提醒**

1. **首次测试必须移除螺旋桨**
2. **确保在开阔安全的环境中测试**
3. **保持安全距离，随时准备断电**
4. **检查所有连接是否牢固**
5. **确认电池电量充足**
6. **遵守当地无人机飞行法规**

## 调试信息

通过串口监视器（115200波特率）查看系统状态：
- 启动信息
- WiFi连接状态
- 电调校准过程
- TCP连接状态

## 故障排除

### 电调不响应
- 检查引脚连接
- 确认电源供应
- 重新执行校准流程

### WiFi连接失败
- 检查SSID和密码
- 确认信号强度
- 重启设备

### 舵机异常
- 检查PWM信号
- 确认电源电压
- 验证引脚配置

## 项目结构

```
fly/
├── fly.ino           # 主程序文件
├── TCPManager.h      # TCP通信管理头文件
├── TCPManager.cpp    # TCP通信管理实现
└── README.md         # 项目说明文档
```

## 许可证

本项目仅供学习和研究使用，使用者需承担所有安全责任。

## 贡献

欢迎提交问题和改进建议！

---

**注意**: 本项目涉及飞行器控制，使用前请确保已熟悉相关安全操作规程。 