#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif


#define SERVICE_UUID                    "030013ac-4202-2196-ec11-8417447de17d"
const uint16_t GATTS_CHAR_UUID_TEST_D = 0xFF04;

int number_of_devices_connected;
int ConnectId;

BLEServer* pServer;
BLECharacteristic* pChar1 = NULL;


class MyServerCallbacks: public BLEServerCallbacks
{
    void onConnect(BLEServer* pServer)
    {
      BLEDevice::startAdvertising();
      number_of_devices_connected++;
      ConnectId = pServer->getConnId();

      if (ALLOW_DEBUG) Serial.printf("connected! connection id: %d, number of devices: %d\n", ConnectId, number_of_devices_connected);
    }

    void onDisconnect(BLEServer* pServer)
    {
      number_of_devices_connected--;
      if (ALLOW_DEBUG) Serial.println("disconnected devices: " + String(number_of_devices_connected));
    }
};

class MyCallbacks: public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
      std::string rxValue = pCharacteristic->getValue();
      if (rxValue.length() > 0)
      {
        decode_packet(rxValue.c_str(), 0);
      }
    }

    void onRead(BLECharacteristic *pCharacteristic)
    {
    }

};

void setup_ble(String ble_name)
{

  // MAC Address
  if (ALLOW_DEBUG) Serial.print("*ESP Board MAC Address: ");
  if (ALLOW_DEBUG) Serial.println(WiFi.macAddress());

  // Create the BLE Device
  BLEDevice::init(ble_name.c_str());

  // MTU
  BLEDevice::setMTU(500);

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  //--------------------------------------------------------------------------------------------------
  // Create the BLE Service
  BLEService *pService1 = pServer->createService(SERVICE_UUID);
  pChar1 = pService1->createCharacteristic(
             GATTS_CHAR_UUID_TEST_D,
             BLECharacteristic::PROPERTY_NOTIFY
             | BLECharacteristic::PROPERTY_WRITE
           );

  pChar1->addDescriptor(new BLE2902());
  pChar1->setCallbacks(new MyCallbacks());

  pService1->start();

  //------------------------------------------------------------------------

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  //  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  //  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  if (ALLOW_DEBUG) Serial.println("*Done setting up BLE!");

}


void update_BLE_data(String packet, int dst)
{
  //  Serial.printf("trying to notify %s\n", packet);
  if (ALLOW_DEBUG) Serial.print("trying to notify: ");
  if (ALLOW_DEBUG) Serial.println(packet);
  pChar1->setValue((uint8_t *)packet.c_str(), packet.length());
  pChar1->notify();
}
