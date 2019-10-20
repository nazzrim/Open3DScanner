/**
 * Since all values for a scan are 16 bit values a single generic
 * state can be used for adjusting all values.
 */

void menu_state_set_16bit_logic(
      const String affected_setting,
      const short min_value,
      const short max_value,
      const uint8_t store_address_a,
      const uint8_t store_address_b) {
  // process inputs
  // the encoder button has higher precedence than the encoder rotation
  if (encoder_button_pressed) {
    uint8_t lower_byte = menu_set_16bit_value & 0xFF;
    uint8_t upper_bits = (menu_set_16bit_value >> 8) & 0xFF;
    EEPROM.write(store_address_a, lower_byte);
    EEPROM.write(store_address_b, upper_bits);
    EEPROM.commit();
    consume_encoder_button();
    current_state = MENU;
  } else if ((int32_t)encoder.getCount() != 0) {
    menu_set_16bit_value += (short) (encoder.getCount() * scroll_speed_multiplier);
    // prevent over- and underflows
    if (menu_set_16bit_value > max_value || menu_set_16bit_value < min_value) {
      if ((int32_t)encoder.getCount() > 0) {
        menu_set_16bit_value = max_value;
      } else {
        menu_set_16bit_value = min_value;
      }
    }
    // reset encoder
    encoder.setCount(0);
    // adjust the multiplier for the next loop
    no_input_loops = 0;
    scroll_speed_multiplier = min(scroll_speed_multiplier * multplier_modifier, max_multiplier);
    // set the new value depending on the current applications state
    if(current_state == MENU_CHANGE_PHOTO_DELAY) {
      TIME_AFTER_PHOTO = menu_set_16bit_value;
    } else if(current_state == MENU_CHANGE_MOVE_DELAY) {
      TIME_AFTER_MOVE = menu_set_16bit_value;
    } 
  } else {
    if (no_input_loops++ >= multiplier_resetter) {
      scroll_speed_multiplier = min_multiplier;
    }
  }
  // draw screen
  lcd.clear(false);
  // construct and draw title line
  String title = "Scan Settings";
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
  String min_str = String(min_value);
  String max_str = String(max_value);
  String current_value = String(menu_set_16bit_value);
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
