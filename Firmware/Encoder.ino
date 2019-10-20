#include <ESP32Encoder.h>

/**
 * The used encoder does provide a button in addition to the standard encoder functionality
 * (detecting rotation). The encoder is used for all user input.
 */

/**
 * This is the minimal time (in ms) the encoder button can not be triggered (software side)
 * after the triggering was consumed and resulted in some kind of action.
 * This prevents "flickering" on the encoder button.
 */
#define ENCODER_BUTTON_DEBOUNCE_TIME 350

// Time since startup since the encoder button was pressed the last time
volatile unsigned long encoder_button_last_state_change = 0;

// Encoder used for user input
ESP32Encoder encoder;

// State of the encoder button
volatile boolean encoder_button_pressed = false;

/**
 * Set encoder button to not pressed and store curren time.
 */
void consume_encoder_button() {
  encoder_button_last_state_change = millis();
  encoder_button_pressed = false;
}

/**
 * Interrupt service function for the encoder button.
 * Ensures that the pressed encoder button is only registered
 * after ENCODER_BUTTON_DEBOUNCE_TIME ms after the button was
 * pressed the last time or an action was triggered because of
 * the encoder button press.
 * 
 * Stored in RAM instead of normal flash storage.
 */
void IRAM_ATTR encoder_button_isr() {
  if (digitalRead (ENCODER_BUTTON) == LOW) {
    unsigned long current_millis = millis();
    if (current_millis - encoder_button_last_state_change > ENCODER_BUTTON_DEBOUNCE_TIME) {
      encoder_button_pressed = true;
      encoder_button_last_state_change = current_millis;
    }
  } else {
    encoder_button_pressed = false;
  }
}

/**
 * Bind defined keys to encoder logic and setup the bin which is defined for the encoder button.
 * 
 * Currently the encoder button pin is not registered for interrupts because the BLE functionality
 * somewhat messes up the interrupt handling because it disables interrupt handling internally.
 */
void initialize_encoder() {
  encoder.attachHalfQuad(ENCODER_PHASE_A, ENCODER_PHASE_B);
  encoder.setCount(0);
  pinMode(ENCODER_BUTTON, INPUT_PULLUP);
  attachInterrupt(ENCODER_BUTTON, encoder_button_isr, CHANGE);
}
