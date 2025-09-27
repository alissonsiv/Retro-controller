#ifndef JOYSTICK_H
#define JOYSTICK_H

const int JOYSTICK_X = 34;
const int JOYSTICK_Y = 35;

int mapJoystick(int value) {
  return map(value, 0, 4095, -127, 127);
}

bool debounceButton(int pin, int &lastState) {
  int reading = !digitalRead(pin);
  if (reading != lastState) {
    delay(10);
    reading = !digitalRead(pin);
  }
  lastState = reading;
  return reading;
}

#endif
