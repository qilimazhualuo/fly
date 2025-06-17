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
    Servo& rollServo;     // 翻滚舵机
    const int port;
    unsigned long lastSignalTime;  // 记录最后一次信号时间
    const unsigned long timeoutMs = 10000;  // 10秒超时
    bool controlActive = false;  // 标记控制是否激活

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
                int pulseWidth = map(roll, -100, 100, 1000, 2000);
                rollServo.writeMicroseconds(pulseWidth);
                Serial.print("设置翻滚: ");
                Serial.println(roll);
                
                // 更新最后信号时间
                lastSignalTime = millis();
                controlActive = true;
                
                server.send(200, "application/json", "{\"status\":\"OK\",\"value\":" + String(roll) + "}");
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

public:
    TCPManager(Servo& e, Servo& y, Servo& p, Servo& r, int serverPort = 80) 
        : esc(e), yawServo(y), pitchServo(p), rollServo(r), 
          server(serverPort), port(serverPort), lastSignalTime(0) {}

    void init(const char* ssid, const char* password) {
        // 连接到WiFi
        Serial.println("正在连接到WiFi...");
        WiFi.begin(ssid, password);
        
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        
        Serial.println("");
        Serial.println("WiFi已连接");
        Serial.print("IP地址: ");
        Serial.println(WiFi.localIP());
        Serial.print("HTTP端口: ");
        Serial.println(port);

        // 设置API路由
        server.on("/throttle", HTTP_GET, [this]() { this->handleThrottle(); });
        server.on("/yaw", HTTP_GET, [this]() { this->handleYaw(); });
        server.on("/pitch", HTTP_GET, [this]() { this->handlePitch(); });
        server.on("/roll", HTTP_GET, [this]() { this->handleRoll(); });
        server.on("/ping", HTTP_GET, [this]() { this->handlePing(); });
        
        // 启动Web服务器
        server.begin();
        Serial.println("HTTP服务器已启动");
        Serial.println("控制接口:");
        Serial.println("  油门: http://" + WiFi.localIP().toString() + "/throttle?value=0-100");
        Serial.println("  偏航: http://" + WiFi.localIP().toString() + "/yaw?value=-100-100");
        Serial.println("  拉升俯冲: http://" + WiFi.localIP().toString() + "/pitch?value=-100-100");
        Serial.println("  翻滚: http://" + WiFi.localIP().toString() + "/roll?value=-100-100");
        Serial.println("  Ping: http://" + WiFi.localIP().toString() + "/ping");
        
        // 初始化最后信号时间和舵机中立位置
        lastSignalTime = millis();
        yawServo.writeMicroseconds(1500);   // 偏航中立
        pitchServo.writeMicroseconds(1500); // 拉升俯冲中立
        rollServo.writeMicroseconds(1500);  // 翻滚中立
        esc.writeMicroseconds(1000);        // 油门归零
    }

    void update() {
        server.handleClient();
        checkTimeout();  // 检查超时
    }
};

#endif 