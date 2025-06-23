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
  myServo.attach(SERVO_PIN, 1000, 2000); // 调整脉冲宽度：1000-2000微秒
  
  // 设置LED引脚为输出
  pinMode(LED_PIN, OUTPUT);
  
  // 初始化舵机到90度位置
  Serial.println("正在初始化舵机到90度...");
  myServo.write(90);
  delay(2000);  // 增加延时让舵机有时间响应
  
  Serial.println("舵机已初始化到90度位置");
  Serial.println("如果舵机没动，请检查：");
  Serial.println("1. 电源是否足够(建议5V外接电源)");
  Serial.println("2. 接线是否正确");
  Serial.println("3. 舵机是否损坏");
  Serial.println("开始测试序列...");
}

void loop() {
  // 简化的测试序列，更容易观察
  Serial.println("\n=== 开始舵机测试 ===");
  
  // 测试0度位置
  Serial.println("设置舵机到0度位置...");
  digitalWrite(LED_PIN, HIGH);
  myServo.write(0);
  delay(3000);  // 等待3秒，观察舵机是否动作
  Serial.println("如果舵机正常，现在应该在0度位置");
  
  // 测试90度位置
  Serial.println("设置舵机到90度位置...");
  digitalWrite(LED_PIN, LOW);
  myServo.write(90);
  delay(3000);
  Serial.println("如果舵机正常，现在应该在90度位置");
  
  // 测试180度位置
  Serial.println("设置舵机到180度位置...");
  digitalWrite(LED_PIN, HIGH);
  myServo.write(180);
  delay(3000);
  Serial.println("如果舵机正常，现在应该在180度位置");
  
  // 返回中心位置
  Serial.println("返回90度中心位置...");
  digitalWrite(LED_PIN, LOW);
  myServo.write(90);
  delay(3000);
  
  Serial.println("=== 测试循环完成，等待5秒后重复 ===\n");
  delay(5000);
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