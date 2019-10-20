/**
 * This is the logic for a menu state that is used for different situation.
 * It allows to set a 12 bit value for short and float values.
 * Minimum, maximum and current value are displayed.
 * Since 12 bit are 1.5 bytes the user can define if the upper or lower half of the second byte will be used.
 */

// The actual definition can be found in Menu.ino
extern MenuSystem menu_system;

// Encoder initialization can be found in Encoder.ino
extern ESP32Encoder encoder;

// Encoder button initialization can be found in Encoder.ino
extern volatile boolean encoder_button_pressed;

/**
 * Logic function for this state handles user input to navigate the menu which is displayed on the lcd.
 * In theory all 12 bit values with type float or short are supported. If a new value should be made
 * changeable by this function some code changes are required since the application of the new value
 * after a change is hard coded right now.
 * 
 * @param affected_element Name of the element which uses the affected setting
 * @param affacted_setting Name of the actual setting which is adjusted now
 * @param use_float_values True if the altered setting is a float value, false if it is a short value
 * @param min_short Minimal value if short values are used
 * @param step_short Minimal difference between to values if short values are used
 * @param min_float Minimal value if float values are used
 * @param step_float Minimal difference between to values if short values are used
 * @param store_address_a EEPROM address where bits 0 to 7 are stored
 * @param store_address_b EEPROM address where bits 8 to 11 are stored
 * @param use_upper bits True if bits 8 to 11 should be stored in bits 4 to 7 of store_address_b, false for target bits 0 to 3
 */
void menu_state_set_min_max_12bit_logic(
      const String affected_element,
      const String affected_setting,
      const bool use_float_values,
      const short min_short,
      const short step_short,
      const float min_float,
      const float step_float,
      const uint8_t store_address_a,
      const uint8_t store_address_b,
      const bool use_upper_bits) {
  // process inputs
  // the encoder button has higher precedence than the encoder rotation
  if (encoder_button_pressed) {
    uint8_t lower_byte = menu_set_min_max_12bit_value & 0xFF;
    uint8_t upper_bits = (menu_set_min_max_12bit_value >> 8) & 0x0F;
    if (use_upper_bits) {
      upper_bits = (upper_bits << 4) | (EEPROM.read(store_address_b) & 0x0F);
    } else {
      upper_bits = (upper_bits) | (EEPROM.read(store_address_b) & 0xF0);
    }
    EEPROM.write(store_address_a, lower_byte);
    EEPROM.write(store_address_b, upper_bits);
    EEPROM.commit();
    consume_encoder_button();
    current_state = MENU;
  } else if ((int32_t)encoder.getCount() != 0) {
    menu_set_min_max_12bit_value += (short) (encoder.getCount() * scroll_speed_multiplier);
    // prevent over- and underflows
    if (menu_set_min_max_12bit_value > 4095) {
      if ((int32_t)encoder.getCount() > 0) {
        menu_set_min_max_12bit_value = 4095;
      } else {
        menu_set_min_max_12bit_value = 0;
      }
    }
    // reset encoder
    encoder.setCount(0);
    // adjust the multiplier for the next loop
    no_input_loops = 0;
    scroll_speed_multiplier = min(scroll_speed_multiplier * multplier_modifier, max_multiplier);
    // set the new value depending on the current applications state
    if(current_state == MENU_CHANGE_RPM_ROTOR) {
      stepper_rotor.setRPM(STEPPER_ROTOR_GEAR_RATIO * min_float + (menu_set_min_max_12bit_value * step_float));
    } else if(current_state == MENU_CHANGE_ACCELERATION_ROTOR) {
      stepper_rotor.setSpeedProfile(stepper_rotor.getSpeedProfile().mode, min_short + (menu_set_min_max_12bit_value * step_short), stepper_rotor.getDeceleration());
    } else if(current_state == MENU_CHANGE_DECELERATION_ROTOR) {
      stepper_rotor.setSpeedProfile(stepper_rotor.getSpeedProfile().mode, stepper_rotor.getAcceleration(), min_short + (menu_set_min_max_12bit_value * step_short));
    } else if(current_state == MENU_CHANGE_RPM_TURNTABLE) {
      stepper_turntable.setRPM(min_float + (menu_set_min_max_12bit_value * step_float));
    } else if(current_state == MENU_CHANGE_ACCELERATION_TURNTABLE) {
      stepper_turntable.setSpeedProfile(stepper_turntable.getSpeedProfile().mode, min_short + (menu_set_min_max_12bit_value * step_short), stepper_turntable.getDeceleration());
    } else if(current_state == MENU_CHANGE_DECELERATION_TURNTABLE) {
      stepper_turntable.setSpeedProfile(stepper_turntable.getSpeedProfile().mode, stepper_turntable.getAcceleration(), min_short + (menu_set_min_max_12bit_value * step_short));
    }
  } else {
    if (no_input_loops++ >= multiplier_resetter) {
      scroll_speed_multiplier = min_multiplier;
    }
  }
  // draw screen
  lcd.clear(false);
  // construct and draw title line
  String title = "Change " + affected_element;
  if (title.length() <= 16) {
    lcd.setCursor((84 - (title.length() * 5)) / 2, 0);
  }
  lcd.println(title.c_str());
  // constuct and draw second title line
  if (affected_setting.length() <= 16) {
    lcd.setCursor((84 - (affected_setting.length() * 5)) / 2, 1);
  }
  lcd.println(affected_setting.c_str());
  // draw min/max "title" - x-offset= (halfscreen (4px mid separation) - 4 characters) / 2 --> (40 - 20) / 2
  // for second entry 44 needs to be added
  lcd.setCursor(10, 2);
  lcd.print("Min:");
  lcd.setCursor(54, 2);
  lcd.print("Max:");
  // draw min and max values
  String min_str;
  String max_str;
  String current_value;
  if (use_float_values) {
    min_str = String(min_float, 2);
    max_str = String(min_float + (4095 * step_float), 2);
    current_value = String(min_float + (menu_set_min_max_12bit_value * step_float), 2);
  } else {
    min_str = String(min_short);
    max_str = String(min_short + (4095 * step_short));
    current_value = String(min_short + (menu_set_min_max_12bit_value * step_short));
  }
  lcd.setCursor((40 - (min_str.length() * 5)) / 2, 3);
  lcd.print(min_str.c_str());
  lcd.setCursor(44 + (40 - (max_str.length() * 5)) / 2, 3);
  lcd.print(max_str.c_str());
  // draw current value title and value itself
  lcd.setCursor(22, 4);
  lcd.print("Current:");
  lcd.setCursor((84 - (current_value.length() * 5)) / 2, 5);
  lcd.print(current_value.c_str());
  
  // sleep some time for (better) visibility on screen
  delay(TIME_BETWEEN_FRAMES);
}
