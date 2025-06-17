#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <BLESecurity.h>
#include <ESP32Servo.h>

// BLE服务和特征值UUID
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

class BLEManager {
    private:
        BLEServer* pServer;
        BLECharacteristic* pCharacteristic;
        bool deviceConnected;
        bool oldDeviceConnected;
        Servo& esc;

    // 连接状态回调
    class ServerCallbacks: public BLEServerCallbacks {
        private:
            bool& deviceConnected;
        public:
            ServerCallbacks(bool& connected) : deviceConnected(connected) {}
            void onConnect(BLEServer* pServer) override {
                deviceConnected = true;
                Serial.println("设备已连接");
                // 连接后停止广播
                BLEDevice::stopAdvertising();
            }
            void onDisconnect(BLEServer* pServer) override {
                deviceConnected = false;
                Serial.println("设备已断开");
            }
    };

    // 特征值回调
    class CharacteristicCallbacks: public BLECharacteristicCallbacks {
        private:
            Servo& esc;
        public:
            CharacteristicCallbacks(Servo& e) : esc(e) {}
            void onWrite(BLECharacteristic *pCharacteristic) override {
                String rxValue = pCharacteristic->getValue().c_str();
                if (rxValue.length() > 0) {
                    int throttle = rxValue.toInt();
                    // 确保油门在0-100%范围内
                    if (throttle >= 0 && throttle <= 100) {
                        // 将0-100的值映射到1000-2000微秒
                        int pulseWidth = map(throttle, 0, 100, 1000, 2000);
                        esc.writeMicroseconds(pulseWidth);
                        Serial.print("设置油门: ");
                        Serial.print(throttle);
                        Serial.println("%");
                    }
                }
            }
    };

    public:
        BLEManager(Servo& e) : esc(e), deviceConnected(false), oldDeviceConnected(false) {}

        void init() {
            // 初始化BLE
            BLEDevice::init("ESP32_FLY");

            // 设置安全参数
            BLESecurity *pSecurity = new BLESecurity();
            pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);  // 使用绑定模式
            pSecurity->setCapability(ESP_IO_CAP_NONE);  // 无输入输出能力
            pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
            
            // 创建BLE服务器
            pServer = BLEDevice::createServer();
            pServer->setCallbacks(new ServerCallbacks(deviceConnected));
            
            // 创建BLE服务
            BLEService *pService = pServer->createService(SERVICE_UUID);
            
            // 创建BLE特征
            pCharacteristic = pService->createCharacteristic(
                                CHARACTERISTIC_UUID,
                                BLECharacteristic::PROPERTY_READ |
                                BLECharacteristic::PROPERTY_WRITE |
                                BLECharacteristic::PROPERTY_NOTIFY
                            );
            
            pCharacteristic->setCallbacks(new CharacteristicCallbacks(esc));
            pCharacteristic->addDescriptor(new BLE2902());
            
            // 设置访问权限
            pCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE);
            
            // 启动服务
            pService->start();
            
            // 配置广播
            BLEAdvertising *pAdvertising = pServer->getAdvertising();
            pAdvertising->addServiceUUID(SERVICE_UUID);
            pAdvertising->setScanResponse(true);
            pAdvertising->setMinInterval(0x20); // 较快的广播间隔
            pAdvertising->setMaxInterval(0x40);
            
            BLEDevice::startAdvertising();
            
            Serial.println("BLE已启动，等待连接...");
            Serial.print("设备名称: ");
            Serial.println("ESP32_FLY");
        }

        void update() {
            // 处理连接状态变化
            if (!deviceConnected && oldDeviceConnected) {
                delay(500);
                pServer->startAdvertising();
                Serial.println("开始广播");
                oldDeviceConnected = deviceConnected;
            }
            
            if (deviceConnected && !oldDeviceConnected) {
                oldDeviceConnected = deviceConnected;
            }
        }

        bool isConnected() {
            return deviceConnected;
        }
};

#endif