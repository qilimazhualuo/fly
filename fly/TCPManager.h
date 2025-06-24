#ifndef TCP_MANAGER_H
#define TCP_MANAGER_H

#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

class TCPManager {
private:
    WebServer server;
    Servo& esc;           // 油门控制（电调）
    Servo& yawServo;      // 偏航舵机
    Servo& pitchServo;    // 拉升俯冲舵机
    Servo& rollServo;     // 翻滚舵机（合并后的单个舵机）
    const int port;
    unsigned long lastSignalTime;  // 记录最后一次信号时间
    const unsigned long timeoutMs = 10000;  // 10秒超时
    bool controlActive = false;  // 标记控制是否激活
    
    // 设备信息
    String deviceName = "ESP32_Fly_Controller";
    String deviceVersion = "1.0.0";
    String deviceType = "Aircraft_Controller";
    String macAddress = "";

    // 设备发现接口
    void handleDeviceInfo() {
        String response = "{";
        response += "\"device_name\":\"" + deviceName + "\",";
        response += "\"device_type\":\"" + deviceType + "\",";
        response += "\"version\":\"" + deviceVersion + "\",";
        response += "\"mac_address\":\"" + macAddress + "\",";
        response += "\"ip_address\":\"" + WiFi.localIP().toString() + "\",";
        response += "\"port\":" + String(port) + ",";
        response += "\"signal_strength\":" + String(WiFi.RSSI()) + ",";
        response += "\"free_heap\":" + String(ESP.getFreeHeap()) + ",";
        response += "\"uptime\":" + String(millis()) + ",";
        response += "\"wifi_ssid\":\"" + WiFi.SSID() + "\",";
        response += "\"control_active\":" + String(controlActive ? "true" : "false") + ",";
        response += "\"capabilities\":[\"throttle\",\"yaw\",\"pitch\",\"roll\"],";
        response += "\"api_endpoints\":{";
        response += "\"throttle\":\"/throttle?value=0-100\",";
        response += "\"yaw\":\"/yaw?value=-100-100\",";
        response += "\"pitch\":\"/pitch?value=-100-100\",";
        response += "\"roll\":\"/roll?value=-100-100 (single servo control)\",";
        response += "\"status\":\"/status\",";
        response += "\"ping\":\"/ping\"";
        response += "}";
        response += "}";
        
        server.send(200, "application/json", response);
        Serial.println("设备信息请求处理完成");
    }

    // 设备状态接口
    void handleStatus() {
        String response = "{";
        response += "\"timestamp\":" + String(millis()) + ",";
        response += "\"control_active\":" + String(controlActive ? "true" : "false") + ",";
        response += "\"last_signal_time\":" + String(lastSignalTime) + ",";
        response += "\"timeout_ms\":" + String(timeoutMs) + ",";
        response += "\"wifi_connected\":" + String(WiFi.status() == WL_CONNECTED ? "true" : "false") + ",";
        response += "\"signal_strength\":" + String(WiFi.RSSI()) + ",";
        response += "\"free_heap\":" + String(ESP.getFreeHeap()) + "";
        response += "}";
        
        server.send(200, "application/json", response);
        Serial.println("状态信息请求处理完成");
    }

    // 设备扫描接口（简化版设备发现）
    void handleScan() {
        String response = "{";
        response += "\"device_found\":true,";
        response += "\"name\":\"" + deviceName + "\",";
        response += "\"type\":\"" + deviceType + "\",";
        response += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
        response += "\"port\":" + String(port) + ",";
        response += "\"mac\":\"" + macAddress + "\",";
        response += "\"signal\":" + String(WiFi.RSSI()) + "";
        response += "}";
        
        server.send(200, "application/json", response);
        Serial.println("扫描请求处理完成");
    }

    void handleThrottle() {
        if (server.hasArg("value")) {
            int throttle = server.arg("value").toInt();
            if (throttle >= 0 && throttle <= 100) {
                int pulseWidth = map(throttle, 0, 100, 1000, 2000);
                esc.writeMicroseconds(pulseWidth);
                Serial.print("设置油门: ");
                Serial.print(throttle);
                Serial.println("%");
                
                // 更新最后信号时间
                lastSignalTime = millis();
                controlActive = true;
                
                server.send(200, "application/json", "{\"status\":\"OK\",\"value\":" + String(throttle) + "}");
            } else {
                server.send(400, "application/json", "{\"error\":\"Invalid throttle value\"}");
            }
        } else {
            server.send(400, "application/json", "{\"error\":\"Missing value parameter\"}");
        }
    }

    void handleYaw() {
        if (server.hasArg("value")) {
            int yaw = server.arg("value").toInt();
            if (yaw >= -100 && yaw <= 100) {
                int pulseWidth = map(yaw, -100, 100, 1000, 2000);
                yawServo.writeMicroseconds(pulseWidth);
                Serial.print("设置偏航: ");
                Serial.println(yaw);
                
                // 更新最后信号时间
                lastSignalTime = millis();
                controlActive = true;
                
                server.send(200, "application/json", "{\"status\":\"OK\",\"value\":" + String(yaw) + "}");
            } else {
                server.send(400, "application/json", "{\"error\":\"Invalid yaw value (-100 to 100)\"}");
            }
        } else {
            server.send(400, "application/json", "{\"error\":\"Missing value parameter\"}");
        }
    }

    void handlePitch() {
        if (server.hasArg("value")) {
            int pitch = server.arg("value").toInt();
            if (pitch >= -100 && pitch <= 100) {
                int pulseWidth = map(pitch, -100, 100, 1000, 2000);
                pitchServo.writeMicroseconds(pulseWidth);
                Serial.print("设置拉升俯冲: ");
                Serial.println(pitch);
                
                // 更新最后信号时间
                lastSignalTime = millis();
                controlActive = true;
                
                server.send(200, "application/json", "{\"status\":\"OK\",\"value\":" + String(pitch) + "}");
            } else {
                server.send(400, "application/json", "{\"error\":\"Invalid pitch value (-100 to 100)\"}");
            }
        } else {
            server.send(400, "application/json", "{\"error\":\"Missing value parameter\"}");
        }
    }

    void handleRoll() {
        if (server.hasArg("value")) {
            int roll = server.arg("value").toInt();
            if (roll >= -100 && roll <= 100) {
                // 单个翻滚舵机控制
                // -100到100映射到500-2500μs的脉冲宽度
                int pulseWidth = map(roll, -100, 100, 500, 2500);
                
                rollServo.writeMicroseconds(pulseWidth);
                
                Serial.print("设置翻滚: ");
                Serial.print(roll);
                Serial.print(" (脉冲宽度: ");
                Serial.print(pulseWidth);
                Serial.println("μs)");
                
                // 更新最后信号时间
                lastSignalTime = millis();
                controlActive = true;
                
                server.send(200, "application/json", "{\"status\":\"OK\",\"value\":" + String(roll) + ",\"pulse\":" + String(pulseWidth) + "}");
            } else {
                server.send(400, "application/json", "{\"error\":\"Invalid roll value (-100 to 100)\"}");
            }
        } else {
            server.send(400, "application/json", "{\"error\":\"Missing value parameter\"}");
        }
    }

    void handlePing() {
        // 更新最后信号时间
        lastSignalTime = millis();
        server.send(200, "application/json", "{\"status\":\"pong\",\"timestamp\":" + String(millis()) + "}");
        Serial.println("收到ping请求");
    }

    void checkTimeout() {
        if (controlActive && (millis() - lastSignalTime > timeoutMs)) {
            // 超时，将所有控制设置为中立位置
            esc.writeMicroseconds(1000);        // 油门归零
            yawServo.writeMicroseconds(1500);   // 偏航中立
            pitchServo.writeMicroseconds(1500); // 拉升俯冲中立
            rollServo.writeMicroseconds(1500);  // 翻滚中立
            controlActive = false;
            Serial.println("超时保护：所有控制已设置为安全位置");
        }
    }

    // 电调测试接口（用于排查滴滴声问题）
    void handleESCTest() {
        if (server.hasArg("pulse")) {
            int pulseWidth = server.arg("pulse").toInt();
            if (pulseWidth >= 1000 && pulseWidth <= 2000) {
                esc.writeMicroseconds(pulseWidth);
                Serial.print("测试电调脉冲宽度: ");
                Serial.print(pulseWidth);
                Serial.println("us");
                
                server.send(200, "application/json", 
                    "{\"status\":\"OK\",\"pulse\":" + String(pulseWidth) + ",\"message\":\"电调测试脉冲已发送\"}");
            } else {
                server.send(400, "application/json", 
                    "{\"error\":\"脉冲宽度必须在1000-2000us之间\"}");
            }
        } else {
            // 返回测试说明
            String response = "{";
            response += "\"message\":\"电调测试接口\",";
            response += "\"usage\":\"GET /esc_test?pulse=1000-2000\",";
            response += "\"examples\":[";
            response += "\"GET /esc_test?pulse=1000 (最小油门)\",";
            response += "\"GET /esc_test?pulse=1500 (中性位置)\",";
            response += "\"GET /esc_test?pulse=2000 (最大油门)\"";
            response += "],";
            response += "\"current_pulse\":\"" + String(1000) + "us\",";
            response += "\"tips\":[";
            response += "\"如果电调滴滴叫，尝试1000us\",";
            response += "\"某些电调需要特定的校准序列\",";
            response += "\"检查电池电压是否充足\"";
            response += "]";
            response += "}";
            
            server.send(200, "application/json", response);
        }
    }

    // 电调校准接口
    void handleESCCalibration() {
        Serial.println("通过HTTP接口启动电调校准...");
        
        // 校准序列
        esc.writeMicroseconds(2000);
        delay(2000);
        esc.writeMicroseconds(1000);
        delay(3000);
        esc.writeMicroseconds(1500);
        delay(1000);
        esc.writeMicroseconds(1000);
        
        Serial.println("HTTP电调校准完成");
        
        String response = "{";
        response += "\"status\":\"OK\",";
        response += "\"message\":\"电调校准序列已执行\",";
        response += "\"sequence\":[\"2000us->1000us->1500us->1000us\"],";
        response += "\"duration\":\"约6秒\",";
        response += "\"final_state\":\"最小油门(1000us)\"";
        response += "}";
        
        server.send(200, "application/json", response);
    }

public:
    TCPManager(Servo& e, Servo& y, Servo& p, Servo& r, int serverPort = 80) 
        : esc(e), yawServo(y), pitchServo(p), rollServo(r), 
          server(serverPort), port(serverPort), lastSignalTime(0) {}

    void init(const char* ssid, const char* password) {
        Serial.println("开始WiFi初始化...");
        
        // 连接到WiFi前的准备
        Serial.println("WiFi初始化准备中...");
        delay(500);
        
        // 分步WiFi初始化，减少对PWM的冲击
        WiFi.mode(WIFI_STA);
        delay(200);
        
        Serial.println("正在连接到WiFi...");
        WiFi.begin(ssid, password);
        
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 20) {
            delay(500);
            Serial.print(".");
            attempts++;
            
            // 每5次尝试后重新确认电调状态
            if (attempts % 5 == 0) {
                Serial.println();
                Serial.print("WiFi连接中... 重新稳定电调");
                // 注意：这里需要外部电调引用，暂时用串口提示
            }
        }
        
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println();
            Serial.println("WiFi连接失败，但继续启动服务");
        }
        
        // 获取MAC地址
        macAddress = WiFi.macAddress();
        
        Serial.println("");
        Serial.println("WiFi初始化完成");
        Serial.print("IP地址: ");
        Serial.println(WiFi.localIP());
        Serial.print("MAC地址: ");
        Serial.println(macAddress);
        Serial.print("HTTP端口: ");
        Serial.println(port);

        // 设置API路由
        server.on("/throttle", HTTP_GET, [this]() { this->handleThrottle(); });
        server.on("/yaw", HTTP_GET, [this]() { this->handleYaw(); });
        server.on("/pitch", HTTP_GET, [this]() { this->handlePitch(); });
        server.on("/roll", HTTP_GET, [this]() { this->handleRoll(); });
        server.on("/ping", HTTP_GET, [this]() { this->handlePing(); });
        
        // 设备发现和状态接口
        server.on("/device", HTTP_GET, [this]() { this->handleDeviceInfo(); });
        server.on("/info", HTTP_GET, [this]() { this->handleDeviceInfo(); });
        server.on("/status", HTTP_GET, [this]() { this->handleStatus(); });
        server.on("/scan", HTTP_GET, [this]() { this->handleScan(); });
        
        // 电调测试接口
        server.on("/esc_test", HTTP_GET, [this]() { this->handleESCTest(); });
        
        // 电调校准接口
        server.on("/esc_calibration", HTTP_GET, [this]() { this->handleESCCalibration(); });
        
        // 启动Web服务器
        server.begin();
        Serial.println("HTTP服务器已启动");
        Serial.println("控制接口:");
        Serial.println("  油门: http://" + WiFi.localIP().toString() + "/throttle?value=0-100");
        Serial.println("  偏航: http://" + WiFi.localIP().toString() + "/yaw?value=-100-100");
        Serial.println("  拉升俯冲: http://" + WiFi.localIP().toString() + "/pitch?value=-100-100");
        Serial.println("  翻滚: http://" + WiFi.localIP().toString() + "/roll?value=-100-100");
        Serial.println("  Ping: http://" + WiFi.localIP().toString() + "/ping");
        Serial.println("发现接口:");
        Serial.println("  设备信息: http://" + WiFi.localIP().toString() + "/device");
        Serial.println("  设备状态: http://" + WiFi.localIP().toString() + "/status");
        Serial.println("  设备扫描: http://" + WiFi.localIP().toString() + "/scan");
        
        // 初始化最后信号时间和舵机中立位置
        lastSignalTime = millis();
        yawServo.writeMicroseconds(1500);   // 偏航中立
        pitchServo.writeMicroseconds(1500); // 拉升俯冲中立
        rollServo.writeMicroseconds(1500);  // 翻滚中立
        esc.writeMicroseconds(1000);        // 油门归零
        
        Serial.println("TCP管理器初始化完成");
    }

    void update() {
        server.handleClient();
        checkTimeout();  // 检查超时
    }
    
    // 公共方法：设置设备名称
    void setDeviceName(const String& name) {
        deviceName = name;
    }
    
    // 公共方法：设置设备版本
    void setDeviceVersion(const String& version) {
        deviceVersion = version;
    }
};

#endif 