/**
 * Simple state logic which allows the navigation of the menu structure.
 */

// The actual definition can be found in Menu.ino
extern MenuSystem menu_system;

// Encoder initialization can be found in Encoder.ino
extern ESP32Encoder encoder;

// Encoder button initialization can be found in Encoder.ino
extern volatile boolean encoder_button_pressed;

// Can be found in Menu.ino
extern void push_current_menu_navigation_data();

// Can be found in Menu.ino
extern void navigate_forward();

// Can be found in Menu.ino
extern void navigate_backward();

/**
 * Logic function for this state handles user input to navigate the menu which is displayed on the lcd.
 */
void menu_state_logic() {
  // process inputs
  // the encoder button has higher precedence than the encoder rotation
  if (encoder_button_pressed) {
    menu_system.select();
    consume_encoder_button();
    push_current_menu_navigation_data();
  } else if ((int32_t)encoder.getCount() != 0) {
    if ((int32_t)encoder.getCount() > 0) {
      menu_system.next();
      navigate_forward();
    } else {
      menu_system.prev();
      navigate_backward();
    }
    encoder.setCount(0);
  }
  // draw the menu
  menu_system.display();
  // sleep some time for (better) visibility on screen
  delay(TIME_BETWEEN_FRAMES);
}
