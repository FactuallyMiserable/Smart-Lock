#include <Keypad.h>

const byte ROWS = 3;
const byte COLS = 3;
byte rowPins[ROWS] = {32, 33, 25};
byte colPins[COLS] = {26, 27, 14};
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'}
};

Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(115200);
  for (byte i = 0; i < ROWS; i++) {
    pinMode(rowPins[i], INPUT_PULLUP);
  }
  for (byte j = 0; j < COLS; j++) {
    pinMode(colPins[j], OUTPUT);
    digitalWrite(colPins[j], HIGH);
  }
  Serial.println("Keypad ready. Press any key.");
}

void loop() {
  char c = keypad.getKey();
  if (c) {
    Serial.print("Key pressed: ");
    Serial.println(c);
  }
}
