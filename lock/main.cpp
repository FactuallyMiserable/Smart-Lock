/*Includes for ESP-NOW communication, based on 2.4Ghz WiFi*/
#include <WiFi.h>
#include <esp_now.h>

/*Includes for DB*/
#include <EEPROM.h>
#include <ArduinoJson.h>

#define EEPROM_SIZE 1024  // Adjust based on storage needs
#define CHUNK_SIZE 256    // Buffer size for reading/writing in chunks

void beginEEPROM() {
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("Failed to initialize EEPROM");
    return;
  }
}

String _readFile() {
  String data = "";
  char buffer[CHUNK_SIZE + 1];
  for (int i = 0; i < EEPROM_SIZE; i += CHUNK_SIZE) {
      for (int j = 0; j < CHUNK_SIZE; j++) {
          char c = EEPROM.read(i + j);
          if (c == '\0') {
              buffer[j] = '\0';
              break;
          }
          buffer[j] = c;
      }
      data += buffer;
      if (strchr(buffer, '\0')) break;
  }
  return data;
}

void _writeFile(String jsonString) {
  if (jsonString.length() >= EEPROM_SIZE) {
      Serial.println("JSON too large for EEPROM");
      return;
  }
  int len = jsonString.length();
  for (int i = 0; i < len; i += CHUNK_SIZE) {
      for (int j = 0; j < CHUNK_SIZE && (i + j) < len; j++) {
          EEPROM.write(i + j, jsonString[i + j]);
      }
  }
  EEPROM.write(len, '\0'); // Null terminator
  EEPROM.commit();
}

DynamicJsonDocument readJSON(int capacity = 2048) {
  String jsonString = _readFile();
  DynamicJsonDocument doc(capacity);
  deserializeJson(doc, jsonString);
  return doc;
}

void writeJSON(DynamicJsonDocument doc) {
  String jsonString;
  serializeJson(doc, jsonString);
  _writeFile(jsonString);
}

void addValue(const String& file, const String& newValue) {
  DynamicJsonDocument doc = readJSON();
  JsonArray arr = doc.to<JsonArray>();
  
  for (JsonVariant value : arr) {
      if (value.as<String>() == newValue) return; // Ensure uniqueness
  }
  
  arr.add(newValue);
  writeJSON(doc);
}

void removeValue(const String& file, const String& valueToRemove) {
  DynamicJsonDocument doc = readJSON();
  JsonArray arr = doc.to<JsonArray>();
  DynamicJsonDocument newDoc(doc.capacity());
  JsonArray newArr = newDoc.to<JsonArray>();
  
  for (JsonVariant value : arr) {
      if (value.as<String>() != valueToRemove) {
          newArr.add(value);
      }
  }
  
  writeJSON(newDoc);
}


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