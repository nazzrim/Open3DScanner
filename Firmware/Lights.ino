
/**
 * This code manages the outputpin for the transistor which is driving the spotlights.
 */

boolean is_light_on = false;

void turn_light_on() {
  if (is_light_on) return;

  is_light_on = true;
  digitalWrite(LIGHT_SWITCH, HIGH);
}

void turn_light_off() {
  if (!is_light_on) return;

  is_light_on = false;
  digitalWrite(LIGHT_SWITCH, LOW);
}

void switch_light() {
  if (is_light_on)
    turn_light_off();
  else
    turn_light_on();
}

/**
 * Configure output pin for the correct mode.
 */
void initialize_light() {
  pinMode(LIGHT_SWITCH, OUTPUT);
  digitalWrite(LIGHT_SWITCH, LOW);
}
