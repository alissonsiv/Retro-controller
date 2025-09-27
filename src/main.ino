#include <Arduino.h>
#include "Adafruit_TinyUSB.h"

// ------------------- CONFIGURAÇÃO DE PINOS -------------------
const int JOYSTICK_X = 34;
const int JOYSTICK_Y = 35;
const int BUTTON_1   = 12;
const int BUTTON_2   = 13;

// ------------------- HID -------------------
Adafruit_USBD_HID usb_hid;
uint8_t report[3]; // 1 byte X, 1 byte Y, 1 byte botões (8 bits)

// ------------------- DEBOUNCE SEM DELAY -------------------
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

// Função de leitura de botão com debounce sem delay
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

// Retorna os estados dos botões em um byte
uint8_t getButtonByte() {
  uint8_t buttons = 0;
  if (readButton(button1)) buttons |= 0x01; // Botão 1
  if (readButton(button2)) buttons |= 0x02; // Botão 2
  return buttons;
}

// ------------------- JOYSTICK -------------------
int mapJoystick(int value) {
  // ESP32 ADC 0-4095 → -127 a 127
  return map(value, 0, 4095, -127, 127);
}

// ------------------- SETUP -------------------
void setup() {
  Serial.begin(115200);

  pinMode(JOYSTICK_X, INPUT);
  pinMode(JOYSTICK_Y, INPUT);
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);

  // Inicializa USB HID como joystick (2 eixos, 8 botões)
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor({
    0x05, 0x01,        // USAGE_PAGE (Generic Desktop)
    0x09, 0x04,        // USAGE (Joystick)
    0xa1, 0x01,        // COLLECTION (Application)
    0x09, 0x01,        //   USAGE (Pointer)
    0xa1, 0x00,        //   COLLECTION (Physical)
    0x09, 0x30,        //     USAGE (X)
    0x09, 0x31,        //     USAGE (Y)
    0x15, 0x81,        //     LOGICAL_MINIMUM (-127)
    0x25, 0x7F,        //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,        //     REPORT_SIZE (8)
    0x95, 0x02,        //     REPORT_COUNT (2)
    0x81, 0x02,        //     INPUT (Data,Var,Abs)
    0xc0,              //   END_COLLECTION
    0x05, 0x09,        //   USAGE_PAGE (Button)
    0x19, 0x01,        //   USAGE_MINIMUM (Button 1)
    0x29, 0x08,        //   USAGE_MAXIMUM (Button 8)
    0x15, 0x00,        //   LOGICAL_MINIMUM (0)
    0x25, 0x01,        //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,        //   REPORT_SIZE (1)
    0x95, 0x08,        //   REPORT_COUNT (8)
    0x81, 0x02,        //   INPUT (Data,Var,Abs)
    0xc0               // END_COLLECTION
  }, sizeof(report));

  usb_hid.begin();
  delay(1000); // tempo para inicializar USB
}

// ------------------- LOOP -------------------
void loop() {
  // Leitura dos botões
  uint8_t buttonByte = getButtonByte();

  // Leitura do joystick
  int x = mapJoystick(analogRead(JOYSTICK_X));
  int y = mapJoystick(analogRead(JOYSTICK_Y));

  // Atualiza relatório HID
  report[0] = x;
  report[1] = y;
  report[2] = buttonByte;

  usb_hid.sendReport(0, report, sizeof(report));
  
  delay(5); // mantém taxa de atualização estável
}
