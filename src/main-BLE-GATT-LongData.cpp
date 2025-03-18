#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer *pServer = NULL;
BLECharacteristic *pTextCharacteristic = NULL;
bool deviceConnected = false;

// BLE UUIDs
#define SERVICE_UUID "19b10000-e8f2-537e-4f6c-d104768a1214"
#define TEXT_CHARACTERISTIC_UUID "19b10001-e8f2-537e-4f6c-d104768a1214"

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) { deviceConnected = true; }
  void onDisconnect(BLEServer *pServer) { deviceConnected = false; }
};

// Callback for receiving text data
class MyTextCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pTextCharacteristic) {
    Serial.print("OnWrite ");
    std::string value = pTextCharacteristic->getValue();
    if (value.length() > 0) {
      Serial.print("Received text: ");
      Serial.println(value.c_str());  // Print received string
    }
  }
};

void setup() {
  Serial.begin(115200);

  // Initialize BLE
  BLEDevice::init("ESP32_BLE_Receiver");

  // Create BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create BLE Characteristic for receiving text
  pTextCharacteristic = pService->createCharacteristic(
      TEXT_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_WRITE);  // Only Write

  // Register callback to handle received text
  pTextCharacteristic->setCallbacks(new MyTextCharacteristicCallbacks());

  // Add BLE2902 descriptor (not needed for Write-only, but avoids errors)
  pTextCharacteristic->addDescriptor(new BLE2902());

  // Start Service
  pService->start();

  // Start Advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x06);
  BLEDevice::startAdvertising();

  Serial.println("Waiting for BLE connection...");
}

void loop() {
  // Nothing needed in the loop since we only handle incoming BLE writes
}
