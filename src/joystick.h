#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <Arduino.h>

// ------------------- PINOS -------------------
const int JOYSTICK_X = 34; // ADC X
const int JOYSTICK_Y = 35; // ADC Y
const int BUTTON_1   = 12; // Botão 1
const int BUTTON_2   = 13; // Botão 2

// ------------------- DEBOUNCE -------------------
struct Button {
  int pin;
  bool state;
  bool lastReading;
  unsigned long lastDebounceTime;
  unsigned long debounceDelay;
};

// Inicializa os botões
Button button1 = {BUTTON_1, LOW, LOW, 0, 10};
Button button2 = {BUTTON_2, LOW, LOW, 0, 10};

// Debounce sem delay
bool readButton(Button &btn) {
  bool reading = !digitalRead(btn.pin); // INPUT_PULLUP ativo
  if (reading != btn.lastReading) {
    btn.lastDebounceTime = millis();
  }
  btn.lastReading = reading;

  if ((millis() - btn.lastDebounceTime) > btn.debounceDelay) {
    if (reading != btn.state) {
      btn.state = reading;
    }
  }
  return btn.state;
}

// ------------------- JOYSTICK -------------------
int mapJoystick(int value) {
  // ADC 0-4095 → -127 a 127
  return map(value, 0, 4095, -127, 127);
}

// ------------------- RELATÓRIO DE BOTÕES -------------------
uint8_t getButtonByte() {
  uint8_t buttons = 0;
  if (readButton(button1)) buttons |= 0x01; // Botão 1 no bit 0
  if (readButton(button2)) buttons |= 0x02; // Botão 2 no bit 1
  return buttons;
}

#endif
