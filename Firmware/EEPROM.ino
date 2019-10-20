#include <EEPROM.h>

/**
 * The EEPROM provides persistent memory to this application and is used to store configurations.
 * 
 * EEPROM size in bytes
 * 1 byte indicator if initialization is needed
 * 1 byte lcd backgroundlight [bit 7] lcd contrast [bit 0-6]
 * 1 byte determining the used camera type (0x1 = Android Device; 0x2 = iOS Device)
 * 2 bytes for rotor stepper RPM and microstepping (12 bits for rpm with rotor_rpm = byte_n byte_n+1[0:3] [4096 different settings] and
 *                                                  byte_n+1[4:7] = 0 --> microstepping = full step
 *                                                  byte_n+1[4:7] = 1 --> microstepping = half step
 *                                                  byte_n+1[4:7] = 2 --> microstepping = quarter step
 *                                                  byte_n+1[4:7] = 4 --> microstepping = eighth step
 *                                                  byte_n+1[4:7] = 8 --> microstepping = sixteenth step)
 * 3 bytes for rotor acceleration and deceleration (1.5 bytes/12 bits each) with 
 *                                                  accel = byte_n+2 & 0x0F << 8 | byte_n
 *                                                  decel = byte_n+2 & 0xF0 << 4 | byte_n+1
 * 1 byte for a shared stepper bitmask. bit 0 to 3 apply to rotor while 4 to 7 aply to the roll stepper.
 *                                                 The interpretation for each stepper is identical. So
 *                                                 semantic wise the following pairs are identically (0,4), (1,5), (2,6), (3,7).
 *                                                 Bit 0 determines the acceleration type (0 = constant speed; 1 = linear speed)
 *                                                 Bit 1 determines motor direction (0 = normal; 1 = inverted)
 * 2 bytes for turntable stepper RPM and microstepping
 * 3 bytes for turntable stepper acceleration and deceleration
 * 2 bytes for time between to photos during a scan
 * 2 bytes for delay after a stepper was moved during a print
 * 2 bytes for photos per revolution during a custom scan
 * 2 bytes for rotation during a custom scan
 * 2 bytes for rotation steps during a custom scan
 * 
 * Bitorder within a byte:
 * Value  | 2^7 | 2^6 | 2^5 | 2^4 | 2^3 | 2^2 | 2^1 | 2^0
 * Index  |   7 |   6 |   5 |   4 |   3 |   2 |   1 |   0
 * Most significant <-----------------> Least significant
 */
const uint8_t EEPROM_SIZE = 24;

const uint8_t EEPROM_INITIALIZED_POS = 0x00;
const uint8_t EEPROM_LCD_BL_CONTRAST_POS = 0x01;
const uint8_t EEPROM_CAMERA_TYPE_POS = 0x02;
const uint8_t EEPROM_STEPPER_ROTOR_RPM_POS = 0x03;
const uint8_t EEPROM_STEPPER_ROTOR_RPM_MS_POS = 0x04;
const uint8_t EEPROM_STEPPER_ROTOR_ACCEL_POS = 0x05;
const uint8_t EEPROM_STEPPER_ROTOR_DECEL_POS = 0x06;
const uint8_t EEPROM_STEPPER_ROTOR_ACCEL_DECEL_POS = 0x07;
const uint8_t EEPROM_STEPPER_BITMASK_POS = 0x08;
const uint8_t EEPROM_STEPPER_TURNTABLE_RPM_POS = 0x09;
const uint8_t EEPROM_STEPPER_TURNTABLE_RPM_MS_POS = 0x0A;
const uint8_t EEPROM_STEPPER_TURNTABLE_ACCEL_POS = 0x0B;
const uint8_t EEPROM_STEPPER_TURNTABLE_DECEL_POS = 0x0C;
const uint8_t EEPROM_STEPPER_TURNTABLE_ACCEL_DECEL_POS = 0x0D;
const uint8_t EEPROM_TIME_BETWEEN_PHOTO_LOW_POS = 0x0E;
const uint8_t EEPROM_TIME_BETWEEN_PHOTO_HIGH_POS = 0x0F;
const uint8_t EEPROM_TIME_AFTER_MOVE_LOW_POS = 0x10;
const uint8_t EEPROM_TIME_AFTER_MOVE_HIGH_POS = 0x11;
const uint8_t EEPROM_CUSTOM_SCAN_PHOTOS_PER_REVOLUTION_LOW_POS = 0x12;
const uint8_t EEPROM_CUSTOM_SCAN_PHOTOS_PER_REVOLUTION_HIGH_POS = 0x13;
const uint8_t EEPROM_CUSTOM_SCAN_ROTATION_LOW_POS = 0x14;
const uint8_t EEPROM_CUSTOM_SCAN_ROTATION_HIGH_POS = 0x15;
const uint8_t EEPROM_CUSTOM_SCAN_ROTATION_DIVISIONS_LOW_POS = 0x16;
const uint8_t EEPROM_CUSTOM_SCAN_ROTATION_DIVISIONS_HIGH_POS = 0x17;

/**
 * Define values for (e.g.) bitmask to simplify the usage.
 * This contains also default values.
 */
const uint8_t EEPROM_CAMERA_TYPE_ANDROID_DEVICE = 0x01;
const uint8_t EEPROM_CAMERA_TYPE_IOS_DEVICE = 0x02;
const uint8_t EEPROM_STEPPER_FULL_STEP = 0x00;
const uint8_t EEPROM_STEPPER_HALF_STEP = 0x01;
const uint8_t EEPROM_STEPPER_QUARTER_STEP = 0x02;
const uint8_t EEPROM_STEPPER_EIGHTH_STEP = 0x04;
const uint8_t EEPROM_STEPPER_SIXTEENTH_STEP = 0x08;
const uint8_t EEPROM_STEPPER_CONSTANT_SPEED = 0x00;
const uint8_t EEPROM_STEPPER_LINEAR_SPEED = 0x01;
const uint8_t EEPROM_STEPPER_DIR_NORMAL = 0x00;
const uint8_t EEPROM_STEPPER_DIR_INVERTED = 0x01;

const unsigned short EEPROM_STEPPER_ROTOR_DEFAULT_RPM = 999;
const uint8_t EEPROM_STEPPER_ROTOR_DEFAULT_MS = EEPROM_STEPPER_SIXTEENTH_STEP;
const unsigned short EEPROM_STEPPER_ROTOR_DEFAULT_ACCEL = 900;
const unsigned short EEPROM_STEPPER_ROTOR_DEFAULT_DECEL = 900;
const uint8_t EEPROM_STEPPER_ROTOR_DEFAULT_SPEED_TYPE = EEPROM_STEPPER_LINEAR_SPEED;

const unsigned short EEPROM_STEPPER_TURNTABLE_DEFAULT_RPM = 999;
const uint8_t EEPROM_STEPPER_TURNTABLE_DEFAULT_MS = EEPROM_STEPPER_SIXTEENTH_STEP;
const unsigned short EEPROM_STEPPER_TURNTABLE_DEFAULT_ACCEL = 900;
const unsigned short EEPROM_STEPPER_TURNTABLE_DEFAULT_DECEL = 900;
const uint8_t EEPROM_STEPPER_TURNTABLE_DEFAULT_SPEED_TYPE = EEPROM_STEPPER_LINEAR_SPEED;

const short EEPROM_DEFAULT_TIME_BEETWEEN_PHOTO = 3000;
const short EEPROM_DEFAULT_TIME_AFTER_MOVE = 1000;
const short EEPROM_DEFAULT_PHOTOS_PER_REVOLUTION = 32;
const short EEPROM_DEFAULT_ROTATION = 90;
const short EEPROM_DEFAULT_ROTATION_DIVISIONS = 3;

/** 
 * Change this value to anything within the range of 0 to 254 to trigger an EEPROM initialization on next startup. 
 * 
 * CAUTION: Do not use this inflationary since the flash cells have a limited amount of write cycles before
 * thes stop working.
 */
#define EEPROM_INITIALIZED_VALUE 128

/**
 * If the controller is started for the first time, the EEPROM needs to be filled with
 * meaningful values.
 */
void load_default_EEPROM_data() {
  EEPROM.write(EEPROM_INITIALIZED_POS, EEPROM_INITIALIZED_VALUE);
  EEPROM.write(EEPROM_LCD_BL_CONTRAST_POS, 0xBC);
  EEPROM.write(EEPROM_CAMERA_TYPE_POS, EEPROM_CAMERA_TYPE_ANDROID_DEVICE);
  EEPROM.write(EEPROM_STEPPER_ROTOR_RPM_POS, EEPROM_STEPPER_ROTOR_DEFAULT_RPM & 0xFF);
  EEPROM.write(EEPROM_STEPPER_ROTOR_RPM_MS_POS, (EEPROM_STEPPER_ROTOR_DEFAULT_MS << 4) | ((EEPROM_STEPPER_ROTOR_DEFAULT_RPM >> 8) & 0x0F));
  EEPROM.write(EEPROM_STEPPER_ROTOR_ACCEL_POS, EEPROM_STEPPER_ROTOR_DEFAULT_ACCEL & 0xFF);
  EEPROM.write(EEPROM_STEPPER_ROTOR_DECEL_POS, EEPROM_STEPPER_ROTOR_DEFAULT_DECEL & 0xFF);
  EEPROM.write(EEPROM_STEPPER_ROTOR_ACCEL_DECEL_POS, (((EEPROM_STEPPER_ROTOR_DEFAULT_ACCEL >> 8) & 0x0F) << 4) | ((EEPROM_STEPPER_ROTOR_DEFAULT_ACCEL >> 8) & 0x0F));
  EEPROM.write(EEPROM_STEPPER_BITMASK_POS,
                 // turntable
                 (EEPROM_STEPPER_DIR_NORMAL << 5) | (EEPROM_STEPPER_TURNTABLE_DEFAULT_SPEED_TYPE << 4)
                 // rotor
                 |  (EEPROM_STEPPER_DIR_INVERTED << 1) | EEPROM_STEPPER_ROTOR_DEFAULT_SPEED_TYPE);
  EEPROM.write(EEPROM_STEPPER_TURNTABLE_RPM_POS, EEPROM_STEPPER_TURNTABLE_DEFAULT_RPM & 0xFF);
  EEPROM.write(EEPROM_STEPPER_TURNTABLE_RPM_MS_POS, (EEPROM_STEPPER_TURNTABLE_DEFAULT_MS << 4) | ((EEPROM_STEPPER_TURNTABLE_DEFAULT_RPM >> 8) & 0x0F));
  EEPROM.write(EEPROM_STEPPER_TURNTABLE_ACCEL_POS, EEPROM_STEPPER_TURNTABLE_DEFAULT_ACCEL & 0xFF);
  EEPROM.write(EEPROM_STEPPER_TURNTABLE_DECEL_POS, EEPROM_STEPPER_TURNTABLE_DEFAULT_DECEL & 0xFF);
  EEPROM.write(EEPROM_STEPPER_TURNTABLE_ACCEL_DECEL_POS, (((EEPROM_STEPPER_TURNTABLE_DEFAULT_ACCEL >> 8) & 0x0F) << 4) | ((EEPROM_STEPPER_TURNTABLE_DEFAULT_ACCEL >> 8) & 0x0F));
  EEPROM.write(EEPROM_TIME_BETWEEN_PHOTO_LOW_POS, EEPROM_DEFAULT_TIME_BEETWEEN_PHOTO & 0xFF);
  EEPROM.write(EEPROM_TIME_BETWEEN_PHOTO_HIGH_POS, (EEPROM_DEFAULT_TIME_BEETWEEN_PHOTO >> 8) & 0xFF);
  EEPROM.write(EEPROM_TIME_AFTER_MOVE_LOW_POS, EEPROM_DEFAULT_TIME_AFTER_MOVE & 0xFF);
  EEPROM.write(EEPROM_TIME_AFTER_MOVE_HIGH_POS, (EEPROM_DEFAULT_TIME_AFTER_MOVE >> 8) & 0xFF);
  EEPROM.write(EEPROM_CUSTOM_SCAN_PHOTOS_PER_REVOLUTION_LOW_POS, EEPROM_DEFAULT_PHOTOS_PER_REVOLUTION & 0xFF);
  EEPROM.write(EEPROM_CUSTOM_SCAN_PHOTOS_PER_REVOLUTION_HIGH_POS, (EEPROM_DEFAULT_PHOTOS_PER_REVOLUTION >> 8) & 0xFF);
  EEPROM.write(EEPROM_CUSTOM_SCAN_ROTATION_LOW_POS, EEPROM_DEFAULT_ROTATION & 0xFF);
  EEPROM.write(EEPROM_CUSTOM_SCAN_ROTATION_HIGH_POS, (EEPROM_DEFAULT_ROTATION >> 8) & 0xFF);
  EEPROM.write(EEPROM_CUSTOM_SCAN_ROTATION_DIVISIONS_LOW_POS, EEPROM_DEFAULT_ROTATION_DIVISIONS & 0xFF);
  EEPROM.write(EEPROM_CUSTOM_SCAN_ROTATION_DIVISIONS_HIGH_POS, (EEPROM_DEFAULT_ROTATION_DIVISIONS >> 8) & 0xFF);

  EEPROM.commit();
}

/**
 * Start the EEPROM and check if it is already initialized.
 */
void initialize_EEPROM() {
  EEPROM.begin(EEPROM_SIZE);
  // determine if the EEPROM needs to be initialized
  if (EEPROM.read(EEPROM_INITIALIZED_POS) != EEPROM_INITIALIZED_VALUE) {
    Serial.println("This is the first run of the application. Going to initialize the EEPROM now.");
    load_default_EEPROM_data();
  }
}
