#include <ESP32Servo.h>
#include "TCPManager.h"

// WiFi设置
const char *ssid = "CMCC-UM54";
const char *password = "123ZHANGbei";

// 定义电机连接的引脚
const int motorPin = 11;
const int yawPin = 16;    // 偏航舵机引脚
const int pitchPin = 4;  // 俯仰舵机引脚
const int leftRollPin = 13; // 左翻滚舵机引脚
const int rightRollPin = 9; // 右翻滚舵机引脚 (改为引脚9，GPIO8是SDIO_D3特殊功能引脚)
const int lightPin = 10;

// 创建电调和舵机对象
Servo esc;
// Servo yawServo;  // 暂时屏蔽偏航舵机
Servo pitchServo;
Servo leftRollServo;
Servo rightRollServo;
TCPManager tcpManager(esc, pitchServo, leftRollServo, rightRollServo);

void calibrateESC()
{
    Serial.println("开始电调校准...");
    
    // 发送最大油门信号
    Serial.println("设置最大油门");
    esc.writeMicroseconds(2000);
    delay(4000);
    
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

    // 最后初始化WiFi和TCP服务器（只调用一次）
    Serial.println("初始化网络服务...");
    tcpManager.init(ssid, password);
    
    // WiFi初始化后，重新确认电调状态
    Serial.println("WiFi初始化后稳定电调...");
    delay(500);
    esc.writeMicroseconds(1000);
    Serial.println("电调状态已重新确认");
    
    // 等待电调稳定，避免与舵机初始化冲突
    Serial.println("等待电调完全稳定...");
    delay(2000);  // 增加等待时间
    esc.writeMicroseconds(1000);  // 再次确认最小油门
    Serial.println("电调稳定完成，准备初始化舵机");

    // 延迟初始化舵机，避免干扰电调
    Serial.println("开始初始化舵机...");
    Serial.println("注意：如果电机响声，这可能是电调对PWM变化的正常响应");
    Serial.print("当前可用内存: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" 字节");
    
    // 分步初始化舵机，减少PWM冲击
    Serial.println("初始化俯仰舵机...");
    pitchServo.setPeriodHertz(50);
    pitchServo.attach(pitchPin, 1000, 2000);
    pitchServo.writeMicroseconds(1500);  // 立即设置中立位置
    delay(500);
    Serial.println("俯仰舵机初始化完成");
    
    Serial.println("初始化左翻滚舵机...");
    Serial.print("左翻滚舵机 - 设置频率...");
    leftRollServo.setPeriodHertz(50);
    Serial.println("完成");
    Serial.print("左翻滚舵机 - 连接引脚 ");
    Serial.print(leftRollPin);
    Serial.print("...");
    leftRollServo.attach(leftRollPin, 500, 2500);
    Serial.println("完成");
    Serial.print("左翻滚舵机 - 设置中立位置...");
    leftRollServo.writeMicroseconds(1500);  // 立即设置中立位置
    Serial.println("完成");
    delay(500);
    
    Serial.println("初始化右翻滚舵机...");
    Serial.print("右翻滚舵机 - 设置频率...");
    
    // 监控内存状态
    Serial.print("内存检查: ");
    Serial.print(ESP.getFreeHeap());
    Serial.println(" 字节");
    
    rightRollServo.setPeriodHertz(50);
    Serial.println("完成");
    Serial.print("右翻滚舵机 - 连接引脚 ");
    Serial.print(rightRollPin);
    Serial.print("...");
    
    // 添加看门狗重置，防止卡死
    Serial.println();
    Serial.println("准备连接右翻滚舵机引脚...");
    
    rightRollServo.attach(rightRollPin, 500, 2500);
    Serial.println("引脚连接完成");
    Serial.print("右翻滚舵机 - 设置中立位置...");
    rightRollServo.writeMicroseconds(1500);  // 立即设置中立位置
    Serial.println("完成");
    delay(500);
    Serial.println("右翻滚舵机初始化完全完成！");

    Serial.println("所有舵机已初始化并设置为中立位置");

    // 最终确认电调状态
    Serial.println("最终确认电调状态...");
    esc.writeMicroseconds(1000);
    delay(1000);
    
    // 暂时跳过舵机测试序列，直接启动服务
    Serial.println("跳过舵机测试序列，直接启动HTTP服务");
    
    // 最终系统状态确认
    Serial.println("执行最终系统状态确认...");
    esc.writeMicroseconds(1000);        // 确认油门为零
    pitchServo.writeMicroseconds(1500); // 确认俯仰中立
    leftRollServo.writeMicroseconds(1500);  // 确认左翻滚中立
    rightRollServo.writeMicroseconds(1500); // 确认右翻滚中立
    delay(500);  // 减少延迟时间

    Serial.println("=== 系统启动完成！===");
    Serial.println("HTTP服务器已就绪，可以接受连接");
    Serial.println("3舵机系统已激活 (电调+俯仰+双翻滚)");
    Serial.println("暂时屏蔽偏航控制以节约PWM资源");
    Serial.println("测试命令:");
    Serial.println("  curl http://192.168.1.9/ping");
    Serial.println("  curl http://192.168.1.9/device");
    Serial.println("  curl \"http://192.168.1.9/throttle?value=20\" (油门测试)");
    Serial.println("  curl \"http://192.168.1.9/pitch?value=30\" (俯仰测试)");
    Serial.println("  curl \"http://192.168.1.9/roll?value=50\" (右翻滚测试)");
    Serial.println("  curl \"http://192.168.1.9/roll?value=-50\" (左翻滚测试)");
    Serial.println("==========================");
}

void loop()
{
    tcpManager.update();
    
    // 每30秒输出一次状态信息，用于调试
    static unsigned long lastStatusReport = 0;
    if (millis() - lastStatusReport > 30000) {
        lastStatusReport = millis();
        Serial.println("=== 系统状态报告 ===");
        Serial.print("WiFi状态: ");
        Serial.println(WiFi.status() == WL_CONNECTED ? "已连接" : "未连接");
        if (WiFi.status() == WL_CONNECTED) {
            Serial.print("IP地址: ");
            Serial.println(WiFi.localIP());
            Serial.print("信号强度: ");
            Serial.print(WiFi.RSSI());
            Serial.println(" dBm");
        }
        Serial.print("系统运行时间: ");
        Serial.print(millis() / 1000);
        Serial.println(" 秒");
        Serial.print("可用内存: ");
        Serial.print(ESP.getFreeHeap());
        Serial.println(" 字节");
        Serial.println("HTTP服务器应该在端口80运行");
        Serial.println("尝试访问: http://" + WiFi.localIP().toString() + "/device");
        Serial.println("==================");
    }
    
    delay(10);
}