#include <Keypad.h>

/*Includes for ESP-NOW communication, based on 2.4Ghz WiFi*/
#include <WiFi.h>
#include <esp_now.h>

/*Includes for MFRC522*/
#include <SPI.h>
#include <MFRC522.h>

/*MFRC522 Pin Conf*/
#define SDA_PIN 5    // SDA connected to D5 (GPIO 5)
#define RST_PIN 22   // RST connected to D22 (GPIO 22)
#define SCK_PIN 18   // SCK connected to D18 (GPIO 18)
#define MOSI_PIN 19  // MOSI connected to D19 (GPIO 19)
#define MISO_PIN 23  // MISO connected to D23 (GPIO 23)


MFRC522 mfrc522(SDA_PIN, RST_PIN);  // Create MFRC522 instance

const uint8_t receiverMACAddress[] = {0x24, 0x6F, 0x28, 0xE2, 0x8B, 0x1A};  // Example MAC address

/*32 bytes sized messaged, could be resized later*/
typedef struct struct_message {
	char msg[32];
} struct_message;

struct_message myData; // Create a global instance of the structure to hold the message

void onDataReceived(const esp_now_recv_info* info, const uint8_t *incomingData, int len) {
	struct_message incomingMessage;
	memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));

	Serial.print("Received message: ");
	Serial.println(incomingMessage.msg);
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

/*Matching GPIO pins*/
byte pin_rows[ROW_NUM] = {34, 35, 32, 33};
byte pin_cols[COL_NUM] = {25, 26, 27};

/*Create keypad object
Example: char key = keypad.getKey();*/
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_cols, ROW_NUM, COL_NUM);

void setup() {
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
	esp_now_send(receiverMACAddress, (uint8_t *)&myData, sizeof(myData));
  Serial.println("Message Sent.");
}

void loop() {
  /* Look for new cards */
  if ( !mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Print UID of the card
  Serial.print("UID tag :");
  String content = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : ""));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println(content);
  
  mfrc522.PICC_HaltA();  // Halt the PICC (Card)
  mfrc522.PCD_StopCrypto1();  // Stop encryption on the reader


	/*Send the message to the receiver ESP32*/
	esp_err_t result = esp_now_send(receiverMACAddress, (uint8_t *)&myData, sizeof(myData));

	if (result == ESP_OK) {
		Serial.println("Message sent successfully.");
	} else {
		Serial.println("Error sending message.");
	}

	delay(2000); // Wait for 2 seconds before sending the next message
}