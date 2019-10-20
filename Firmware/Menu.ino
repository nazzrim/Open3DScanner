#include <MenuSystem.h>
#include <StackArray.h>

/**
 * The menu is used for navigation in the applications settings, functionalities and debug options.
 * It will be displayed on the display while userinput comes from the encoder.
 */

// Actual lcd initialization can be found in Nokia5110-Display.ino
extern Nokia_LCD lcd;

// Can be found in Stepper-Rotor.ino
extern A4988 stepper_rotor;
extern void stepper_rotor_debug_task(void*);

// Can be found in Stepper-Turntable.ino
extern A4988 stepper_turntable;
extern void stepper_turntable_debug_task(void*);

// Can be found in Scan.ino
extern ScanPreset scan_presets[];
extern Scan* selected_scan;

/**
 * While we navigate the menu we use/store this data to determine
 * which items to render and if a scrollbar is needed.
 */
struct MenuNavigationData {
  uint8_t min_display;
  uint8_t max_display;
  uint8_t item_count;
  uint8_t current_position;
};

/**
 * Each level in the menu hirachy gets a new entry on top of the stack.
 * The current navigated element can be found on top of the stack.
 */
StackArray<MenuNavigationData> menu_navigation_data_stack;

/**
 * This class is required to render the menu structure (see below) to the LCD.
 */
class MenuRenderer : public MenuComponentRenderer {
public:
    void render(Menu const& menu) const {
      lcd.clear(false);
      String menu_name = String("# ") + menu.get_name() + " #";
      lcd.println(menu_name.c_str());
      // get the current navigation data to determine which items needs to be rendered
      MenuNavigationData current = menu_navigation_data_stack.peek();
      for (int i = current.min_display; i < current.max_display + 1; ++i) {
          MenuComponent const* current_menu_component = menu.get_menu_component(i);
          if (current_menu_component->is_current()) {
            lcd.print("> ");
          } else {
            lcd.print("  ");
          }
          current_menu_component->render(*this);
      }
      // check if we need a scrollbar
      if (current.item_count > 5) {
        // the maximal number verticl of pixels which is used for the scrolling bar (display has 48 in total)
        uint8_t available_pixels = 40;
        // this is the percentage of items displayed
        float shown_quantity = 5.0f / current.item_count;
        // calculate the number of pixels which will form the scroll bar
        uint8_t scrollbar_pixels = (int) (available_pixels * shown_quantity);
        // determine where the scrollbar starts
        uint8_t scrollbar_start = (int) available_pixels * ((float) current.min_display / current.item_count);
        // ensure that the scrollbar does not exceed the number of available y-pixels
        if ((scrollbar_start + scrollbar_pixels) > available_pixels) {
          scrollbar_pixels = available_pixels - scrollbar_start;
        }
        // get starting line
        // since we do not want a scrollbar on the menu title line we increase the result by one
        uint8_t starting_line = 1 + scrollbar_start / 8;
        lcd.setCursor(82, starting_line);
        switch (scrollbar_start % 8) {
          case 0:
            // we need to draw a full line
            lcd.sendData(0xFF);
            lcd.sendData(0xFF);
            break;
          case 1:
            lcd.sendData(0xFE);
            lcd.sendData(0xFE);
            break;
          case 2:
            lcd.sendData(0xFC);
            lcd.sendData(0xFC);
            break;
          case 3:
            lcd.sendData(0xF8);
            lcd.sendData(0xF8);
            break;
          case 4:
            lcd.sendData(0xF0);
            lcd.sendData(0xF0);
            break;
          case 5:
            lcd.sendData(0xE0);
            lcd.sendData(0xE0);
            break;
          case 6:
            lcd.sendData(0xC0);
            lcd.sendData(0xC0);
            break;
          case 7:
            // we need to draw a single pixel
            lcd.sendData(0x80);
            lcd.sendData(0x80);
            break;
        }        
        uint8_t remaining_pixels = scrollbar_pixels - (8 - (scrollbar_start % 8));
        // draw the full line part of the scrollbar
        uint8_t full_lines = remaining_pixels / 8;
        for (uint8_t i = 1; i <= remaining_pixels / 8; i++) {
          lcd.setCursor(82, i + starting_line);
          lcd.sendData(0xFF);
          lcd.sendData(0xFF);
        }
        remaining_pixels -= full_lines * 8;
        // draw the remaining pixels of the scrollbar
        lcd.setCursor(82, 1 + starting_line + (full_lines));
        switch (remaining_pixels % 8) {
          case 0:
            // we do not need to draw pixels at all
            break;
          case 1:
            lcd.sendData(0x01);
            lcd.sendData(0x01);
            break;
          case 2:
            lcd.sendData(0x03);
            lcd.sendData(0x03);
            break;
          case 3:
            lcd.sendData(0x07);
            lcd.sendData(0x07);
            break;
          case 4:
            lcd.sendData(0x0F);
            lcd.sendData(0x0F);
            break;
          case 5:
            lcd.sendData(0x1F);
            lcd.sendData(0x1F);
            break;
          case 6:
            lcd.sendData(0x3F);
            lcd.sendData(0x3F);
            break;
          case 7:
            // we need to draw seven pixels
            lcd.sendData(0x7F);
            lcd.sendData(0x7F);
            break;
          case 8:
            lcd.sendData(0xFF);
            lcd.sendData(0xFF);
            break;
        }
      }
    }

    void render_menu_item(MenuItem const& menu_item) const {
        lcd.println(menu_item.get_name());
    }

    void render_back_menu_item(BackMenuItem const& menu_item) const {
        lcd.println(menu_item.get_name());
    }

    void render_numeric_menu_item(NumericMenuItem const& menu_item) const {
        lcd.println(menu_item.get_name());
    }

    void render_menu(Menu const& menu) const {
        lcd.println(menu.get_name());
    }
};

// We use this instance to render our menu system
MenuRenderer menu_renderer;

/**
 * Callback functions for menu items
 */

/**
 * If the current menu level is exited the corresponding data is removed from the stack.
 */
void on_back_menu_item(MenuComponent* p_menu_component) {
  menu_navigation_data_stack.pop();
}

/**
 * Ensure that the light is of while navigating the menu.
 */
void on_back_menu_item_light_off(MenuComponent* p_menu_component) {
  turn_light_off();
  on_back_menu_item(p_menu_component);
}

/**
 * Stops an eventually runnin task and then calls normal on_menu_back function.
 */
void on_back_menu_item_stop_task(MenuComponent* p_menu_component) {
  if (rtos_task_handle != NULL) {
    vTaskSuspend(rtos_task_handle);
    vTaskDelete(rtos_task_handle);
    rtos_task_handle = NULL;
  }
  on_back_menu_item(p_menu_component);
}

/**
 * Stops the rotor stepper and then calls on_menu_back function_stop_task.
 */
void on_back_menu_item_stop_task_stop_rotor(MenuComponent* p_menu_component) {
  stepper_rotor.stop();
  stepper_rotor.disable();
  on_back_menu_item_stop_task(p_menu_component);
}

/**
 * Stops the turntable stepper and then calls on_menu_back function_stop_task.
 */
void on_back_menu_item_stop_task_stop_turntable(MenuComponent* p_menu_component) {
  stepper_turntable.stop();
  stepper_turntable.disable();
  on_back_menu_item_stop_task(p_menu_component);
}

/**
 * Reads current display backlight status from EEPROM, inverts it and saves it
 * back to EEPROM. Also changes the state of the corresponding GPIO pin to match
 * the new state.
 */
void on_menu_item_display_switch_light_selected(MenuComponent* p_menu_component) {
  uint8_t bl_constrast_setting = EEPROM.read(EEPROM_LCD_BL_CONTRAST_POS);
  if ((bl_constrast_setting & 0x80) > 0) {
    digitalWrite(DISPLAY_BL, LOW);
  } else {
    digitalWrite(DISPLAY_BL, HIGH);
  }

  EEPROM.write(EEPROM_LCD_BL_CONTRAST_POS, bl_constrast_setting ^ 0x80);
  EEPROM.commit();
}

/**
 * Enters MENU_CHANGE_CONTRAST state which allows to adjust the contrast of the lcd
 * screen. Statehandling is placed in main sketch file (ESP32-3D_Scanner.ino).
 */
void on_menu_item_display_change_contrast_selected(MenuComponent* p_menu_component) {
  current_state = MENU_CHANGE_CONTRAST;

  uint8_t bl_constrast_setting = EEPROM.read(EEPROM_LCD_BL_CONTRAST_POS);
  menu_change_contrast_current_value = bl_constrast_setting & 0x7F;
  menu_change_contrast_backlight_state = bl_constrast_setting & 0x80;
}

/**
 * Triggers the BLE-keyboard to send the command required to take a
 * photo to the connected device and consume the button press on the
 * encoder.
 */
void on_menu_item_debug_take_photo(MenuComponent* p_menu_component) {
  ble_take_photo();
  consume_encoder_button();
}

void on_menu_item_debug_switch_light(MenuComponent* p_menu_component) {
  switch_light();
  consume_encoder_button();
}

/**
 * Sets the current camera model to an be an android device.
 */
void on_menu_item_camera_type_android(MenuComponent* p_menu_component) {
  EEPROM.write(EEPROM_CAMERA_TYPE_POS, EEPROM_CAMERA_TYPE_ANDROID_DEVICE);
  EEPROM.commit();
}

/**
 * Sets the current camera model to be an iOS device.
 */
void on_menu_item_camera_type_iOS(MenuComponent* p_menu_component) {
  EEPROM.write(EEPROM_CAMERA_TYPE_POS, EEPROM_CAMERA_TYPE_IOS_DEVICE);
  EEPROM.commit();
}

/**
 * On entering the menu for rotor settings a task for moving the stepper is started.
 */
void on_menu_rotor(MenuComponent* p_menu_component) {
  xTaskCreate(stepper_rotor_debug_task, "RotorMovement", 1000, NULL, 5, &rtos_task_handle);
}

/**
 * On entering the menu for turntable settings a task for moving the stepper is started.
 */
void on_menu_turntable(MenuComponent* p_menu_component) {
  xTaskCreate(stepper_turntable_debug_task, "TurntableMovement", 1000, NULL, 5, &rtos_task_handle);
}


/**
 * Set microstepping of rotor stepper to full steps.
 */
void on_menu_item_steppers_rotor_microstepping_full_step(MenuComponent* p_menu_component) {
  EEPROM.write(EEPROM_STEPPER_ROTOR_RPM_MS_POS, 
      (EEPROM.read(EEPROM_STEPPER_ROTOR_RPM_MS_POS) & 0x0F) | EEPROM_STEPPER_FULL_STEP << 4
  );
  EEPROM.commit();
  stepper_rotor.setMicrostep(1);
}

/**
 * Set microstepping of turntable stepper to full steps.
 */
void on_menu_item_steppers_turntable_microstepping_full_step(MenuComponent* p_menu_component) {
  EEPROM.write(EEPROM_STEPPER_TURNTABLE_RPM_MS_POS, 
      (EEPROM.read(EEPROM_STEPPER_TURNTABLE_RPM_MS_POS) & 0x0F) | EEPROM_STEPPER_FULL_STEP << 4
  );
  EEPROM.commit();
  stepper_turntable.setMicrostep(1);
}

/**
 * Set microstepping of rotor stepper to half steps.
 */
void on_menu_item_steppers_rotor_microstepping_half_step(MenuComponent* p_menu_component) {
  EEPROM.write(EEPROM_STEPPER_ROTOR_RPM_MS_POS, 
      (EEPROM.read(EEPROM_STEPPER_ROTOR_RPM_MS_POS) & 0x0F) | EEPROM_STEPPER_HALF_STEP << 4
  );
  EEPROM.commit();
  stepper_rotor.setMicrostep(2);
}

/**
 * Set microstepping of turntable stepper to half steps.
 */
void on_menu_item_steppers_turntable_microstepping_half_step(MenuComponent* p_menu_component) {
  EEPROM.write(EEPROM_STEPPER_TURNTABLE_RPM_MS_POS, 
      (EEPROM.read(EEPROM_STEPPER_TURNTABLE_RPM_MS_POS) & 0x0F) | EEPROM_STEPPER_HALF_STEP << 4
  );
  EEPROM.commit();
  stepper_turntable.setMicrostep(2);
}

/**
 * Set microstepping of rotor stepper to quarter steps.
 */
void on_menu_item_steppers_rotor_microstepping_quarter_step(MenuComponent* p_menu_component) {
  EEPROM.write(EEPROM_STEPPER_ROTOR_RPM_MS_POS, 
      (EEPROM.read(EEPROM_STEPPER_ROTOR_RPM_MS_POS) & 0x0F) | EEPROM_STEPPER_QUARTER_STEP << 4
  );
  EEPROM.commit();
  stepper_rotor.setMicrostep(4);
}

/**
 * Set microstepping of turntable stepper to quarter steps.
 */
void on_menu_item_steppers_turntable_microstepping_quarter_step(MenuComponent* p_menu_component) {
  EEPROM.write(EEPROM_STEPPER_TURNTABLE_RPM_MS_POS, 
      (EEPROM.read(EEPROM_STEPPER_TURNTABLE_RPM_MS_POS) & 0x0F) | EEPROM_STEPPER_QUARTER_STEP << 4
  );
  EEPROM.commit();
  stepper_turntable.setMicrostep(4);
}

/**
 * Set microstepping of rotor stepper to eighth steps.
 */
void on_menu_item_steppers_rotor_microstepping_eighth_step(MenuComponent* p_menu_component) {
  EEPROM.write(EEPROM_STEPPER_ROTOR_RPM_MS_POS, 
      (EEPROM.read(EEPROM_STEPPER_ROTOR_RPM_MS_POS) & 0x0F) | EEPROM_STEPPER_EIGHTH_STEP << 4
  );
  EEPROM.commit();
  stepper_rotor.setMicrostep(8);
}

/**
 * Set microstepping of turntable stepper to eighth steps.
 */
void on_menu_item_steppers_turntable_microstepping_eighth_step(MenuComponent* p_menu_component) {
  EEPROM.write(EEPROM_STEPPER_TURNTABLE_RPM_MS_POS, 
      (EEPROM.read(EEPROM_STEPPER_TURNTABLE_RPM_MS_POS) & 0x0F) | EEPROM_STEPPER_EIGHTH_STEP << 4
  );
  EEPROM.commit();
  stepper_turntable.setMicrostep(8);
}

/**
 * Set microstepping of rotor stepper to sixteenth steps.
 */
void on_menu_item_steppers_rotor_microstepping_sixteenth_step(MenuComponent* p_menu_component) {
  EEPROM.write(EEPROM_STEPPER_ROTOR_RPM_MS_POS, 
      (EEPROM.read(EEPROM_STEPPER_ROTOR_RPM_MS_POS) & 0x0F) | EEPROM_STEPPER_SIXTEENTH_STEP << 4
  );
  EEPROM.commit();
  stepper_rotor.setMicrostep(16);
}

/**
 * Set microstepping of turntable stepper to sixteenth steps.
 */
void on_menu_item_steppers_turntable_microstepping_sixteenth_step(MenuComponent* p_menu_component) {
  EEPROM.write(EEPROM_STEPPER_TURNTABLE_RPM_MS_POS, 
      (EEPROM.read(EEPROM_STEPPER_TURNTABLE_RPM_MS_POS) & 0x0F) | EEPROM_STEPPER_SIXTEENTH_STEP << 4
  );
  EEPROM.commit();
  stepper_turntable.setMicrostep(16);
}

/**
 * Enter state which allows the adjustment of rotor stepper's RPM and read the current value from EEPROM.
 */
void on_menu_item_steppers_rotor_rpm(MenuComponent* p_menu_component) {
  current_state = MENU_CHANGE_RPM_ROTOR;

  menu_set_min_max_12bit_value = ((EEPROM.read(EEPROM_STEPPER_ROTOR_RPM_MS_POS) & 0x0F) << 8) | EEPROM.read(EEPROM_STEPPER_ROTOR_RPM_POS);
}

/**
 * Enter state which allows the adjustment of turntable stepper's RPM and read the current value from EEPROM.
 */
void on_menu_item_steppers_turntable_rpm(MenuComponent* p_menu_component) {
  current_state = MENU_CHANGE_RPM_TURNTABLE;

  menu_set_min_max_12bit_value = ((EEPROM.read(EEPROM_STEPPER_TURNTABLE_RPM_MS_POS) & 0x0F) << 8) | EEPROM.read(EEPROM_STEPPER_TURNTABLE_RPM_POS);
}

/**
 * Enter state which allows the adjustment of rotor stepper's acceleration and read the current value from EEPROM.
 */
void on_menu_item_steppers_rotor_acceleration(MenuComponent* p_menu_component) {
  current_state = MENU_CHANGE_ACCELERATION_ROTOR;

  menu_set_min_max_12bit_value = ((EEPROM.read(EEPROM_STEPPER_ROTOR_ACCEL_DECEL_POS) & 0x0F) << 8) | EEPROM.read(EEPROM_STEPPER_ROTOR_ACCEL_POS);
}

/**
 * Enter state which allows the adjustment of turntable stepper's acceleration and read the current value from EEPROM.
 */
void on_menu_item_steppers_turntable_acceleration(MenuComponent* p_menu_component) {
  current_state = MENU_CHANGE_ACCELERATION_TURNTABLE;

  menu_set_min_max_12bit_value = ((EEPROM.read(EEPROM_STEPPER_TURNTABLE_ACCEL_DECEL_POS) & 0x0F) << 8) | EEPROM.read(EEPROM_STEPPER_TURNTABLE_ACCEL_POS);
}

/**
 * Enter state which allows the adjustment of rotor stepper's deceleration and read the current value from EEPROM.
 */
void on_menu_item_steppers_rotor_deceleration(MenuComponent* p_menu_component) {
  current_state = MENU_CHANGE_DECELERATION_ROTOR;

  menu_set_min_max_12bit_value = ((EEPROM.read(EEPROM_STEPPER_ROTOR_ACCEL_DECEL_POS) & 0xF0) << 4) | EEPROM.read(EEPROM_STEPPER_ROTOR_DECEL_POS);
}

/**
 * Enter state which allows the adjustment of turntable stepper's deceleration and read the current value from EEPROM.
 */
void on_menu_item_steppers_turntable_deceleration(MenuComponent* p_menu_component) {
  current_state = MENU_CHANGE_DECELERATION_TURNTABLE;

  menu_set_min_max_12bit_value = ((EEPROM.read(EEPROM_STEPPER_TURNTABLE_ACCEL_DECEL_POS) & 0xF0) << 4) | EEPROM.read(EEPROM_STEPPER_TURNTABLE_DECEL_POS);
}


/**
 * Sets the speed mode of the rotor stepper to constant speed.
 */
void on_menu_item_steppers_rotor_speed_mode_constant(MenuComponent* p_menu_component) {
  EEPROM.write(EEPROM_STEPPER_BITMASK_POS, EEPROM.read(EEPROM_STEPPER_BITMASK_POS) & 0xFE);
  EEPROM.commit();

  stepper_rotor.setSpeedProfile(stepper_rotor.CONSTANT_SPEED, stepper_rotor.getAcceleration(), stepper_rotor.getDeceleration());
}

/**
 * Sets the speed mode of the turntable stepper to constant speed.
 */
void on_menu_item_steppers_turntable_speed_mode_constant(MenuComponent* p_menu_component) {
  EEPROM.write(EEPROM_STEPPER_BITMASK_POS, EEPROM.read(EEPROM_STEPPER_BITMASK_POS) & 0xEF);
  EEPROM.commit();

  stepper_turntable.setSpeedProfile(stepper_turntable.CONSTANT_SPEED, stepper_turntable.getAcceleration(), stepper_turntable.getDeceleration());
}

/**
 * Sets the speed mode of the rotor stepper to linear speed.
 */
void on_menu_item_steppers_rotor_speed_mode_linear(MenuComponent* p_menu_component) {
  EEPROM.write(EEPROM_STEPPER_BITMASK_POS, EEPROM.read(EEPROM_STEPPER_BITMASK_POS) | EEPROM_STEPPER_LINEAR_SPEED);
  EEPROM.commit();

  stepper_rotor.setSpeedProfile(stepper_rotor.LINEAR_SPEED, stepper_rotor.getAcceleration(), stepper_rotor.getDeceleration());
}

/**
 * Sets the speed mode of the turntable stepper to linear speed.
 */
void on_menu_item_steppers_turntable_speed_mode_linear(MenuComponent* p_menu_component) {
  EEPROM.write(EEPROM_STEPPER_BITMASK_POS, EEPROM.read(EEPROM_STEPPER_BITMASK_POS) | (EEPROM_STEPPER_LINEAR_SPEED << 4));
  EEPROM.commit();

  stepper_turntable.setSpeedProfile(stepper_turntable.LINEAR_SPEED, stepper_turntable.getAcceleration(), stepper_turntable.getDeceleration());
}

/**
 * The user wants to create a scan with custom settings.
 */
void on_menu_item_scan_custom_scan(MenuComponent* p_menu_component) {
  current_state = SCAN_CONFIGURE;
}

// this is not really necessary since it is defined ~20 lines later but we want to keep the file structure/ordering
extern MenuItem menu_item_presets[NUMBER_OF_SCAN_PRESETS];

/**
 * A preset was selected. It is necessary to determine which one and start the corresponding scan.
 */
void on_menu_item_scan_preset(MenuComponent* p_menu_component) {
  // determine the selected preset
  for (char i = 0; i < NUMBER_OF_SCAN_PRESETS; i++) {
    if (&menu_item_presets[i] == p_menu_component) {
      selected_scan = &scan_presets[i].preset;
      break;
    }
  }

  current_state = SCAN_CONFIRM;
}

/**
 * Adjust the delay after a photo was taken during scanning.
 */
void on_menu_item_settings_scan_photo_delay(MenuComponent* p_menu_component) {
  current_state = MENU_CHANGE_PHOTO_DELAY;
  menu_set_16bit_value = ((EEPROM.read(EEPROM_TIME_BETWEEN_PHOTO_HIGH_POS) & 0xFF) << 8) | EEPROM.read(EEPROM_TIME_BETWEEN_PHOTO_LOW_POS);
}

/**
 * Adjust the delay after a stepper was moved during scanning.
 */
void on_menu_item_settings_scan_move_delay(MenuComponent* p_menu_component) {
  current_state = MENU_CHANGE_MOVE_DELAY;
  menu_set_16bit_value = ((EEPROM.read(EEPROM_TIME_AFTER_MOVE_HIGH_POS) & 0xFF) << 8) | EEPROM.read(EEPROM_TIME_AFTER_MOVE_LOW_POS);
}

/**
 * Invert the rotor steppers direction during scanning.
 */
void on_menu_item_steppers_rotor_invert(MenuComponent* p_menu_component) {
  char bitMask = EEPROM.read(EEPROM_STEPPER_BITMASK_POS);
  if (bitRead(bitMask, 1) == 0) {
    bitWrite(bitMask, 1, 1);
    EEPROM.write(EEPROM_STEPPER_BITMASK_POS, bitMask);
  } else {
    bitWrite(bitMask, 1, 0);
    EEPROM.write(EEPROM_STEPPER_BITMASK_POS, bitMask);
  }

  EEPROM.commit();
}


/**
 * Invert the turntable steppers direction during scanning.
 */
void on_menu_item_steppers_turntable_invert(MenuComponent* p_menu_component) {
  char bitMask = EEPROM.read(EEPROM_STEPPER_BITMASK_POS);
  if (bitRead(bitMask, 5) == 0) {
    bitWrite(bitMask, 5, 1);
    EEPROM.write(EEPROM_STEPPER_BITMASK_POS, bitMask);
  } else {
    bitWrite(bitMask, 5, 0);
    EEPROM.write(EEPROM_STEPPER_BITMASK_POS, bitMask);
  }

  EEPROM.commit();
}

/**
 * Definitions of menus and menu items
 */
 
// The actual menu system containing all menus and menu items
MenuSystem menu_system(menu_renderer);

/**
 * The scanning menu is the main functionality menu since it enables the user to start
 * a new scan by either using one of the presets or by using a custom configuration.
 */
Menu menu_scan("Scan");
BackMenuItem menu_scan_back("..", &on_back_menu_item, &menu_system);
MenuItem menu_item_scan_custom_scan("Custom Scan", &on_menu_item_scan_custom_scan);
Menu menu_scan_presets("Presets");
BackMenuItem menu_scan_presets_back("..", &on_back_menu_item, &menu_system);
MenuItem menu_item_presets[NUMBER_OF_SCAN_PRESETS] = {
  MenuItem("", NULL),
  MenuItem("", NULL),
  MenuItem("", NULL),
  MenuItem("", NULL)
};

/**
 * The settings menu contains all configurable items for this application.
 * They are sorted in submenus based on the component they belong to.
 * 
 * Unfortunately it is necessary to define the back menu item every time it is used,
 * since there are errors regarding the currently selected element otherwise.
 */
Menu menu_settings("Settings");
BackMenuItem menu_item_settings_back("..", &on_back_menu_item, &menu_system);

/**
 * Scan settings menu contains scanning values that can be changed by the user.
 */
Menu menu_settings_scan("Scan");
BackMenuItem menu_item_settings_scan_back("..", &on_back_menu_item, &menu_system);
MenuItem menu_item_settings_scan_photo_delay("Photo Delay", &on_menu_item_settings_scan_photo_delay);
MenuItem menu_item_settings_scan_move_delay("Move Delay", &on_menu_item_settings_scan_move_delay);

/**
 * The display menu contains all adjustable settings for the LCD screen. These are:
 * - Turning the background light on/off
 * - Changing the constrast of the display
 */
Menu menu_settings_display("Display");
BackMenuItem menu_item_display_back("..", &on_back_menu_item_light_off, &menu_system);
MenuItem menu_item_display_switch_light("Switch Light", &on_menu_item_display_switch_light_selected);
MenuItem menu_item_display_change_contrast("Change Contrast", &on_menu_item_display_change_contrast_selected);

/**
 * The camera menu contains all adjustable settings for the camera. These are:
 * - Choosing the type of camera
 */
Menu menu_settings_camera("Camera");
BackMenuItem menu_item_camera_back("..", &on_back_menu_item, &menu_system);

/**
 * This menu allows to chose the type of used camera. Currently supported are:
 * - Android Devices
 * - iOS Devices
 */
Menu menu_settings_camera_type("Camera Type");
BackMenuItem menu_item_camera_type_back("..", &on_back_menu_item, &menu_system);
MenuItem menu_item_camera_type_android("Android Device", &on_menu_item_camera_type_android);
MenuItem menu_item_camera_type_ios("iOS Device", &on_menu_item_camera_type_iOS);

/**
 * The steppers menu contains all steppers which are used within this project. These are:
 * - Rotor stepper
 */
Menu menu_settings_steppers("Steppers");
BackMenuItem menu_item_steppers_back("..", &on_back_menu_item, &menu_system);

/**
 * The rotor menu contains all adjustable settings for the rotor stepper. These are:
 * - Inverting stepper direction
 * - Microstepping mode (currently disabled because of "missing" GPIOs)
 * - RPM
 * - Speed Mode
 * - Acceleration
 * - Deceleration
 */
Menu menu_settings_steppers_rotor("Rotor", &on_menu_rotor);
BackMenuItem menu_item_steppers_rotor_back("..", &on_back_menu_item_stop_task_stop_rotor, &menu_system);
MenuItem menu_item_steppers_rotor_invert("Invert Dir.", &on_menu_item_steppers_rotor_invert);

/**
 * Rotor Microstepping menu allows to set the desired microstepping module for the rotor stepper.
 * Since a A4988 is used as stepper driver the following modes are available:
 * - Full step
 * - Half step
 * - Quarter step
 * - Eighth step
 * - Sixteenth step
 */
Menu menu_steppers_rotor_microstepping("Microsteps");
BackMenuItem menu_item_steppers_rotor_microstepping_back("..", &on_back_menu_item, &menu_system);
MenuItem menu_item_steppers_rotor_microstepping_full_step("Full Step", &on_menu_item_steppers_rotor_microstepping_full_step);
MenuItem menu_item_steppers_rotor_microstepping_half_step("Half Step", &on_menu_item_steppers_rotor_microstepping_half_step);
MenuItem menu_item_steppers_rotor_microstepping_quarter_step("Quarter Step", &on_menu_item_steppers_rotor_microstepping_quarter_step);
MenuItem menu_item_steppers_rotor_microstepping_eighth_step("Eighth Step", &on_menu_item_steppers_rotor_microstepping_eighth_step);
MenuItem menu_item_steppers_rotor_microstepping_sixteenth_step("Sixteenth Step", &on_menu_item_steppers_rotor_microstepping_sixteenth_step);

MenuItem menu_item_steppers_rotor_rpm("RPM", &on_menu_item_steppers_rotor_rpm);

/**
 * Rotor Speed Mode menu allows to switch the used mode for accelerations and decelerations.
 * Available modes are constant speed and linear speed.
 */
Menu menu_steppers_rotor_speed_mode("Speed Mode");
BackMenuItem menu_item_steppers_rotor_speed_mode_back("..", &on_back_menu_item, &menu_system);
MenuItem menu_item_steppers_rotor_speed_mode_constant("Constant Speed", &on_menu_item_steppers_rotor_speed_mode_constant);
MenuItem menu_item_steppers_rotor_speed_mode_linear("Linear Speed", &on_menu_item_steppers_rotor_speed_mode_linear);

MenuItem menu_item_steppers_rotor_acceleration("Acceleration", &on_menu_item_steppers_rotor_acceleration);
MenuItem menu_item_steppers_rotor_deceleration("Deceleration", &on_menu_item_steppers_rotor_deceleration);

/**
 * The turntable menu contains all adjustable settings for the rotor stepper. These are:
 * - Inverting stepper direction
 * - Microstepping mode (currently disabled because of "missing" GPIOs)
 * - RPM
 * - Speed Mode
 * - Acceleration
 * - Deceleration
 */
Menu menu_settings_steppers_turntable("Turntable", &on_menu_turntable);
BackMenuItem menu_item_steppers_turntable_back("..", &on_back_menu_item_stop_task_stop_turntable, &menu_system);
MenuItem menu_item_steppers_turntable_invert("Invert Dir.", &on_menu_item_steppers_turntable_invert);

/**
 * Turntable Microstepping menu allows to set the desired microstepping module for the rotor stepper.
 * Since a A4988 is used as stepper driver the following modes are available:
 * - Full step
 * - Half step
 * - Quarter step
 * - Eighth step
 * - Sixteenth step
 */
Menu menu_steppers_turntable_microstepping("Microsteps");
BackMenuItem menu_item_steppers_turntable_microstepping_back("..", &on_back_menu_item, &menu_system);
MenuItem menu_item_steppers_turntable_microstepping_full_step("Full Step", &on_menu_item_steppers_turntable_microstepping_full_step);
MenuItem menu_item_steppers_turntable_microstepping_half_step("Half Step", &on_menu_item_steppers_turntable_microstepping_half_step);
MenuItem menu_item_steppers_turntable_microstepping_quarter_step("Quarter Step", &on_menu_item_steppers_turntable_microstepping_quarter_step);
MenuItem menu_item_steppers_turntable_microstepping_eighth_step("Eighth Step", &on_menu_item_steppers_turntable_microstepping_eighth_step);
MenuItem menu_item_steppers_turntable_microstepping_sixteenth_step("Sixteenth Step", &on_menu_item_steppers_turntable_microstepping_sixteenth_step);

MenuItem menu_item_steppers_turntable_rpm("RPM", &on_menu_item_steppers_turntable_rpm);

/**
 * Turntable Speed Mode menu allows to switch the used mode for accelerations and decelerations.
 * Available modes are constant speed and linear speed.
 */
Menu menu_steppers_turntable_speed_mode("Speed Mode");
BackMenuItem menu_item_steppers_turntable_speed_mode_back("..", &on_back_menu_item, &menu_system);
MenuItem menu_item_steppers_turntable_speed_mode_constant("Constant Speed", &on_menu_item_steppers_turntable_speed_mode_constant);
MenuItem menu_item_steppers_turntable_speed_mode_linear("Linear Speed", &on_menu_item_steppers_turntable_speed_mode_linear);

MenuItem menu_item_steppers_turntable_acceleration("Acceleration", &on_menu_item_steppers_turntable_acceleration);
MenuItem menu_item_steppers_turntable_deceleration("Deceleration", &on_menu_item_steppers_turntable_deceleration);

/**
 * The debug menu allows to trigger various functions which normally take place during normal operation.
 * Allows testing of the different components. Supported functions:
 * - Taking a photo on the connected device
 * - Switching the light status
 */
Menu menu_debug("Debug");
BackMenuItem menu_item_debug_back("..", &on_back_menu_item, &menu_system);
MenuItem menu_item_debug_take_photo("Take Photo", &on_menu_item_debug_take_photo);
MenuItem menu_item_debug_switch_light("Switch Light", &on_menu_item_debug_switch_light);

/**
 * This function will get data which is used for menu navigation from the current menu
 * and push it to the stack.
 */
void push_current_menu_navigation_data() {
  menu_navigation_data_stack.push(
    MenuNavigationData{0, min((uint8_t) 4, (uint8_t) (menu_system.get_current_menu()->get_num_components() - 1)), menu_system.get_current_menu()->get_num_components(), 0}
  );
}

/**
 * This function is always called when within the current menu the next item is selected.
 * It ensures proper scrolling behaviour and scrollbar handling.
 */
void navigate_forward() {
  MenuNavigationData current = menu_navigation_data_stack.pop();
  if (current.current_position < 0xFF) {
    current.current_position = (uint8_t) min((uint8_t) (current.current_position + 1), (uint8_t) (current.item_count - 1));
  }

  if (current.current_position > current.max_display) {
    current.min_display++;
    current.max_display++;
  }

  menu_navigation_data_stack.push(current);
}

/**
 * This function is always called when within the current menu the previous item is selected.
 * It ensures proper scrolling behaviour and scrollbar handling.
 */
void navigate_backward() {
  MenuNavigationData current = menu_navigation_data_stack.pop();
  if (current.current_position > 0) {
    current.current_position = (uint8_t) max((uint8_t) (current.current_position - 1), (uint8_t) 0);
  }

  if (current.current_position < current.min_display) {
    current.min_display--;
    current.max_display--;
  }

  menu_navigation_data_stack.push(current);
}

/**
 * This function builds the menu structure.
 * The wired itendation is a visual indication of the final menu structure.
 */
void initialize_menu_structure() {
  menu_system.get_root_menu().set_name("Main Menu");
  menu_system.get_root_menu().add_menu(&menu_scan);
    menu_scan.add_item(&menu_scan_back);
    menu_scan.add_item(&menu_item_scan_custom_scan);
    menu_scan.add_menu(&menu_scan_presets);
      menu_scan_presets.add_item(&menu_scan_presets_back);
      for (char i = 0; i < NUMBER_OF_SCAN_PRESETS; i++) {
        menu_item_presets[i] = MenuItem(scan_presets[i].display_name, &on_menu_item_scan_preset);
        menu_scan_presets.add_item(&menu_item_presets[i]);
      }
  menu_system.get_root_menu().add_menu(&menu_settings);
    menu_settings.add_item(&menu_item_settings_back);
    menu_settings.add_menu(&menu_settings_scan);
      menu_settings_scan.add_item(&menu_item_settings_scan_back);
      menu_settings_scan.add_item(&menu_item_settings_scan_photo_delay);
      menu_settings_scan.add_item(&menu_item_settings_scan_move_delay);
    menu_settings.add_menu(&menu_settings_display);
      menu_settings_display.add_item(&menu_item_display_back);
      menu_settings_display.add_item(&menu_item_display_switch_light);
      menu_settings_display.add_item(&menu_item_display_change_contrast);
    menu_settings.add_menu(&menu_settings_camera);
      menu_settings_camera.add_item(&menu_item_camera_back);
      menu_settings_camera.add_menu(&menu_settings_camera_type);
        menu_settings_camera_type.add_item(&menu_item_camera_type_back);
        menu_settings_camera_type.add_item(&menu_item_camera_type_android);
        menu_settings_camera_type.add_item(&menu_item_camera_type_ios);
    menu_settings.add_menu(&menu_settings_steppers);
      menu_settings_steppers.add_item(&menu_item_steppers_back);
      menu_settings_steppers.add_menu(&menu_settings_steppers_rotor);
        menu_settings_steppers_rotor.add_item(&menu_item_steppers_rotor_back);
        menu_settings_steppers_rotor.add_item(&menu_item_steppers_rotor_invert);
      //menu_settings_steppers_rotor.add_menu(&menu_steppers_rotor_microstepping);
          menu_steppers_rotor_microstepping.add_item(&menu_item_steppers_rotor_microstepping_back);
          menu_steppers_rotor_microstepping.add_item(&menu_item_steppers_rotor_microstepping_full_step);
          menu_steppers_rotor_microstepping.add_item(&menu_item_steppers_rotor_microstepping_half_step);
          menu_steppers_rotor_microstepping.add_item(&menu_item_steppers_rotor_microstepping_quarter_step);
          menu_steppers_rotor_microstepping.add_item(&menu_item_steppers_rotor_microstepping_eighth_step);
          menu_steppers_rotor_microstepping.add_item(&menu_item_steppers_rotor_microstepping_sixteenth_step);
        menu_settings_steppers_rotor.add_item(&menu_item_steppers_rotor_rpm);
        menu_settings_steppers_rotor.add_menu(&menu_steppers_rotor_speed_mode);
          menu_steppers_rotor_speed_mode.add_item(&menu_item_steppers_rotor_speed_mode_back);
          menu_steppers_rotor_speed_mode.add_item(&menu_item_steppers_rotor_speed_mode_constant);
          menu_steppers_rotor_speed_mode.add_item(&menu_item_steppers_rotor_speed_mode_linear);
        menu_settings_steppers_rotor.add_item(&menu_item_steppers_rotor_acceleration);
        menu_settings_steppers_rotor.add_item(&menu_item_steppers_rotor_deceleration);
      menu_settings_steppers.add_menu(&menu_settings_steppers_turntable);
        menu_settings_steppers_turntable.add_item(&menu_item_steppers_turntable_back);
        menu_settings_steppers_turntable.add_item(&menu_item_steppers_turntable_invert);
      //menu_settings_steppers_turntable.add_menu(&menu_steppers_turntable_microstepping);
          menu_steppers_turntable_microstepping.add_item(&menu_item_steppers_turntable_microstepping_back);
          menu_steppers_turntable_microstepping.add_item(&menu_item_steppers_turntable_microstepping_full_step);
          menu_steppers_turntable_microstepping.add_item(&menu_item_steppers_turntable_microstepping_half_step);
          menu_steppers_turntable_microstepping.add_item(&menu_item_steppers_turntable_microstepping_quarter_step);
          menu_steppers_turntable_microstepping.add_item(&menu_item_steppers_turntable_microstepping_eighth_step);
          menu_steppers_turntable_microstepping.add_item(&menu_item_steppers_turntable_microstepping_sixteenth_step);
        menu_settings_steppers_turntable.add_item(&menu_item_steppers_turntable_rpm);
        menu_settings_steppers_turntable.add_menu(&menu_steppers_turntable_speed_mode);
          menu_steppers_turntable_speed_mode.add_item(&menu_item_steppers_turntable_speed_mode_back);
          menu_steppers_turntable_speed_mode.add_item(&menu_item_steppers_turntable_speed_mode_constant);
          menu_steppers_turntable_speed_mode.add_item(&menu_item_steppers_turntable_speed_mode_linear);
        menu_settings_steppers_turntable.add_item(&menu_item_steppers_turntable_acceleration);
        menu_settings_steppers_turntable.add_item(&menu_item_steppers_turntable_deceleration);
  menu_system.get_root_menu().add_menu(&menu_debug);
    menu_debug.add_item(&menu_item_debug_back);
    menu_debug.add_item(&menu_item_debug_take_photo);
    menu_debug.add_item(&menu_item_debug_switch_light);

  // add the first entry for the root menu onto the stack
  push_current_menu_navigation_data();
}
