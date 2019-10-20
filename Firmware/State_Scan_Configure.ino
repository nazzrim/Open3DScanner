/**
 * If the user wants to start a custom print, it is necessary to get his input for the various settings.
 */

/**
 * This object contains the custom scan the user wants to perform.
 */
Scan custom_scan;

/**
 * Loads the last known custom scan from EEPROM
 */
void initialize_custom_scan() {
  custom_scan.photos_per_revolution = (EEPROM.read(EEPROM_CUSTOM_SCAN_PHOTOS_PER_REVOLUTION_HIGH_POS) << 8) | EEPROM.read(EEPROM_CUSTOM_SCAN_PHOTOS_PER_REVOLUTION_LOW_POS);
  custom_scan.rotation = (EEPROM.read(EEPROM_CUSTOM_SCAN_ROTATION_HIGH_POS) << 8) | EEPROM.read(EEPROM_CUSTOM_SCAN_ROTATION_LOW_POS);
  custom_scan.rotation_divisions = (EEPROM.read(EEPROM_CUSTOM_SCAN_ROTATION_DIVISIONS_HIGH_POS) << 8) | EEPROM.read(EEPROM_CUSTOM_SCAN_ROTATION_DIVISIONS_LOW_POS);
}

/**
 * Logic for getting user input for all required scan settings and proceed to scanning.
 */
void scan_configure_state_logic() {
  query_photos_per_rev();
  query_rotation();
  query_rotation_steps();
  
  selected_scan = &custom_scan;
  current_state = SCAN_CONFIRM;

  // store the new custom print to eeprom
  EEPROM.write(EEPROM_CUSTOM_SCAN_PHOTOS_PER_REVOLUTION_LOW_POS, custom_scan.photos_per_revolution & 0xFF);
  EEPROM.write(EEPROM_CUSTOM_SCAN_PHOTOS_PER_REVOLUTION_HIGH_POS, (custom_scan.photos_per_revolution >> 8) & 0xFF);
  EEPROM.write(EEPROM_CUSTOM_SCAN_ROTATION_LOW_POS, custom_scan.rotation & 0xFF);
  EEPROM.write(EEPROM_CUSTOM_SCAN_ROTATION_HIGH_POS, (custom_scan.rotation >> 8) & 0xFF);
  EEPROM.write(EEPROM_CUSTOM_SCAN_ROTATION_DIVISIONS_LOW_POS, custom_scan.rotation_divisions & 0xFF);
  EEPROM.write(EEPROM_CUSTOM_SCAN_ROTATION_DIVISIONS_HIGH_POS, (custom_scan.rotation_divisions >> 8) & 0xFF);

  EEPROM.commit();
}

void query_photos_per_rev() {
  const char* lines[4];
  lines[0] = "How many photos";
  lines[1] = "should be taken";
  lines[2] = "during each tu-";
  lines[3] = "rntable rev?";
  custom_scan.photos_per_revolution = query_user(lines, 4, 1, custom_scan.photos_per_revolution, 3200);
}

void query_rotation() {
  const char* lines[4];
  lines[0] = "How many degrees";
  lines[1] = "should the rotor";
  lines[2] = "be turned in";
  lines[3] = "total?";
  custom_scan.rotation = query_user(lines, 4, 1, custom_scan.rotation, 359);
}

void query_rotation_steps() {
  short max_val = (short) (3200 * (custom_scan.rotation / 360.0));
  const char* lines[4];
  lines[0] = "Into how many";
  lines[1] = "steps should the";
  lines[2] = "turntable rota-";
  lines[3] = "tion divided?";
  custom_scan.rotation_divisions = query_user(lines, 4, 1, custom_scan.rotation_divisions, max_val);
}

short query_user(const char** lines, uint8_t num_lines, short min_val, short def_val, short max_val) {
  short result = def_val;
  while (true) {
    // handle user input
    if (encoder_button_pressed) {
      consume_encoder_button();
      // a value was selected so return it
      break;
    } else if ((int32_t)encoder.getCount() != 0) {
      result += (short) (encoder.getCount() * scroll_speed_multiplier);
      if (result < min_val || result > max_val) {
        if ((int32_t)encoder.getCount() > 0) {
          result = max_val;
        } else {
          result = min_val;
        }
      }
      encoder.setCount(0);
      // adjust the multiplier for the next loop
      no_input_loops = 0;
      scroll_speed_multiplier = min(scroll_speed_multiplier * multplier_modifier, max_multiplier);
    } else {
      if (no_input_loops++ >= multiplier_resetter) {
        scroll_speed_multiplier = min_multiplier;
      }
    }
    // draw to lcd
    lcd.clear();
    for (uint8_t i = 0; i < num_lines; i++) {
      lcd.println(lines[i]);
    }
    String min_str = "Min:" + String(min_val);
    String max_str = "Max:" + String(max_val);
    lcd.setCursor((42 - (min_str.length() * 5)) / 2, 4);
    lcd.print(min_str.c_str());
    lcd.setCursor(42 + ((42 - (max_str.length() * 5)) / 2), 4);
    lcd.print(max_str.c_str());
    lcd.setCursor((84 - (String(result).length() * 5)) / 2, 5);
    lcd.print(result);
    
    delay(TIME_BETWEEN_FRAMES);
  }
  return result;
}
