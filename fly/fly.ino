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

    // 允许分配通道给电调PWM
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    // 配置电调PWM
    esc.setPeriodHertz(400);          // 设置为400Hz，适合航模电调
    esc.attach(motorPin, 1000, 2000); // 引脚, 最小脉冲宽度1000us(0%油门), 最大脉冲宽度2000us(100%油门)

    // 配置舵机PWM
    yawServo.setPeriodHertz(50);        // 标准舵机50Hz
    yawServo.attach(yawPin, 1000, 2000);
    
    pitchServo.setPeriodHertz(50);
    pitchServo.attach(pitchPin, 1000, 2000);
    
    rollServo.setPeriodHertz(50);
    rollServo.attach(rollPin, 1000, 2000);

    Serial.println("电调和舵机已初始化");

    pinMode(lightPin, OUTPUT);
    digitalWrite(lightPin, HIGH);

    // 先设置为最小油门，确保电调能正确识别信号范围
    esc.writeMicroseconds(1000);
    delay(3000); // 等待电调完成初始化

    calibrateESC();

    Serial.println("油门设置为0%");

    // 初始化TCP服务器
    tcpManager.init(ssid, password);
}

void loop()
{
    tcpManager.update();
    delay(10);
}