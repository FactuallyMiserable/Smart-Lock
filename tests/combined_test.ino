#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// Pins
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2
#define SDA_PIN 21
#define SCL_PIN 22

#define RST_PIN 17
#define SS_PIN 5

const byte ROWS_ALL = 3;
const byte COLS_ALL = 3;
byte rowPinsAll[ROWS_ALL] = {32, 33, 25};
byte colPinsAll[COLS_ALL] = {26, 27, 14};
char keysAll[ROWS_ALL][COLS_ALL] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'}
};

LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
MFRC522 rfid(SS_PIN, RST_PIN);
Keypad keypadAll(makeKeymap(keysAll), rowPinsAll, colPinsAll, ROWS_ALL, COLS_ALL);

void setup() {
  Serial.begin(115200);

  // LCD init
  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Combined Test");

  // RFID init
  SPI.begin(18, 19, 23, SS_PIN);
  rfid.PCD_Init();

  // Keypad init
  for (byte i = 0; i < ROWS_ALL; i++) pinMode(rowPinsAll[i], INPUT_PULLUP);
  for (byte j = 0; j < COLS_ALL; j++) { pinMode(colPinsAll[j], OUTPUT); digitalWrite(colPinsAll[j], HIGH); }

  delay(1000);
  lcd.clear();
}

void loop() {
  // Keypad
  char c = keypadAll.getKey();
  if (c) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Key: "); lcd.print(c);
    Serial.print("Key: "); Serial.println(c);
    delay(500);
    lcd.clear();
  }

  // RFID
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("UID:");
    for (byte i = 0; i < rfid.uid.size; i++) {
      lcd.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      lcd.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.print("RFID UID: ");
    for (byte i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
      Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();
    delay(2000);
    lcd.clear();
  }
}
