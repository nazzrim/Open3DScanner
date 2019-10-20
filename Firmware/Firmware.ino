#include <Arduino.h>                // Arduino
#include <EEPROM.h>                 // Arduino
#include <StackArray.h>             // Arduino
#include <BLEDevice.h>              // Arduino (https://github.com/nkolban/ESP32_BLE_Arduino)
#include <BLEUtils.h>               // Arduino (https://github.com/nkolban/ESP32_BLE_Arduino)
#include <BLEServer.h>              // Arduino (https://github.com/nkolban/ESP32_BLE_Arduino)
#include "BLE2902.h"                // Arduino (https://github.com/nkolban/ESP32_BLE_Arduino)
#include "BLEHIDDevice.h"           // Arduino (https://github.com/nkolban/ESP32_BLE_Arduino)
#include "HIDTypes.h"               // Arduino (https://github.com/nkolban/ESP32_BLE_Arduino)
#include "HIDKeyboardTypes.h"       // Arduino (https://github.com/nkolban/ESP32_BLE_Arduino)
#include <driver/adc.h>             // https://github.com/espressif/esp-idf
#include "soc/timer_group_struct.h" // https://github.com/espressif/esp-idf
#include "soc/timer_group_reg.h"    // https://github.com/espressif/esp-idf
#include <Nokia_LCD.h>              // https://github.com/platisd/nokia-5110-lcd-library
#include <A4988.h>                  // https://github.com/laurb9/StepperDriver
#include <ESP32Encoder.h>           // https://github.com/madhephaestus/ESP32Encoder/
#include <MenuSystem.h>             // https://github.com/jonblack/arduino-menusystem

/**
 * Begin of pin definitions
 */

/**
 * Free GPIOs:
 * - GPIO 0 (G0 - outputs PWM signal at boot)
 * - GPIO 3 (RX0 - HIGH at boot)
 */

 /**
  * Note: Because of the limited amount of GPIO Pins on the ESP32
  * the microstepping functionality is not used within the application.
  * Instead sixteenth microstepping is used by default for both steppers.
  * 
  * If in a future version a better solution is found the functionality
  * is added again. No code was removed, only outcommented.
  * 
  * If you want to ommit on some parts in favor of microstepping you can
  * return to the "old" state.
  * 
  * Relevant changes are in ESP32-3D_Scanner_-_Menu (microstepping submenues are not inserted into menu structure),
  * ESP32-3D_Scanner_-_Stepper-Rotor/Turntable (Microstepping function always returns sixteenth stepping and constructor
  * does not use microstepping pins anymore)
  */

// Pins used by the Nokia 5110 LCD Display
const uint8_t DISPLAY_RST = 13; // reset pin
const uint8_t DISPLAY_CE = 33;  // chip enable pin
const uint8_t DISPLAY_DC = 27;  // data/command selection pin
const uint8_t DISPLAY_DIN = 26; // serial input pin
const uint8_t DISPLAY_CLK = 25; // clock pin
const uint8_t DISPLAY_BL = 12; // background light pin

// Encoder pins used by Encoder STEC12E08
const uint8_t ENCODER_PHASE_A = 23; // bottom right pin
const uint8_t ENCODER_PHASE_B = 22; // bottom left pin
const uint8_t ENCODER_BUTTON = 34;  // top left pin

// Pins used by the A4988 driving the rotor stepper
//uint8_t ROTOR_MS1 = 15;
//uint8_t ROTOR_MS2 = 2;
//uint8_t ROTOR_MS3 = 4;
const uint8_t ROTOR_ENABLE = 18;
const uint8_t ROTOR_STEP = 17;
const uint8_t ROTOR_DIR = 16;

// Pins used by the A4988 driving the turntable stepper
//uint8_t TURNTABLE_MS1 = 5;
//uint8_t TURNTABLE_MS2 = 18;
//uint8_t TURNTABLE_MS3 = 19;
const uint8_t TURNTABLE_ENABLE = 5;
const uint8_t TURNTABLE_STEP = 14;
const uint8_t TURNTABLE_DIR = 19;

// Pins used by the bi-color status LED
const uint8_t STATUS_LED_GREEN = 2;
const uint8_t STATUS_LED_RED = 4;

// The transistor which switches light on/off needs a pin to drive the gate
const uint8_t LIGHT_SWITCH = 32;

// This is the number of scan presets defined in Scan.ino; information needed here since it is used in Menu.ino to build the correct menu structure
#define NUMBER_OF_SCAN_PRESETS 4

/**
 * End of pin definitions
 */

/**
 * Relevant data to keep track of the current application status.
 * Required for e.g. user input handling
 */
enum State {
  MENU,                                 // state for general menu navigation
  MENU_CHANGE_CONTRAST,                 // state for changing the contrast settings
  MENU_CHANGE_RPM_ROTOR,                // state for changing target RPM for rotor stepper
  MENU_CHANGE_ACCELERATION_ROTOR,       // state for changing acceleration for rotor stepper
  MENU_CHANGE_DECELERATION_ROTOR,       // state for changing deceleration for rotor stepper
  MENU_CHANGE_RPM_TURNTABLE,            // state for changing target RPM for turntable stepper
  MENU_CHANGE_ACCELERATION_TURNTABLE,   // state for changing acceleration for turntable stepper
  MENU_CHANGE_DECELERATION_TURNTABLE,   // state for changing deceleration for turntable stepper
  MENU_CHANGE_PHOTO_DELAY,              // state for changing the delay after a photo was taken
  MENU_CHANGE_MOVE_DELAY,               // state for changing the delay after a stepper was moved
  SCAN_CONFIGURE,                       // state for defining a custom scan
  SCAN_CONFIRM,                         // state for confirming chosen scan settings
  SCAN,                                 // state for scanning
};

// This is the current state of the application.
State current_state;

/**
 * This enum contains the valid microstepping values for the used A4988 drivers
 */
enum MicroStepping {
  FULL_STEP = 1,
  HALF_STEP = 2,
  QUARTER_STEP = 4,
  EIGHTH_STEP = 8,
  SIXTEENTH_STEP = 16
};

/**
 * Possible colors for the status LED.
 */
enum StatusLedColor : uint8_t {
  RED = 0,
  YELLOW = 1,
  GREEN = 2
};

/**
 * Possible blinking speeds for the status LED.
 */
enum StatusLedSpeed : uint8_t {
  SLOW = 0,
  FAST = 1,
  EXTRA_SLOW = 2
};

/**
 * The struct contains all settings which are required to perform a full scan.
 */
struct Scan {
  short photos_per_revolution;  // maximum "reasonable" value is rotor.microstepping * rotor.steps_per_revolution
  short rotation;               // maximum "reasonable" value is 360 since this setting is interpreted as degree value
  short rotation_divisions;     // maximum "reasonable" value is (rotor.microstepping * rotor.steps_per_revolution) / (rotation/360), while minimum value is 1 (start only)
};

/**
 * Scan presets are predefined scan objects with a name and scan settings which can be chosen from the menu.
 */
struct ScanPreset {
  char const* display_name;
  Scan preset;
};


// Used during MENU_CHANGE_CONTRAST state to store the current contrast value
uint8_t menu_change_contrast_current_value = 0;

// Used during MENU_CHANGE_CONTRAST state to store the current backlight state
uint8_t menu_change_contrast_backlight_state = 0;

// Used during ALL states which use the set min/max 12 bit function
unsigned short menu_set_min_max_12bit_value = 0;

// Used during ALL states which use the set 16 bit function
short menu_set_16bit_value = 0;

// This handle is used for task which run only a defined period of time and need to be deleted
TaskHandle_t rtos_task_handle = NULL;

// Actual lcd initialization can be found in Nokia5110-Display.ino
extern Nokia_LCD lcd;
// Can be found in Nokia5110-Display.ino
extern void initialize_display();

// Can be found in Encoder.ino
void encoder_button_isr();  

// Can be found in Stepper-Rotor.ino
extern A4988 stepper_rotor;
extern void initialize_stepper_rotor();

// Can be found in Stepper-Rotor.ino
extern float STEPPER_ROTOR_MIN_RPM;
extern float STEPPER_ROTOR_RPM_STEP_SIZE;
extern unsigned short STEPPER_ROTOR_MIN_ACCEL;
extern unsigned short STEPPER_ROTOR_ACCEL_STEP_SIZE;
extern unsigned short STEPPER_ROTOR_MIN_DECEL;
extern unsigned short STEPPER_ROTOR_DECEL_STEP_SIZE;

// Can be found in EEPROM.ino
extern const uint8_t EEPROM_STEPPER_ROTOR_RPM_POS;
extern const uint8_t EEPROM_STEPPER_ROTOR_RPM_MS_POS;
extern const uint8_t EEPROM_STEPPER_ROTOR_ACCEL_POS;
extern const uint8_t EEPROM_STEPPER_ROTOR_DECEL_POS;
extern const uint8_t EEPROM_STEPPER_ROTOR_ACCEL_DECEL_POS;


// Can be found in Stepper-Turntable.ino
extern A4988 stepper_turntable;
extern void initialize_stepper_turntable();

// Can be found in Stepper-Turntable.ino
extern float STEPPER_TURNTABLE_MIN_RPM;
extern float STEPPER_TURNTABLE_RPM_STEP_SIZE;
extern unsigned short STEPPER_TURNTABLE_MIN_ACCEL;
extern unsigned short STEPPER_TURNTABLE_ACCEL_STEP_SIZE;
extern unsigned short STEPPER_TURNTABLE_MIN_DECEL;
extern unsigned short STEPPER_TURNTABLE_DECEL_STEP_SIZE;

// Can be found in EEPROM.ino
extern const uint8_t EEPROM_STEPPER_TURNTABLE_RPM_POS;
extern const uint8_t EEPROM_STEPPER_TURNTABLE_RPM_MS_POS;
extern const uint8_t EEPROM_STEPPER_TURNTABLE_ACCEL_POS;
extern const uint8_t EEPROM_STEPPER_TURNTABLE_DECEL_POS;
extern const uint8_t EEPROM_STEPPER_TURNTABLE_ACCEL_DECEL_POS;
extern const uint8_t EEPROM_TIME_BETWEEN_PHOTO_LOW_POS;
extern const uint8_t EEPROM_TIME_BETWEEN_PHOTO_HIGH_POS;
extern const uint8_t EEPROM_TIME_AFTER_MOVE_LOW_POS;
extern const uint8_t EEPROM_TIME_AFTER_MOVE_HIGH_POS;

// Can be found in Menu.ino
extern void initialize_menu_structure();

// Can be found in State_Menu.ino
extern void menu_state_logic();

// Can be found in State-Menu-Change-Contrast.ino
extern void menu_change_contrast_state_logic();

// Can be found in State-Menu-Set-Min-Max-12Bit.ino
extern void menu_state_set_min_max_12bit_logic(
      const String affected_element,
      const String affected_setting,
      const bool use_float_values,
      const short min_short,
      const short step_short,
      const float min_float,
      const float step_float,
      const uint8_t store_address_a,
      const uint8_t store_address_b,
      const bool use_upper_bits);

/**
 * Begin of scroll multiplier values which are used in multiple locations.
 */


/**
 * Since there are 4096 different values some kind of scroll speed multiplier will help to make big adjustments.
 */
float scroll_speed_multiplier = 1.0;

/**
 * Counter how long no input was done. Required to reset the speed multiplier.
 */
uint8_t no_input_loops = 0;

/**
 * After how many frames without input the multiplier should be reset
 */
uint8_t multiplier_resetter = 6;

/**
 * Minimum scroll multiplier speed.
 */
float min_multiplier = 1.0;

/**
 * Maximum scroll multiplier speed.
 */
float max_multiplier = 10.0;

/**
 * This is a multiplier for the multiplier, and is used to increase the multiplier.
 */
float multplier_modifier = 1.16;

/**
 * End of multiplier flags.
 */

/**
 * Default arduino function. Will be called once on startup and is used to init
 * all subsystems.
 */
void setup() {
  // initialize serial connection for output printing
  Serial.begin(115200);

  // initialize all (hardware & software) subsystems
  initialize_light();
  initialize_status_led();
  initialize_EEPROM();
  initialize_BLE();
  initialize_display();
  initialize_stepper_rotor();
  initialize_stepper_turntable();
  initialize_encoder();
  initialize_scan();
  initialize_custom_scan();
  initialize_menu_structure();

  // application starts in menu state
  current_state = MENU;

  // start the main logic loop as freeRTOS task
  xTaskCreate(main_loop, "MainLoop", 20000, NULL, 5, NULL);
}

/**
 * The main logic loop will different logic functions depending on the current applications state.
 */
void main_loop(void*) {
  while (true) {
    // the BLE functionality seems to mess up interrupt handling.
    // therefore it is necessary to call this function here for proper user input handling.
    encoder_button_isr();
    // execute logic based on current application state
    if (current_state == SCAN_CONFIGURE) {
      scan_configure_state_logic();
    } else if (current_state == SCAN) {
      perform_scan();
    } else if (current_state == SCAN_CONFIRM) {
      scan_confirm_state_logic();
    } else if (current_state == MENU) {
      menu_state_logic();
    } else if(current_state == MENU_CHANGE_CONTRAST) {
      menu_change_contrast_state_logic();
    } else if(current_state == MENU_CHANGE_RPM_ROTOR) {
      menu_state_set_min_max_12bit_logic(
        "Rotor",
        "RPM",
        true,
        0,
        0,
        STEPPER_ROTOR_MIN_RPM,
        STEPPER_ROTOR_RPM_STEP_SIZE,
        EEPROM_STEPPER_ROTOR_RPM_POS,
        EEPROM_STEPPER_ROTOR_RPM_MS_POS,
        false);
    } else if(current_state == MENU_CHANGE_ACCELERATION_ROTOR) {
      menu_state_set_min_max_12bit_logic(
        "Rotor",
        "Acceleration",
        false,
        STEPPER_ROTOR_MIN_ACCEL,
        STEPPER_ROTOR_ACCEL_STEP_SIZE,
        0.0,
        0.0,
        EEPROM_STEPPER_ROTOR_ACCEL_POS,
        EEPROM_STEPPER_ROTOR_ACCEL_DECEL_POS,
        false);
    } else if(current_state == MENU_CHANGE_DECELERATION_ROTOR) {
      menu_state_set_min_max_12bit_logic(
        "Rotor",
        "Deceleration",
        false,
        STEPPER_ROTOR_MIN_DECEL,
        STEPPER_ROTOR_DECEL_STEP_SIZE,
        0.0,
        0.0,
        EEPROM_STEPPER_ROTOR_DECEL_POS,
        EEPROM_STEPPER_ROTOR_ACCEL_DECEL_POS,
        true);
    } else if(current_state == MENU_CHANGE_RPM_TURNTABLE) {
      menu_state_set_min_max_12bit_logic(
        "Turntable",
        "RPM",
        true,
        0,
        0,
        STEPPER_TURNTABLE_MIN_RPM,
        STEPPER_TURNTABLE_RPM_STEP_SIZE,
        EEPROM_STEPPER_TURNTABLE_RPM_POS,
        EEPROM_STEPPER_TURNTABLE_RPM_MS_POS,
        false);
    } else if(current_state == MENU_CHANGE_ACCELERATION_TURNTABLE) {
      menu_state_set_min_max_12bit_logic(
        "Turntable",
        "Acceleration",
        false,
        STEPPER_TURNTABLE_MIN_ACCEL,
        STEPPER_TURNTABLE_ACCEL_STEP_SIZE,
        0.0,
        0.0,
        EEPROM_STEPPER_TURNTABLE_ACCEL_POS,
        EEPROM_STEPPER_TURNTABLE_ACCEL_DECEL_POS,
        false);
    } else if(current_state == MENU_CHANGE_DECELERATION_TURNTABLE) {
      menu_state_set_min_max_12bit_logic(
        "Turntable",
        "Deceleration",
        false,
        STEPPER_TURNTABLE_MIN_DECEL,
        STEPPER_TURNTABLE_DECEL_STEP_SIZE,
        0.0,
        0.0,
        EEPROM_STEPPER_TURNTABLE_DECEL_POS,
        EEPROM_STEPPER_TURNTABLE_ACCEL_DECEL_POS,
        true);
    } else if(current_state == MENU_CHANGE_PHOTO_DELAY) {
      menu_state_set_16bit_logic(
          String("Photo Delay"),
          100,
          30000,
          EEPROM_TIME_BETWEEN_PHOTO_LOW_POS,
          EEPROM_TIME_BETWEEN_PHOTO_HIGH_POS);
    } else if(current_state == MENU_CHANGE_MOVE_DELAY) {
        menu_state_set_16bit_logic(
          String("Move Delay"),
          100,
          30000,
          EEPROM_TIME_AFTER_MOVE_LOW_POS,
          EEPROM_TIME_AFTER_MOVE_HIGH_POS);
    }
  }
}

/**
 * The default arduino loop is empty except of a delay since all functionality is handled with freeRTOS tasks.
 */
void loop() {
  delay(100);
}
