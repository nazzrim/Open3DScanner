#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include <A4988.h>

/**
 * The turntable stepper is used for rotating the scanned object around the z-axis.
 */

// turntable stepper's minimal RPM value
float STEPPER_TURNTABLE_MIN_RPM = 0.01;
// minimal difference between to RPM values for turntable stepper
float STEPPER_TURNTABLE_RPM_STEP_SIZE = 0.01;

// turntable stepper's minimal acceleration value
unsigned short STEPPER_TURNTABLE_MIN_ACCEL = 100;
// minimal difference between to acceleration values for turntable stepper
unsigned short STEPPER_TURNTABLE_ACCEL_STEP_SIZE = 1;

// turntable stepper's minimal deceleration value
unsigned short STEPPER_TURNTABLE_MIN_DECEL = 100;
// minimal difference between to deceleration values for turntable stepper
unsigned short STEPPER_TURNTABLE_DECEL_STEP_SIZE = 1;

// this is the number of steps the stepper needs for a full revolution
// typical values are 200 (1.8° stepper) and 400 (0.9° stepper)
short TURNTABLE_STEPS_PER_REVOLUTION = 200;

// The actual A4988 instance; microstepping disabled because the ESP32 offers not enough gpio pins for all functionality
A4988 stepper_turntable(TURNTABLE_STEPS_PER_REVOLUTION, TURNTABLE_DIR, TURNTABLE_STEP, TURNTABLE_ENABLE);//, TURNTABLE_MS1, TURNTABLE_MS2, TURNTABLE_MS3);

/**
 * Initialization of the turntable stepper loads all needed values from EEPROM
 * (RPM, microstepping, speedmode, acceleration, deceleration) and enables 
 * the motor.
 */
void initialize_stepper_turntable() {
  stepper_turntable.begin(load_stepper_turntable_rpm(), load_stepper_turntable_ms());
  stepper_turntable.setEnableActiveState(LOW);
  stepper_turntable.disable();
  update_stepper_turntable_speed_profile();
}

/**
 * Get the current RPM value for the turntable stepper.
 * The value is stored and EEPROM and is a combination of two bytes (one and a half to be more precise).
 */
float load_stepper_turntable_rpm() {
  // get rpm and ms settings from eeprom
  uint8_t rpm_byte = EEPROM.read(EEPROM_STEPPER_TURNTABLE_RPM_POS);
  uint8_t rpm_ms_byte = EEPROM.read(EEPROM_STEPPER_TURNTABLE_RPM_MS_POS);

  return STEPPER_TURNTABLE_MIN_RPM + (STEPPER_TURNTABLE_RPM_STEP_SIZE * (((rpm_ms_byte & 0x0F) << 8) | rpm_byte));
}

/**
 * Get the microstepping settings for the turntable stepper.
 * The value is stored in the upper half of a byte in EEPROM which is used for RPM settings.
 */
uint8_t load_stepper_turntable_ms() {
  // get ms settings from eeprom
  /*uint8_t rpm_ms_byte = EEPROM.read(EEPROM_STEPPER_TURNTABLE_RPM_MS_POS);

  if (bitRead(rpm_ms_byte, 4) == 1) {
    return HALF_STEP;
  } else if (bitRead(rpm_ms_byte, 5) == 1) {
    return QUARTER_STEP;
  } else if (bitRead(rpm_ms_byte, 6) == 1) {
    return EIGHTH_STEP;
  } else if (bitRead(rpm_ms_byte, 7) == 1) {
    return SIXTEENTH_STEP;
  }

  return FULL_STEP;*/
  return SIXTEENTH_STEP;
}

/**
 * Load the speed profile for the turntable motor from EEPROM.
 * The profile contains data about the speed type, acceleration and deceleration.
 */
void update_stepper_turntable_speed_profile() {
  // load EEPROM data
  uint8_t accel_byte = EEPROM.read(EEPROM_STEPPER_TURNTABLE_ACCEL_POS);
  uint8_t decel_byte = EEPROM.read(EEPROM_STEPPER_TURNTABLE_DECEL_POS);
  uint8_t accel_decel_byte = EEPROM.read(EEPROM_STEPPER_TURNTABLE_ACCEL_DECEL_POS);
  uint8_t bitmask = EEPROM.read(EEPROM_STEPPER_BITMASK_POS) & 0x0F;
  // create data for speed profile
  BasicStepperDriver::Mode speed_mode = (bitRead(bitmask, 4) == 1) ? stepper_turntable.LINEAR_SPEED : stepper_turntable.CONSTANT_SPEED;
  unsigned short accel = STEPPER_TURNTABLE_MIN_ACCEL + (STEPPER_TURNTABLE_ACCEL_STEP_SIZE * ((accel_decel_byte & 0x0F) << 8) | accel_byte);
  unsigned short decel = STEPPER_TURNTABLE_MIN_DECEL + (STEPPER_TURNTABLE_DECEL_STEP_SIZE * ((accel_decel_byte & 0xF0) << 4) | decel_byte);

  stepper_turntable.setSpeedProfile(speed_mode, accel, decel);
}

/**
 * This function will be executed as RTOS task if changes to the stepper configuration are applied.
 * It will periodically turn the stepper by 180 degrees into alternating direction.
 */
void stepper_turntable_debug_task(void*) {
  stepper_turntable.enable();
  long rotate_degrees = 30;
  while(true) {
    // start an asynchronous movement (triggering steps programatically is required)
    int8_t multiplier = (bitRead(EEPROM.read(EEPROM_STEPPER_BITMASK_POS), 5) == 0) ? 1 : -1;
    stepper_turntable.startRotate(multiplier*rotate_degrees);
    rotate_degrees *= -1;
    unsigned int wait_time = stepper_turntable.nextAction();
    while (stepper_turntable.getStepsRemaining() > 0) {
      // reset the watchdog manually since delayMicroseconds does not do it
      TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
      TIMERG0.wdt_feed=1;
      TIMERG0.wdt_wprotect=0;
      // wait till next action needs to be executed
      delayMicroseconds(wait_time);
      // execute next action
      wait_time = stepper_turntable.nextAction();
    }
    vTaskDelay(250);
  }
}
