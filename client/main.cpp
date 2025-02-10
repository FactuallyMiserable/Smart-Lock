#include <Keypad.h>

/*Includes for ESP-NOW communication, based on 2.4Ghz WiFi*/
#include <WiFi.h>
#include <esp_now.h>


const uint8_t receiverMACAddress[] = {0x24, 0x6F, 0x28, 0xE2, 0x8B, 0x1A};  // Example MAC address

/*32 bytes sized messaged, could be resized later*/
typedef struct struct_message {
	char msg[32];
} struct_message;

struct_message myData; // Create a global instance of the structure to hold the message

void onDataReceived(const uint8_t *mac, const uint8_t *incomingData, int len) {
	struct_message incomingMessage;
	memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));

	/*Print the received message*/
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
const byte pin_rows[ROW_NUM] = {34, 35, 32, 33};
const byte pin_cols[COL_NUM] = {25, 26, 27};

/*Create keypad object
Example: char key = keypad.getKey();*/
Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_cols, ROW_NUM, COL_NUM);

void setup() {
	Serial.begin(115200);

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
	peerInfo.encrypt = false; // No encryption
	if (esp_now_add_peer(&peerInfo) != ESP_OK) {
		Serial.println("Failed to add peer.");
		return;
	}
  
	/*Prepare the data to send*/
	strcpy(myData.msg, "Hello from ESP32 Sender!");
}

void loop() {
	/*Send the message to the receiver ESP32*/
	esp_err_t result = esp_now_send(receiverMACAddress, (uint8_t *)&myData, sizeof(myData));

	if (result == ESP_OK) {
		Serial.println("Message sent successfully.");
	} else {
		Serial.println("Error sending message.");
	}

	delay(2000); // Wait for 2 seconds before sending the next message
}