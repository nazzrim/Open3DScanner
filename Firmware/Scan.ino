/**
 * This file contains the whole logic to perform a 3D scan with given parameters including LCD and status LED handling.
 */

// Can be found in Stepper-Rotor.ino
extern A4988 stepper_rotor;
extern float STEPPER_ROTOR_GEAR_RATIO;

// Can be found in Stepper-Turntable.ino
extern A4988 stepper_turntable;

// After moving the rotor/turntable wait this amount of time (in ms)
short TIME_AFTER_MOVE = 100;

// After taking a photo is triggered wait for this amount of time (in ms)
short TIME_AFTER_PHOTO = 5000;

// After the connection to a bluetooth device was reestablished wait for this much ms to allow the user to open its camera app
short TIME_AFTER_RECONNECT = 10000;

// After a scan is finished successfully notify the user with blinking and an lcd message
short TIME_AFTER_SCAN = 20000;


// These default settings are used as starting values if a custom scan is started
Scan default_scan{32, 175, 33};

// This array contains the defined scan presets which can be selected from the menu.
ScanPreset scan_presets[NUMBER_OF_SCAN_PRESETS];

// This is the scan which was selected from presets or defined by the user and which will be performed
Scan* selected_scan;

/**
 * Indicator if yes or no was selected
 */
bool yes_selected = false;

// Bitmap to display on the lcd. It one line on the screen and shows YES and NO
// OK is selected
const unsigned char YES_SELECTED[84] PROGMEM = {
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xFB, 0x87,
0xFB, 0xFD, 0xFF, 0x81, 0xB5, 0xB5, 0xBD, 0xBD, 0xFF, 0xDB, 0xB5, 0xB5, 0xB5, 0xCD, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x04, 0x08, 0x10, 0x7E, 0x00,
0x3C, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 
};

// Bitmap to display on the lcd. It one line on the screen and shows YES and NO
// CANCEL is selected
const unsigned char NO_SELECTED[84] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x78,
0x04, 0x02, 0x00, 0x7E, 0x4A, 0x4A, 0x42, 0x42, 0x00, 0x24, 0x4A, 0x4A, 0x4A, 0x32, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x81, 0xFB, 0xF7, 0xEF, 0x81, 0xFF,
0xC3, 0xBD, 0xBD, 0xBD, 0xC3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 
};

/**
 * This function will create all presets which will be available within the menu
 */
void initialize_scan() {
  scan_presets[0] = ScanPreset{"Low Detail", Scan{16, 90, 3}};
  scan_presets[1] = ScanPreset{"Medium Detail", Scan{32, 120, 6}};
  scan_presets[2] = ScanPreset{"High Detail", Scan{64, 160, 10}};
  scan_presets[3] = ScanPreset{"Ultra Detail", Scan{128, 175, 18}};

  TIME_AFTER_PHOTO = (EEPROM.read(EEPROM_TIME_BETWEEN_PHOTO_HIGH_POS) << 8) | EEPROM.read(EEPROM_TIME_BETWEEN_PHOTO_LOW_POS);
  TIME_AFTER_MOVE = (EEPROM.read(EEPROM_TIME_AFTER_MOVE_HIGH_POS) << 8) | EEPROM.read(EEPROM_TIME_AFTER_MOVE_LOW_POS);
}

/**
 * Utility function that allows to delay the execution for a given amount of time and show an optionally abort-request screen on encoder button presses.
 * Returns true if the scan should be aborted, false otherwise.
 */
bool wait_for_time(short time_to_wait, boolean abort_screen = true) {
  while(time_to_wait > 0) {
    // if the user presses a button we skip the finish message
    if (encoder_button_pressed) {
      consume_encoder_button();
      if (abort_screen) {
        while (true) {
          // process inputs
          if (encoder_button_pressed) {
            consume_encoder_button();
            current_state = MENU;
            // reset the yes_selected status for next time the menu is opened
            yes_selected = false;
            return true;
          } else if ((int32_t)encoder.getCount() != 0) {
            yes_selected = (int32_t)encoder.getCount() < 0;
            encoder.setCount(0);
          }
          // draw to screen
          lcd.clear(false);
          lcd.println("Do you really");
          lcd.println("want to abort");
          lcd.println("the current");
          lcd.println("scan?");
          lcd.setCursor(0, 5);
          if (yes_selected) {
            lcd.draw(YES_SELECTED, 84, true);
          } else {
            lcd.draw(NO_SELECTED, 84, true);
          }
          
          // sleep some time for (better) visibility on screen
          delay(TIME_BETWEEN_FRAMES);
        }
      } else {
        break;
      }
    }
    if (time_to_wait > TIME_BETWEEN_FRAMES) {
      delay(TIME_BETWEEN_FRAMES);
      time_to_wait -= TIME_BETWEEN_FRAMES;
    } else {
      delay(time_to_wait);
      time_to_wait = 0;
    }
  }
  return false;
}

/**
 * Performs the given scan
 */
void perform_scan() {
  // ask the user if he wants to use the lights during this scan
  
  while (true) {
    // process inputs
    if (encoder_button_pressed) {
      consume_encoder_button();
      // reset the yes_selected status for next time the menu is opened
      yes_selected = false;
      break;
    } else if ((int32_t)encoder.getCount() != 0) {
      yes_selected = (int32_t)encoder.getCount() < 0;
      encoder.setCount(0);
      // turn light on/off
      if (yes_selected) {
        turn_light_on();
      } else {
        turn_light_off();
      }
    }
    // draw to screen
    lcd.clear(false);
    lcd.println("Do you want to");
    lcd.println("turn on the");
    lcd.println("lights for this");
    lcd.println("scan?");
    lcd.setCursor(0, 5);
    if (yes_selected) {
      lcd.draw(YES_SELECTED, 84, true);
    } else {
      lcd.draw(NO_SELECTED, 84, true);
    }
    
    // sleep some time for (better) visibility on screen
    delay(TIME_BETWEEN_FRAMES);
  }
  // ensure camera is connected and positioned
  while (true) {
    if (encoder_button_pressed) {
      consume_encoder_button();
      break;
    }
    lcd.clear(false);
    if (!hid_connected) {
      lcd.println("Please connect");
      lcd.println("and position the");
      lcd.println("camara. Home the");
      lcd.println("rotor and press");
      lcd.println("the button to");
      lcd.println("start scanning.");
    } else {
      lcd.println("Please position");
      lcd.println("the camera.");
      lcd.println("Home the rotor");
      lcd.println("and press the");
      lcd.println("button to start");
      lcd.println("scanning.");
    }
    delay(TIME_BETWEEN_FRAMES);
  }
  // enable motors
  stepper_rotor.enable();
  stepper_turntable.enable();
  // set status led to yellow to indicate operating mode
  status_led_yellow();
  // calculate total number of photos taken during the scan
  uint32_t total_photos = selected_scan->photos_per_revolution * selected_scan->rotation_divisions;
  uint32_t current_photo = 1;
  // retrieve the total time required for this print
  uint64_t etr = time_for_scan(*selected_scan);
  // for each position of the rotor perform a full cycle and take photos
  for(uint16_t rot_pos = 0; rot_pos < selected_scan->rotation_divisions; rot_pos++) {
    for(uint16_t table_pos = 0; table_pos < selected_scan->photos_per_revolution; table_pos++) {
      if (take_next_photo(current_photo++, total_photos, etr)) {
        current_state = MENU;
        return;
      }
      etr -= time_for_photo();
      // we can ommit the final rotation
      if (table_pos < selected_scan->photos_per_revolution - 1) {
        if (turntable_to_next_position(*selected_scan)) {
          current_state = MENU;
          return;
        }
        etr -= time_for_turntable_move(*selected_scan);
      }
    }
    // we can ommit the final rotation
    if (rot_pos < selected_scan->rotation_divisions - 1) {
      if (rotor_to_next_position(*selected_scan)) {
        current_state = MENU;
        return;
      }
      etr -= time_for_rotor_move(*selected_scan);
    }
  }
  // move rotor back to starting position
  rotor_to_start_position(*selected_scan);
  // scan finished
  start_blinking(EXTRA_SLOW, GREEN);
  lcd.clear(false);
  lcd.setCursor(7, 0);
  lcd.println("Scan finished.");
  wait_for_time(TIME_AFTER_SCAN, false);
  stop_blinking();
  // disable motors
  stepper_rotor.disable();
  stepper_turntable.disable();
  // turn off the lights (if they were on)
  turn_light_off();
  // after sucessfully finishing a scan turn the status led green again
  status_led_green();
  // remove selected scan
  selected_scan = NULL;
  // return to the menu
  current_state = MENU;
}

/**
 * Calculates the estimated time that is needed for a given scan.
 */
uint64_t time_for_scan(struct Scan scan) {
  return ((scan.rotation_divisions * scan.photos_per_revolution) * (time_for_turntable_move(scan) + time_for_photo()))  // time per revolution
         - (scan.rotation_divisions * time_for_turntable_move(scan))                                                    // this is the final rotation of the turntable we ommit
         + ((scan.rotation_divisions - 1) * time_for_rotor_move(scan))                                                  // time for rotor movements (take care of the last ommited move)
         + time_for_rotor_to_start_move(scan);                                                                          // time to move the rotor back to the starting position
}

/**
 * Calculates the time in ms that is needed for each turntable move (including delays).
 */
uint32_t time_for_turntable_move(struct Scan scan) {
  return TIME_AFTER_MOVE + (stepper_turntable.getTimeForMove(stepper_turntable.calcStepsForRotation(360 / ((double) scan.photos_per_revolution))) / 1000);
}

/**
 * Calculates the time in ms that is needed for each rotor move (including delays).
 */
uint32_t time_for_rotor_move(struct Scan scan) {
  return TIME_AFTER_MOVE + (stepper_rotor.getTimeForMove(stepper_rotor.calcStepsForRotation(STEPPER_ROTOR_GEAR_RATIO * (scan.rotation / ((double) scan.rotation_divisions)))) / 1000);
}

uint32_t time_for_rotor_to_start_move(struct Scan scan) {
  return TIME_AFTER_MOVE + (stepper_rotor.getTimeForMove(stepper_rotor.calcStepsForRotation(STEPPER_ROTOR_GEAR_RATIO * (double) scan.rotation)) / 1000);
}

/**
 * Calculates the time in ms that is needed for taking each photo (including delays).
 */
uint32_t time_for_photo() {
  return TIME_AFTER_PHOTO;
}

/**
 * Moves the turntable to the next position for a given print
 */
bool turntable_to_next_position(struct Scan scan) {
  int8_t multiplier = (bitRead(EEPROM.read(EEPROM_STEPPER_BITMASK_POS), 5) == 0) ? 1 : -1;
  stepper_turntable.rotate(multiplier * 360 / ((double) scan.photos_per_revolution));
  return wait_for_time(TIME_AFTER_MOVE);
}


/**
 * Moves the rotor to the next position for a given print
 */
bool rotor_to_next_position(struct Scan scan) {
  int8_t multiplier = (bitRead(EEPROM.read(EEPROM_STEPPER_BITMASK_POS), 1) == 0) ? 1 : -1;
  stepper_rotor.rotate((STEPPER_ROTOR_GEAR_RATIO * multiplier * scan.rotation) / ((double) scan.rotation_divisions));
  return wait_for_time(TIME_AFTER_MOVE);
}

/**
 * Moves the rotor to the start position for a given print
 */
bool rotor_to_start_position(struct Scan scan) {
  int8_t multiplier = (bitRead(EEPROM.read(EEPROM_STEPPER_BITMASK_POS), 1) == 0) ? 1 : -1;
  stepper_rotor.rotate(STEPPER_ROTOR_GEAR_RATIO * multiplier * scan.rotation * -1);
  return wait_for_time(TIME_AFTER_MOVE);
}

String retrieve_time_string_for_ms(uint64_t etr) {
  unsigned long etr_seconds = etr / 1000;
  String etr_seconds_str = String(etr_seconds % 60);
  if (etr_seconds_str.length() == 1) {
    etr_seconds_str = "0" + etr_seconds_str;
  }
  String etr_minutes_str = String((etr_seconds / 60) % 60);
  if (etr_minutes_str.length() == 1) {
    etr_minutes_str = "0" + etr_minutes_str;
  }
  String etr_hours_str = String(etr_seconds / 3600);
  return "ETR: " + etr_hours_str + ":" + etr_minutes_str + ":" + etr_seconds_str;
}

/**
 * Take a photo during a print and ensure that the bluetooth camera is connected
 */
bool take_next_photo(uint32_t current_photo, uint32_t total_photos, uint64_t etr) {
  // check if the camera connection was lost
  if (!hid_connected) {
    // promt user to reestablish connection
    lcd.clear(false);
    lcd.println("Please reconnect");
    lcd.println("the camera via");
    lcd.println("Bluetooth!");
    // fast red blinking of status led
    start_blinking(FAST, RED);
    while(!hid_connected) {
      if (wait_for_time(TIME_BETWEEN_FRAMES)) {
        return true;
      }
    }
    stop_blinking();
    // slow yellow blinking of status led till print continues
    start_blinking(SLOW, YELLOW);
    // wait for some time to allow the user to set everything up again (e.g. starting camera app)
    lcd.clear(false);
    lcd.println("Successfully re-");
    lcd.println("connected to");
    lcd.println("camera.");

    lcd.println("Continue scann-");
    lcd.println("ing in:");
    short time_till_continue = TIME_AFTER_RECONNECT;
    while(time_till_continue > 0) {
      lcd.println("                "); // little trick to clear the line
      String time_remaining = String(time_till_continue / 1000.0, 2);
      lcd.setCursor((84 - ((2 + time_remaining.length()) * 5)) / 2, 5);
      lcd.print(time_remaining.c_str());
      lcd.print(" s");
      if (time_till_continue > TIME_BETWEEN_FRAMES) {
        if (wait_for_time(TIME_BETWEEN_FRAMES)) {
          return true;
        }
        time_till_continue -= TIME_BETWEEN_FRAMES;
      } else {
        if (wait_for_time(time_till_continue)) {
          return true;
        }
        time_till_continue = 0;
      }
    }
    stop_blinking();
    // turn led into "operating" mode again (yellow)
    status_led_yellow();
  }
  String cur = String(current_photo);
  String total = String(total_photos);
  String progress = String(((float) current_photo) / total_photos, 2) + " %";
  // create better readable values from etr time
  String etr_str = retrieve_time_string_for_ms(etr);
  lcd.clear(false);
  lcd.println("Taking photo:");
  lcd.setCursor((84 - (cur.length() * 5)) / 2, 1);
  lcd.print(cur.c_str());
  lcd.setCursor(37, 2);
  lcd.print("of");
  lcd.setCursor((84 - (total.length() * 5)) / 2, 3);
  lcd.print(total.c_str());
  lcd.setCursor((84 - (progress.length() * 5)) / 2, 4);
  lcd.print(progress.c_str());
  lcd.setCursor((84 - (etr_str.length() * 5)) / 2, 4);
  lcd.print(etr_str.c_str());
  ble_take_photo();
  if (wait_for_time(TIME_AFTER_PHOTO)) {
    return true;
  }
  return false;
}
