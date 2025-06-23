#include <Arduino.h>
#line 1 "E:\\zb\\Documents\\GitHub\\fight\\testSg90\\testSg90.ino"
/*
 * SG90舵机测试程序 (ESP32版本)
 * 功能：测试SG90舵机的基本转动功能
 * 适用于：ESP32开发板
 * 作者：Arduino项目
 * 日期：2024
 */

#include <ESP32Servo.h>

// 创建Servo对象
Servo myServo;

// 定义引脚 (ESP32引脚)
const int SERVO_PIN = 4;   // 舵机信号线连接到GPIO4 (更安全的PWM引脚)
const int LED_PIN = 2;      // ESP32内置LED (GPIO2)

// 定义变量
int pos = 0;                // 存储舵机位置

#line 21 "E:\\zb\\Documents\\GitHub\\fight\\testSg90\\testSg90.ino"
void setup();
#line 48 "E:\\zb\\Documents\\GitHub\\fight\\testSg90\\testSg90.ino"
void loop();
#line 21 "E:\\zb\\Documents\\GitHub\\fight\\testSg90\\testSg90.ino"
void setup() {
  // 初始化串口通信
  Serial.begin(115200);     // ESP32通常使用115200波特率
  Serial.println("SG90舵机测试程序启动 (ESP32版本)");
  Serial.println("===============================");
  
  // ESP32Servo库需要设置PWM频率和分辨率
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  // 将舵机连接到指定引脚
  myServo.setPeriodHertz(50);    // 标准50Hz舵机
  myServo.attach(SERVO_PIN, 500, 2400); // 最小和最大脉冲宽度
  
  // 设置LED引脚为输出
  pinMode(LED_PIN, OUTPUT);
  
  // 初始化舵机到0度位置
  myServo.write(0);
  delay(1000);
  
  Serial.println("舵机已初始化到0度位置");
  Serial.println("开始测试序列...");
}

void loop() {
  // 测试1：慢速扫描 (0度到180度)
  Serial.println("\n=== 测试1：慢速扫描 (0°-180°) ===");
  digitalWrite(LED_PIN, HIGH);  // 点亮LED表示测试开始
  
  for (pos = 0; pos <= 180; pos += 1) {
    myServo.write(pos);
    Serial.print("当前角度: ");
    Serial.print(pos);
    Serial.println("°");
    delay(15);  // 每度之间等待15毫秒
  }
  
  digitalWrite(LED_PIN, LOW);
  delay(1000);
  
  // 测试2：快速返回 (180度到0度)
  Serial.println("\n=== 测试2：快速返回 (180°-0°) ===");
  digitalWrite(LED_PIN, HIGH);
  
  for (pos = 180; pos >= 0; pos -= 1) {
    myServo.write(pos);
    Serial.print("当前角度: ");
    Serial.print(pos);
    Serial.println("°");
    delay(5);   // 更快的返回速度
  }
  
  digitalWrite(LED_PIN, LOW);
  delay(1000);
  
  // 测试3：特定角度测试
  Serial.println("\n=== 测试3：特定角度定位测试 ===");
  int testAngles[] = {0, 45, 90, 135, 180, 90, 0};
  int numAngles = sizeof(testAngles) / sizeof(testAngles[0]);
  
  for (int i = 0; i < numAngles; i++) {
    digitalWrite(LED_PIN, HIGH);
    myServo.write(testAngles[i]);
    Serial.print("移动到: ");
    Serial.print(testAngles[i]);
    Serial.println("°");
    delay(1000);  // 在每个位置停留1秒
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
  
  // 测试4：摆动测试
  Serial.println("\n=== 测试4：摆动测试 ===");
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    myServo.write(45);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    myServo.write(135);
    delay(500);
  }
  
  // 回到中心位置
  myServo.write(90);
  Serial.println("\n舵机回到中心位置 (90°)");
  Serial.println("测试完成，等待5秒后重新开始...\n");
  
  delay(5000);  // 等待5秒后重新开始测试循环
}

/*
 * 辅助函数：平滑移动到指定角度
 */
void smoothMove(int targetAngle, int stepDelay = 15) {
  int currentAngle = myServo.read();
  
  if (currentAngle < targetAngle) {
    for (int angle = currentAngle; angle <= targetAngle; angle++) {
      myServo.write(angle);
      delay(stepDelay);
    }
  } else {
    for (int angle = currentAngle; angle >= targetAngle; angle--) {
      myServo.write(angle);
      delay(stepDelay);
    }
  }
} 
