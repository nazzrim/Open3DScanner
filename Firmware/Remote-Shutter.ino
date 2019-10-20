#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "BLE2902.h"
#include "BLEHIDDevice.h"
#include "HIDTypes.h"
#include "HIDKeyboardTypes.h"
#include <driver/adc.h>

/**
 * The remote shutter for this application is a BLE service which operates as keyboard/user device (depending on the connected device [iOS/Android]).
 * BLE logic as well as the actual shutter code can be found here.
 */
 
/**
 * Definitions can be found in EEPROM.ino
 */
extern const uint8_t EEPROM_CAMERA_TYPE_POS;
extern const uint8_t EEPROM_CAMERA_TYPE_ANDROID_DEVICE;
extern const uint8_t EEPROM_CAMERA_TYPE_IOS_DEVICE;

/**
 * Relevant data for BLE functionality.
 * 
 * The BLE HID code is based on https://github.com/nkolban/esp32-snippets/issues/230#issuecomment-474594017
 */

// HID device containing different charactersitics which form the services
BLEHIDDevice* hid;
// keyboard input characteristic
BLECharacteristic* input;
// keyboard output characteristic
BLECharacteristic* output;
// characteristics for media keys (e.g. volume up)
BLECharacteristic* volume;

// indicator if currently a BLE connection is established
bool hid_connected = false;

/**
 * The server callbacks handle starting and ending BLE connections
 */
class BLECallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer){
    hid_connected = true;
    BLE2902* desc = (BLE2902*)input->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    desc->setNotifications(true);
  }

  void onDisconnect(BLEServer* pServer){
    hid_connected = false;
    BLE2902* desc = (BLE2902*)input->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    desc->setNotifications(false);
  }
};

/*
 * This callback is connect with output report. In keyboard output report report special keys changes, like CAPSLOCK, NUMLOCK
 * We can add digital pins with LED to show status
 * bit 0 - NUM LOCK
 * bit 1 - CAPS LOCK
 * bit 2 - SCROLL LOCK
 */
 class BLEOutputCallbacks : public BLECharacteristicCallbacks {
 void onWrite(BLECharacteristic* me){
    uint8_t* value = (uint8_t*)(me->getValue().c_str());
    ESP_LOGI(LOG_TAG, "special keys: %d", *value);
  }
};

/**
 * This function will be executed as freeRTOS task.
 * It configures and starts the BLE device
 */
void ble_task_server(void*) {
    BLEDevice::init("Nazrim's 3D Scanner");
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new BLECallbacks());

    hid = new BLEHIDDevice(pServer);
    input = hid->inputReport(1); // <-- input REPORT_ID from report map
    output = hid->outputReport(1); // <-- output REPORT_ID from report map
    volume = hid->inputReport(2);
    output->setCallbacks(new BLEOutputCallbacks());
    // Set manufacturer name (OPTIONAL)
    std::string name = "Nazrim";
    hid->manufacturer()->setValue(name);
    // Set pnp parameters (MANDATORY)
    hid->pnp(0x02, 0xe502, 0xa111, 0x0210);
    // Set hid informations (MANDATORY)
    hid->hidInfo(0x00,0x02);
    /*
     * The report contains two different services:
     * - keyboard (relevant for android cameras)
     * - consumer control (media keys) for iOS
     */
    const uint8_t report[] = {
      USAGE_PAGE(1),      0x01,       // Generic Desktop Ctrls
      USAGE(1),           0x06,       // Keyboard
      COLLECTION(1),      0x01,       // Application
      REPORT_ID(1),       0x01,       // Report ID (1)
      USAGE_PAGE(1),      0x07,       // Kbrd/Keypad
      USAGE_MINIMUM(1),   0xE0,
      USAGE_MAXIMUM(1),   0xE7,
      LOGICAL_MINIMUM(1), 0x00,
      LOGICAL_MAXIMUM(1), 0x01,
      REPORT_SIZE(1),     0x01,       // 1 byte (Modifier)
      REPORT_COUNT(1),    0x08,
      HIDINPUT(1),        0x02,       // Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position
      REPORT_COUNT(1),    0x01,       // 1 byte (Reserved)
      REPORT_SIZE(1),     0x08,
      HIDINPUT(1),        0x01,       // Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position
      REPORT_COUNT(1),    0x01,       // 6 bytes (Keys)
      REPORT_SIZE(1),     0x08,
      LOGICAL_MINIMUM(1), 0x00,
      LOGICAL_MAXIMUM(1), 0x65,       // 101 keys
      USAGE_MINIMUM(1),   0x00,
      USAGE_MAXIMUM(1),   0x65,
      HIDINPUT(1),        0x00,       // Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position
      REPORT_COUNT(1),    0x05,       // 5 bits (Num lock, Caps lock, Scroll lock, Compose, Kana)
      REPORT_SIZE(1),     0x01,
      USAGE_PAGE(1),      0x08,       // LEDs
      USAGE_MINIMUM(1),   0x01,       // Num Lock
      USAGE_MAXIMUM(1),   0x05,       // Kana
      HIDOUTPUT(1),       0x02,       // Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile
      REPORT_COUNT(1),    0x01,       // 3 bits (Padding)
      REPORT_SIZE(1),     0x03,
      HIDOUTPUT(1),       0x01,       // Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile
      END_COLLECTION(0)
      ,
      0x05, 0x0c,                    // USAGE_PAGE (Consumer Devices)
      0x09, 0x01,                    // USAGE (Consumer Control)
      0x85, 0x02,
      0xa1, 0x01,                    // COLLECTION (Application)
                                    // -------------------- common global items
      0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
      0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
      0x75, 0x01,                    //   REPORT_SIZE (1)    - each field occupies 1 bit
                                    // -------------------- misc bits
      0x95, 0x05,                    //   REPORT_COUNT (5)
      0x09, 0xb5,                    //   USAGE (Scan Next Track)
      0x09, 0xb6,                    //   USAGE (Scan Previous Track)
      0x09, 0xb7,                    //   USAGE (Stop)
      0x09, 0xcd,                    //   USAGE (Play/Pause)
      0x09, 0xe2,                    //   USAGE (Mute)
      0x81, 0x06,                    //   INPUT (Data,Var,Rel)  - relative inputs
                                    // -------------------- volume up/down bits
      0x95, 0x02,                    //   REPORT_COUNT (2)
      0x09, 0xe9,                    //   USAGE (Volume Up)
      0x09, 0xea,                    //   USAGE (Volume Down)
      0x81, 0x02,                    //   INPUT (Data,Var,Abs)  - absolute inputs
                                    // -------------------- padding bit
      0x95, 0x01,                    //   REPORT_COUNT (1)
      0x81, 0x01,                    //   INPUT (Cnst,Ary,Abs)
      0xc0                           // END_COLLECTION
    };

    /*
     * Set report map (here is initialized device driver on client side) (MANDATORY)
     * https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.report_map.xml
     */
    hid->reportMap((uint8_t*)report, sizeof(report));
    
    /*
     * We are prepared to start hid device services. Before this point we can change all values and/or set parameters we need.
     * Also before we start, if we want to provide battery info, we need to prepare battery service.
     * We can setup characteristics authorization
     */
    hid->startServices();

    /*
     * Its good to setup advertising by providing appearance and advertised service. This will let clients find our device by type
     */
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->setAppearance(HID_KEYBOARD);
    pAdvertising->addServiceUUID(hid->hidService()->getUUID());
    pAdvertising->start();
    hid->setBatteryLevel(100);

    ESP_LOGD(LOG_TAG, "Advertising started!");

    BLESecurity *pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);


    while(true) {
      vTaskDelay(portMAX_DELAY);
    }
};

/**
 * Send the required keyboard inputs to a connected device which triggers to take a
 * photo as long as the camera app is opened.
 * 
 * Right now only android smartphones are supported.
 */
void ble_take_photo() {
  if (hid_connected) {
    uint8_t camera_type = EEPROM.read(EEPROM_CAMERA_TYPE_POS);

    if (camera_type == EEPROM_CAMERA_TYPE_ANDROID_DEVICE) {
      // for android the enter key (0x0A) triggers to take a photo
      const char triggerCameraAndroid = 0x0A;
      KEYMAP map = keymap[(uint8_t)triggerCameraAndroid];
      
      uint8_t triggerCameraAndroidDown[] = {map.modifier, 0x0, map.usage};
      input->setValue(triggerCameraAndroidDown, sizeof(triggerCameraAndroidDown));
      input->notify();
      
      uint8_t triggerCameraAndroidUp[] = {0x0, 0x0, 0x0};
      input->setValue(triggerCameraAndroidUp, sizeof(triggerCameraAndroidUp));
      input->notify();
    } else if (camera_type == EEPROM_CAMERA_TYPE_IOS_DEVICE) {
      // on iOS devices the volume up key must be triggered
      uint8_t vol_up_msg[] = {0x20};
      volume->setValue(vol_up_msg, sizeof(vol_up_msg));
      volume->notify();
  
      uint8_t vol_up_msg1[] = {0x00};
      volume->setValue(vol_up_msg1, sizeof(vol_up_msg1));
      volume->notify();
    }
  }
}

/**
 * Start a RTOS thread which takes care of the ble functionality.
 */
void initialize_BLE() {
  xTaskCreate(ble_task_server, "BleServer", 20000, NULL, 5, NULL);
}
