#include "joystick.h"
#include <HID-Project.h>
#include <HID-Settings.h>

// Configuração de botões
const int buttonPin1 = 12;
const int buttonPin2 = 13;

int lastButton1State = LOW;
int lastButton2State = LOW;

void setup() {
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);

  pinMode(JOYSTICK_X, INPUT);
  pinMode(JOYSTICK_Y, INPUT);

  Gamepad.begin();
}

void loop() {
  bool btn1 = debounceButton(buttonPin1, lastButton1State);
  bool btn2 = debounceButton(buttonPin2, lastButton2State);

  Gamepad.setButton(0, btn1);
  Gamepad.setButton(1, btn2);

  int x = mapJoystick(analogRead(JOYSTICK_X));
  int y = mapJoystick(analogRead(JOYSTICK_Y));

  Gamepad.setX(x);
  Gamepad.setY(y);

  Gamepad.sendReport();
  delay(5);
}
