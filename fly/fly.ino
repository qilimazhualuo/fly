#include <ESP32Servo.h>
#include "TCPManager.h"

// WiFi设置
const char *ssid = "CMCC-UM54";
const char *password = "123ZHANGbei";

// 定义电机连接的引脚
const int motorPin = 11;
const int yawPin = 5;    // 偏航舵机引脚
const int pitchPin = 6;  // 俯仰舵机引脚
const int rollPin = 7;   // 翻滚舵机引脚
const int lightPin = 10;

// 创建电调和舵机对象
Servo esc;
Servo yawServo;
Servo pitchServo;
Servo rollServo;
TCPManager tcpManager(esc, yawServo, pitchServo, rollServo);

void calibrateESC()
{
    Serial.println("开始电调校准...");
    
    // 发送最大油门信号
    Serial.println("设置最大油门");
    esc.writeMicroseconds(2000);
    delay(2000);
    
    // 发送最小油门信号
    Serial.println("设置最小油门");
    esc.writeMicroseconds(1000);
    delay(4000);
    
    Serial.println("电调校准完成");
}

void setup()
{
    Serial.begin(115200);
    Serial.println("启动中...");

    // 重新分配定时器，避免与WiFi冲突
    // WiFi通常使用Timer 0和1，我们使用Timer 2和3
    ESP32PWM::allocateTimer(2);  // 改用Timer 2
    ESP32PWM::allocateTimer(3);  // 改用Timer 3

    // 先只初始化电调，避免多PWM干扰
    // 如果400Hz与WiFi有冲突，可以尝试50Hz
    esc.setPeriodHertz(50);           // 尝试50Hz频率，避免与WiFi 2.4GHz冲突
    esc.attach(motorPin, 1000, 2000); // 引脚, 最小脉冲宽度1000us(0%油门), 最大脉冲宽度2000us(100%油门)

    Serial.println("电调已初始化");

    pinMode(lightPin, OUTPUT);
    digitalWrite(lightPin, HIGH);

    // 先设置为最小油门，确保电调能正确识别信号范围
    esc.writeMicroseconds(1000);
    delay(3000); // 等待电调完成初始化

    // 电调校准（使用BLE版本的成功流程）
    calibrateESC();

    Serial.println("油门设置为0%");

    // WiFi初始化前，确保电调处于稳定状态
    Serial.println("WiFi初始化前稳定电调...");
    esc.writeMicroseconds(1000);
    delay(1000);

    // 最后初始化WiFi和TCP服务器
    Serial.println("初始化网络服务...");
    tcpManager.init(ssid, password);

    // WiFi初始化后，重新确认电调状态
    Serial.println("WiFi初始化后稳定电调...");
    delay(500);
    esc.writeMicroseconds(1000);
    Serial.println("电调状态已重新确认");

    // 延迟初始化舵机，避免干扰电调
    Serial.println("初始化舵机...");
    yawServo.setPeriodHertz(50);        // 标准舵机50Hz
    yawServo.attach(yawPin, 1000, 2000);
    
    pitchServo.setPeriodHertz(50);
    pitchServo.attach(pitchPin, 1000, 2000);
    
    // SG90舵机使用正确的脉冲宽度范围
    rollServo.setPeriodHertz(50);
    rollServo.attach(rollPin, 500, 2500);  // SG90标准范围：500-2500μs

    Serial.println("舵机已初始化");

    // 设置舵机中立位置
    yawServo.writeMicroseconds(1500);   // 偏航中立
    pitchServo.writeMicroseconds(1500); // 拉升俯冲中立
    rollServo.writeMicroseconds(1500);  // 翻滚中立
    
    // 添加rollServo测试序列
    Serial.println("开始rollServo测试...");
    
    // 测试rollServo的完整范围
    Serial.println("rollServo -> 0度");
    rollServo.writeMicroseconds(500);
    delay(1000);
    
    Serial.println("rollServo -> 90度 (中立)");
    rollServo.writeMicroseconds(1500);
    delay(1000);
    
    Serial.println("rollServo -> 180度");
    rollServo.writeMicroseconds(2500);
    delay(1000);
    
    Serial.println("rollServo -> 90度 (中立) - 最终位置");
    rollServo.writeMicroseconds(1500);
    delay(500);
    
    Serial.println("rollServo测试完成");

    // 最后初始化WiFi和TCP服务器
    Serial.println("初始化网络服务...");
    tcpManager.init(ssid, password);
    
    Serial.println("系统启动完成！");
}

void loop()
{
    tcpManager.update();
    delay(10);
}