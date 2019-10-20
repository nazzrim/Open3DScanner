#include <Nokia_LCD.h>

/**
 * The used Nokia5110 Display is a monochrome 84*48 display which is used to display various information and contextual data to the user.
 */

// delay in ms between 2 frames/logic executions
#define TIME_BETWEEN_FRAMES 50

/**
 * Each character drawn takes 5*8 pixels of the 84*48 pixel display.
 */
Nokia_LCD lcd(DISPLAY_CLK, DISPLAY_DIN, DISPLAY_DC, DISPLAY_CE, DISPLAY_RST);

/**
 * Initialization of the lcd.
 * This includes clearing its content and loading relevant data from EEPROM for configuration.
 * 
 * The settings for contrast and backgroundlight are stored within EEPROM at address EEPROM_LCD_BL_CONTRAST_POS.
 * Bit seven (most significant one) is used as flag if the backgroundlight shall be turned on (1) or off (0), 
 * while bits 0 to 6 contain a value between 0 and 127 which is used for the contrast of the display.
 * 
 * Bitorder within a byte:
 * Value  | 2^7 | 2^6 | 2^5 | 2^4 | 2^3 | 2^2 | 2^1 | 2^0
 * Index  |   7 |   6 |   5 |   4 |   3 |   2 |   1 |   0
 * Most significant <-----------------> Least significant
 */
void initialize_display() {
  lcd.begin();
  // the parameter indicates if the display should be filled with black pixels
  lcd.clear(false);
  // read relevant data from EEPROM
  uint8_t bl_constrast_setting = EEPROM.read(EEPROM_LCD_BL_CONTRAST_POS);
  // extract and set contrast value
  lcd.setContrast(bl_constrast_setting & 0x7F);
  // ensure the pin used for the background light is an output pin
  pinMode(DISPLAY_BL, OUTPUT);
  // extract and set state of display's background light
  if ((bl_constrast_setting & 0x80) > 0) {
    digitalWrite(DISPLAY_BL, HIGH);
  } else {
    digitalWrite(DISPLAY_BL, LOW);
  }
}

/**
 * Utility function which allows to print a string centered at a given line.
 */
void print_centered(String str, char line) {
  lcd.setCursor((84 - (str.length() * 5)) / 2, line);
  lcd.print(str.c_str());
}
