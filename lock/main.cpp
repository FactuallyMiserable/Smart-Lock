/*Includes for ESP-NOW communication, based on 2.4Ghz WiFi*/
#include <WiFi.h>
#include <esp_now.h>

#define PASSWORD_LEN 4
#define PASSWORDS_AMOUNT 3

const uint8_t receiverMACAddress[] = {0x24, 0x6F, 0x28, 0xE2, 0x8B, 0x1A}; // Example MAC address

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

const int passwords[PASSWORDS_AMOUNT][PASSWORD_LEN] = {
	{1, 2, 3, 4},
	{2, 0, 0, 7},
	{3, 3, 3, 3}
};

int password_buffer[PASSWORD_LEN] = {0};

/*Knowing array is passed by reference*/
void clear_buffer(int (&buff)[PASSWORD_LEN]) {
	for (int i = 0; i < PASSWORD_LEN; i++) {
		buff[i] = -1;
	}
	return;
}

void setup() {
	clear_buffer(password_buffer);
}

void loop() {

}