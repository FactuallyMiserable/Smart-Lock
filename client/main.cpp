#include <Keypad.h>

/*Includes for ESP-NOW communication, based on 2.4Ghz WiFi*/
#include <WiFi.h>
#include <esp_now.h>

/*Includes for MFRC522*/
#include <SPI.h>
#include <MFRC522.h>

/*Includes for LCD*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/*LCD Address and Pin Conf*/
#define I2C_ADDR  0x27 // I2C address is 0x27 for the 16x2 display
#define LCD_SDA 21
#define LCD_SCL 22

#define LCD_COLUMNS 16
#define LCD_ROWS 2

LiquidCrystal_I2C lcd(I2C_ADDR, LCD_COLUMNS, LCD_ROWS);

void PrintToLCD(int row, const String& text) {
  if (row > 2 || row < 1) return;
  // set cursor to first column, chosen row
  lcd.setCursor(0, row-1);
  // print message
  lcd.print(text);
}

void ClearLCD() {
  lcd.clear();
}

/*MFRC522 Pin Conf*/
#define SDA_PIN 5
#define RST_PIN 17
#define SCK_PIN 18
#define MOSI_PIN 23
#define MISO_PIN 19

MFRC522 mfrc522(SDA_PIN, RST_PIN);  // Create MFRC522 instance

const uint8_t receiverMACAddress[] = {0x24, 0x6F, 0x28, 0xE2, 0x8B, 0x1A};  // Example MAC address

/*32 bytes sized messaged, could be resized later*/
typedef struct struct_message {
	char msg[32];
} struct_message;

struct_message myData; // Create a global instance of the structure to hold the message

volatile bool responseReceived = false;
volatile bool accessGranted = false;

void onDataReceived(const esp_now_recv_info* info, const uint8_t *incomingData, int len) {
  char msg[6];
  memcpy(msg, incomingData, len);
  msg[len] = '\0';

  if (strcmp(msg, "true") == 0) {
    accessGranted = true;
  } else {
    accessGranted = false;
  }
  responseReceived = true;
}

#define ROW_NUM 4
#define COL_NUM 3

/*Matrix representing keys in keys*/
const char keys[ROW_NUM][COL_NUM] = {
	{'1', '2', '3'},
	{'4', '5', '6'},
	{'7', '8', '9'},
	{'*', '0', '#'}
};

// char keypresses[4];
// int keypresses_index = 0;

/*Matching GPIO pins*/
byte pin_rows[ROW_NUM] = {32, 33, 25, 13};
byte pin_cols[COL_NUM] = {26, 27, 14};

/*Create keypad object
Example: char key = keypad.getKey();*/
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_cols, ROW_NUM, COL_NUM);


void setup() {
  /*Initialize the I2C bus*/
  Wire.begin(LCD_SDA, LCD_SCL);

  /*Initialize LCD*/
  lcd.init();
  /*turn on LCD backlight*/                 
  lcd.backlight();

	Serial.begin(115200);

	SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SDA_PIN);  // Initialize SPI bus
  
  mfrc522.PCD_Init();  // Initialize the MFRC522 RFID reader

	/*Initialize WiFi in Station mode (necessary for ESP-NOW)*/
	WiFi.mode(WIFI_STA);
	Serial.println("ESP32 Sender (Transmitter) Initialized.");

	/*Initialize ESP-NOW*/
	if (esp_now_init() != ESP_OK) {
		Serial.println("ESP-NOW Initialization failed.");
		return;
	}

	/*Register the callback function to receive messages*/
	esp_now_register_recv_cb(onDataReceived);
  
	/*Add the receiver's MAC address as a peer*/
	esp_now_peer_info_t peerInfo;
	memcpy(peerInfo.peer_addr, receiverMACAddress, 6);
	peerInfo.channel = 0;  // Use the default channel
	peerInfo.encrypt = true; // No encryption
	if (esp_now_add_peer(&peerInfo) != ESP_OK) {
		Serial.println("Failed to add peer.");
		return;
	}
  
	/*Prepare the data to send*/
	strcpy(myData.msg, "Hello from ESP32 Sender!");

	/*Send message*/
	// esp_now_send(receiverMACAddress, (uint8_t *)&myData, sizeof(myData));
  // Serial.println("Message Sent.");
}

void loop() {
    // Use LCD functions to Print to Rows 1 and 2
    ClearLCD();
    lcd.setCursor(0, 0);
    PrintToLCD(1, "Choose Mode:");
    PrintToLCD(2, "1. NFC 2. Keypad");


    char key = keypad.getKey();
    delay(50); // Debounce

    if (key == '1') {
      // RFID Logic
      ClearLCD();
      lcd.setCursor(0, 0);
      PrintToLCD(1, "Choose mode:");
      PrintToLCD(2, "1. READ 2. ADD");
      while (true) {  
        char key = keypad.getKey();
        if (key) {  // If a key is detected, break the loop
            if (key == '1') {
              char newKey = keypad.getKey();
              while (true) {
                if (newKey) break;
                if (!mfrc522.PICC_IsNewCardPresent())
                  continue;
        
                // Select one of the cards
                if (!mfrc522.PICC_ReadCardSerial())
                  continue;

                if (esp_now_send(receiverMACAddress, mfrc522.uid.uidByte, mfrc522.uid.size) == ESP_OK) {
                  ClearLCD();
                  lcd.setCursor(0, 0);
                  PrintToLCD(1, "Sent!");
                  /*Wait for approval from server side*/
                } else {
                  ClearLCD();
                  lcd.setCursor(0, 0);
                  PrintToLCD(1, "Sent!");
                }

                delay(1000);

                break;

              }
            } else if (key == '2') {
              // Write a logic that requires authentication by special passcode
            } else if (key == '#') {
              return; // Go back to main menu
            } else {
              ClearLCD();
              lcd.setCursor(0, 0);
              PrintToLCD(1, "Invalid Option");
              delay(1000);
            }

            break;
        }
      }


    } else if (key == '2') {
      ClearLCD();
      lcd.setCursor(0, 0);
      PrintToLCD(1, "Enter Password:");
      PrintToLCD(2, "_ _ _ _");
      char stream[] = {'A', 'A', 'A', 'A', 'A'};
      int idx = 0;
      while (true) {
        char key = keypad.getKey();
        delay(50); // Debounce

        if (key == '*') {
          idx = 0;
          memset(stream, 'A', sizeof(stream));
          lcd.setCursor(0, 0);
          ClearLCD();
          PrintToLCD(1, "Enter Password:");
          PrintToLCD(2, "_ _ _ _");
          continue;
        }
        else if (key == '#') {
          return; // Go back to main menu
        }
        else if (key && idx < 4) {
          stream[idx] = key;
          idx++;

          String pressed = "";
          for (int i = 0; i < 4; i++) {
            if (stream[i] == 'A') {
              pressed += "_ ";
            }
            else {
              pressed += stream[i];
              pressed += " ";
            }
          }

          lcd.setCursor(0, 0);
          ClearLCD();
          PrintToLCD(1, "Enter Password:");
          PrintToLCD(2, pressed);
        }

        if (idx == 4) {
          break;
        }
      }

      stream[4] = '\0';

      /*Prepare the data to send*/
      strcpy(myData.msg, stream);

      /*Send message*/
      responseReceived = false;
      accessGranted = false;

      /* Send message */
      esp_now_send(receiverMACAddress, (uint8_t *)&myData, sizeof(myData));
      Serial.println("Message Sent.");

      /* Wait for response or user abort (#) */
      unsigned long startTime = millis();
      lcd.setCursor(0, 0);
      ClearLCD();
      PrintToLCD(1, "Waiting for");
      PrintToLCD(2, "response...");

      while (!responseReceived) {
        if (keypad.getKey() == '#') {
          lcd.setCursor(0, 0);
          ClearLCD();
          PrintToLCD(1, "Aborted");
          delay(1000);
          return;
        }

        if (millis() - startTime > 10000) { // Optional timeout after 10s
          lcd.setCursor(0, 0);
          ClearLCD();
          PrintToLCD(1, "Timeout");
          delay(1000);
          return;
        }

        delay(50);
      }

      /* Show result */
      ClearLCD();
      lcd.setCursor(0, 0);
      
      if (accessGranted) {
        PrintToLCD(1, "Access Granted");
      } else {
        PrintToLCD(1, "Access Denied");
      }
      delay(2000);
    }
  
  // /* Look for new cards */
  // if ( !mfrc522.PICC_IsNewCardPresent()) {
  //   return;
  // }

  // // Select one of the cards
  // if ( !mfrc522.PICC_ReadCardSerial()) {
  //   return;
  // }

  // // Print UID of the card
  // Serial.print("UID tag :");
  // String content = "";
  // for (byte i = 0; i < mfrc522.uid.size; i++) {
  //   content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : ""));
  //   content.concat(String(mfrc522.uid.uidByte[i], HEX));
  // }
  // Serial.println(content);
  
  // mfrc522.PICC_HaltA();  // Halt the PICC (Card)
  // mfrc522.PCD_StopCrypto1();  // Stop encryption on the reader


	// /*Send the message to the receiver ESP32*/
	// esp_err_t result = esp_now_send(receiverMACAddress, (uint8_t *)&myData, sizeof(myData));

	// if (result == ESP_OK) {
	// 	Serial.println("Message sent successfully.");
	// } else {
	// 	Serial.println("Error sending message.");
	// }

	// delay(2000); // Wait for 2 seconds before sending the next message
  ClearLCD();
  lcd.setCursor(0, 0);
}