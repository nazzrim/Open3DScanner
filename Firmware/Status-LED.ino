/**
 * The status LED is used to present the current state of the application to the user.
 * Green --> Turned on
 * Yellow --> Operating (Scanning)
 * Red --> Error
 * 
 * Various blinking frequencies are used to create "sub-status"
 */

// Time how long the led is turned on/off for fast blinking
short FAST_BLINK_TIME = 250;

// Time how long the led is turned on/off for slow blinking
short SLOW_BLINK_TIME = 500;

// Time how long the led is turned on/off for extra slow blinking
short EXTRA_SLOW_BLINK_TIME = 750;

// This handle is used for task which run only a defined period of time and need to be deleted
TaskHandle_t blinking_task_handle = NULL;

/**
 * Initialization of the red/green duo LED which is used for indicating the
 * current status (read/operating).
 */
void initialize_status_led() {
  pinMode(STATUS_LED_GREEN, OUTPUT);
  digitalWrite(STATUS_LED_GREEN, LOW);
  
  pinMode(STATUS_LED_RED, OUTPUT);
  digitalWrite(STATUS_LED_RED, LOW);

  status_led_green();
}

// Enable LED in green mode
void status_led_green() {
  digitalWrite(STATUS_LED_RED, LOW);
  digitalWrite(STATUS_LED_GREEN, HIGH);
}

// Enable LED in yellow (red+green) mode
void status_led_yellow() {
  digitalWrite(STATUS_LED_RED, HIGH);
  digitalWrite(STATUS_LED_GREEN, HIGH);
}

// Enable LED in red mode
void status_led_red() {
  digitalWrite(STATUS_LED_GREEN, LOW);
  digitalWrite(STATUS_LED_RED, HIGH);
}

// Turn LED off
void status_led_off() {
  digitalWrite(STATUS_LED_GREEN, LOW);
  digitalWrite(STATUS_LED_RED, LOW);
}

/**
 * Start freeRTOS task which blinks the led.
 * speed_mode:
 * - 0 --> slow blinking
 * - 1 --> fast blinking
 * - 2 --> extra slow blinking
 * 
 * color:
 * - 0 --> red
 * - 1 --> yellow
 * - 2 --> green
 */
void start_blinking(uint8_t speed_mode, uint8_t color) {
  xTaskCreate(blinking_task, "StatusLedBlinking", 1000, (void*)(color | (speed_mode << 4)), 5, &blinking_task_handle);
}

/**
 * If the status LED is currently blinking, stop blinking and turn LED off.
 */
void stop_blinking() {
  if (blinking_task_handle != NULL) {
    vTaskSuspend(blinking_task_handle);
    vTaskDelete(blinking_task_handle);
    blinking_task_handle = NULL;
    status_led_off();
  }
}

/**
 * Starts blinking of the status LED with given color and speed.
 * The data is coded within a single uint32_t
 * See start_blinking documentation for further details on the provided parameter
 */
void blinking_task(void* parameter) {
  // extract values from parameters
  uint32_t param = (uint32_t) parameter;
  uint8_t color = param & 0x0F;
  uint8_t speed_mode = (param >> 4) & 0x0F;
  // obtain used values/functions
  short delay_time = 0;
  switch (speed_mode) {
    case 0:
      delay_time = SLOW_BLINK_TIME;
      break;
    case 1:
      delay_time = FAST_BLINK_TIME;
      break;
    case 2:
      delay_time = EXTRA_SLOW_BLINK_TIME;
      break;
  }
  void (*led_on)() = NULL;
  switch (color) {
    case 0:
      led_on = &status_led_red;
      break;
    case 1:
      led_on = &status_led_yellow;
      break;
    case 2:
      led_on = &status_led_green;
      break;
  }
  // at first turn of the led to ensure no colors overlap
  status_led_off();
  // actual blinking of the status led
  while (true) {
    led_on();
    vTaskDelay(delay_time);
    status_led_off();
    vTaskDelay(delay_time);
  }
}
