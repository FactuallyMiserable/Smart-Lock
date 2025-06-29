# 🔐 Smart Lock (ESP8266) — Work in Progress - Automatically genreated readme. Read with caution.

This is a lightweight Wi-Fi-based smart lock controller using an ESP8266 board.  
It exposes simple HTTP endpoints to **lock** and **unlock** a mechanism connected to GPIO.

🧪 This project is **unfinished**, but already includes a working HTTP server and GPIO control logic.

---

## 📁 Files Overview

```
Smart-Lock/
├── Smart_Lock.ino     # Arduino sketch - connects Wi-Fi & starts server
├── main.cpp           # Implements HTTP server routes and logic
├── main.hpp           # Function declarations used in main.cpp
├── pinout.hpp         # Defines GPIO pin for lock control
```

---

## ⚙️ How It Works

1. ESP8266 connects to a Wi-Fi network using credentials hardcoded in `Smart_Lock.ino`.
2. A web server is started on port 80.
3. Routes:
   - `/` — returns basic HTML message: `"Hello from the esp!"`
   - `/lock` — pulls `LOCK_PIN` **LOW**
   - `/unlock` — pulls `LOCK_PIN` **HIGH**

The server parses and stores all HTTP headers (currently unused) in a `std::map`.

---

## 🔌 Hardware Setup

In `pinout.hpp`:

```cpp
#define LOCK_PIN 13
```

Connect your lock mechanism (relay, servo, etc.) to **GPIO 13**.

🛠️ Note: You may need a transistor or relay to safely control high-current hardware.

---

## 🌐 API Reference

All routes use `GET` requests:

| Endpoint   | Description             | Effect                      |
|------------|-------------------------|-----------------------------|
| `/`        | Returns "Hello" message | Basic server test           |
| `/lock`    | Activates lock          | Sets `LOCK_PIN` to `LOW`    |
| `/unlock`  | Deactivates lock        | Sets `LOCK_PIN` to `HIGH`   |

📎 Headers are stored in a map: `std::map<String, String> requestHeaders` (future auth idea).

---

## 🚀 How to Use

### ✅ Prerequisites

- ESP8266 board (e.g. NodeMCU)
- Arduino IDE with ESP8266 board support
- Required libraries:
  - `ESP8266WiFi.h`
  - `ESP8266WebServer.h`

### 🔌 Setup

1. Clone this repo:
   ```bash
   git clone https://github.com/FactuallyMiserable/Smart-Lock.git
   ```
2. Open `Smart_Lock.ino` in Arduino IDE.
3. Edit your Wi-Fi credentials:
   ```cpp
   WiFi.begin("YOUR_SSID", "YOUR_PASSWORD");
   ```
4. Upload to your ESP8266.

---

## 📌 Planned Features

- [ ] Auth via token or header key
- [ ] JSON responses instead of plain text
- [ ] Lock state persistence (EEPROM or file)
- [ ] Cross-device access via web client or mobile UI
- [ ] OTA (Over-the-Air) updates

---

## ⚠️ Notes

- No security is implemented yet — anyone on the network can access the lock endpoints.
- This is an MVP meant for local testing and expansion.

---

## 🧑‍💻 Author

Made by [FactuallyMiserable](https://github.com/FactuallyMiserable)  
Pull requests, issues, and feedback welcome!

---

## 📄 License

To be added.
