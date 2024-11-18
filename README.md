# IoT-Based Home Security and Automation System

This project is an IoT-based home security and automation system that leverages an Arduino Mega and ESP8266 for multi-mode door unlocking with fingerprint and keypad security. It uses face detection to alert the user but not to unlock the system. The system integrates Blynk for IoT control, Pushbullet notifications, and Google Sheets logging for entries.

## Features

- **Modes of Unlocking**: 
  - **Fingerprint Unlock Mode**: Uses a fingerprint sensor for authentication.
  - **Keypad Unlock Mode**: Allows unlocking via keypad input.
- **Master Unlock Mode**: Triggered after three failed password attempts. Allows resetting the password via a master signal or special input.
- **Logging**: Logs entries (valid fingerprint detections) to a Google Sheet using a Google Apps Script Web App.
- **LCD Display**: Provides user feedback with a ST7920 LCD display.
- **Pushbullet Notifications**: Sends alerts when an unknown person is detected.

## Components Used

- Arduino Mega
- ESP8266
- Fingerprint Sensor
- ST7920 LCD
- Relay for Door Lock
- Keypad
- Blynk for notifications and IOT

## Software Libraries

- `Keypad.h`
- `SoftwareSerial.h`
- `EEPROM.h`
- `BlynkSimpleEsp8266.h`
- `Adafruit_Fingerprint.h`
- `U8g2lib.h`
- `ESP8266WiFi.h`
- `WiFiClientSecure.h`
- `ESP8266HTTPClient.h`

## Installation and Setup

### Hardware Connections

1. **Keypad Connections**: Connect the keypad rows to Arduino Mega pins 47, 45, 43, 41 and columns to pins 39, 37, 35, 33.
2. **Relay and Switch**: Connect the relay to ESP8266 D1 (Pin 5). Use a switch on D2 (Pin 4) to select the unlocking mode.
3. **Fingerprint Sensor**: Connect to ESP8266 using SoftwareSerial (TX: D3, RX: D4).
4. **LCD Display**: Connect to ESP8266 and initialize with U8g2.

### Software Setup

1. **Install Libraries**: Ensure you have installed the required libraries mentioned above.
2. **Blynk Setup**:
   - Add your Blynk credentials in `auth[]`.
   - Set up a Blynk template for the project.
3. **Wi-Fi Configuration**:
   - Set your Wi-Fi SSID and password.
4. **Google Sheets Logging**:
   - Use the provided Google Apps Script Web App (identified by `GAS_ID`) to log entries.

### Upload the Code

1. Upload the Arduino Mega code using the Arduino IDE.
2. Upload the ESP8266 code using the Arduino IDE with the correct board settings for the ESP8266.

## Usage

- **Default Operation**: On startup, set a new keypad password if not previously configured.
- **Switching Modes**: Use the mode selection switch to toggle between fingerprint and keypad unlocking.
- **Master Unlock**: If the system locks after failed attempts, use the special input '000' on the keypad or send a high signal from the ESP8266 to the `MASTER_UNLOCK_PIN` to reset.

## Troubleshooting

- **Fingerprint Sensor Initialization**: Ensure the connections are correct. The ESP8266 serial connection must be at 57600 baud.
- **ESP8266-Arduino Communication**: Check serial communication settings and ensure they are consistent between devices.

## Future Improvements

- Adding more robust security features.
- Replacing Arduino Mega and ESP-8266 with Raspberry pi for compact Design
- Enhancing the GUI experience.
- Expanding functionality for additional automation tasks.

