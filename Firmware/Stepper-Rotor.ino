#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include <A4988.h>

/**
 * The rotor stepper is used for rotating the scanned object around the x-axis.
 */

// gear ratio which is used to drive the rotor
float STEPPER_ROTOR_GEAR_RATIO = 40/8.0;

// rotor stepper's minimal RPM value
float STEPPER_ROTOR_MIN_RPM = 0.01;
// minimal difference between to RPM values for rotor stepper
float STEPPER_ROTOR_RPM_STEP_SIZE = 0.01;

// rotor stepper's minimal acceleration value
unsigned short STEPPER_ROTOR_MIN_ACCEL = 100;
// minimal difference between to acceleration values for rotor stepper
unsigned short STEPPER_ROTOR_ACCEL_STEP_SIZE = 1;

// rotor stepper's minimal deceleration value
unsigned short STEPPER_ROTOR_MIN_DECEL = 100;
// minimal difference between to deceleration values for rotor stepper
unsigned short STEPPER_ROTOR_DECEL_STEP_SIZE = 1;

// this is the number of steps the stepper needs for a full revolution
// typical values are 200 (1.8° stepper) and 400 (0.9° stepper)
short ROTOR_STEPS_PER_REVOLUTION = 200;

// The actual A4988 instance; microstepping disabled because the ESP32 offers not enough gpio pins for all functionality
A4988 stepper_rotor(ROTOR_STEPS_PER_REVOLUTION, ROTOR_DIR, ROTOR_STEP, ROTOR_ENABLE);//, ROTOR_MS1, ROTOR_MS2, ROTOR_MS3);

/**
 * Initialization of the rotor stepper loads all needed values from EEPROM
 * (RPM, microstepping, speedmode, acceleration, deceleration) and enables 
 * the motor.
 */
void initialize_stepper_rotor() {
  stepper_rotor.begin(load_stepper_rotor_rpm(), load_stepper_rotor_ms());
  stepper_rotor.setEnableActiveState(LOW);
  stepper_rotor.disable();
  update_stepper_rotor_speed_profile();
}

/**
 * Get the current RPM value for the rotor stepper.
 * The value is stored and EEPROM and is a combination of two bytes (one and a half to be more precise).
 */
float load_stepper_rotor_rpm() {
  // get rpm and ms settings from eeprom
  uint8_t rpm_byte = EEPROM.read(EEPROM_STEPPER_ROTOR_RPM_POS);
  uint8_t rpm_ms_byte = EEPROM.read(EEPROM_STEPPER_ROTOR_RPM_MS_POS);

  return STEPPER_ROTOR_GEAR_RATIO * STEPPER_ROTOR_MIN_RPM + (STEPPER_ROTOR_RPM_STEP_SIZE * (((rpm_ms_byte & 0x0F) << 8) | rpm_byte));
}

/**
 * Get the microstepping settings for the rotor stepper.
 * The value is stored in the upper half of a byte in EEPROM which is used for RPM settings.
 */
uint8_t load_stepper_rotor_ms() {
  // get ms settings from eeprom
  /*uint8_t rpm_ms_byte = EEPROM.read(EEPROM_STEPPER_ROTOR_RPM_MS_POS);

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
 * Load the speed profile for the rotor motor from EEPROM.
 * The profile contains data about the speed type, acceleration and deceleration.
 */
void update_stepper_rotor_speed_profile() {
  // load EEPROM data
  uint8_t accel_byte = EEPROM.read(EEPROM_STEPPER_ROTOR_ACCEL_POS);
  uint8_t decel_byte = EEPROM.read(EEPROM_STEPPER_ROTOR_DECEL_POS);
  uint8_t accel_decel_byte = EEPROM.read(EEPROM_STEPPER_ROTOR_ACCEL_DECEL_POS);
  uint8_t bitmask = EEPROM.read(EEPROM_STEPPER_BITMASK_POS) & 0x0F;
  // create data for speed profile
  BasicStepperDriver::Mode speed_mode = (bitRead(bitmask, 0) == 1) ? stepper_rotor.LINEAR_SPEED : stepper_rotor.CONSTANT_SPEED;
  unsigned short accel = STEPPER_ROTOR_MIN_ACCEL + (STEPPER_ROTOR_ACCEL_STEP_SIZE * ((accel_decel_byte & 0x0F) << 8) | accel_byte);
  unsigned short decel = STEPPER_ROTOR_MIN_DECEL + (STEPPER_ROTOR_DECEL_STEP_SIZE * ((accel_decel_byte & 0xF0) << 4) | decel_byte);

  stepper_rotor.setSpeedProfile(speed_mode, accel, decel);
}

/**
 * This function will be executed as RTOS task if changes to the stepper configuration are applied.
 * It will periodically turn the stepper by 180 degrees into alternating direction.
 */
void stepper_rotor_debug_task(void*) {
  long rotate_degrees = 15;
  stepper_rotor.enable();
  while(true) {
    // start an asynchronous movement (triggering steps programatically is required)
    int8_t multiplier = (bitRead(EEPROM.read(EEPROM_STEPPER_BITMASK_POS), 1) == 0) ? 1 : -1;
    stepper_rotor.startRotate(multiplier*rotate_degrees * STEPPER_ROTOR_GEAR_RATIO);
    rotate_degrees *= -1;
    unsigned int wait_time = stepper_rotor.nextAction();
    while (stepper_rotor.getStepsRemaining() > 0) {
      // reset the watchdog manually since delayMicroseconds does not do it
      TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
      TIMERG0.wdt_feed=1;
      TIMERG0.wdt_wprotect=0;
      // wait till next action needs to be executed
      delayMicroseconds(wait_time);
      // execute next action
      wait_time = stepper_rotor.nextAction();
    }
    vTaskDelay(250);
  }
}
