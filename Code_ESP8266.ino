// Blynk credentials
#define BLYNK_TEMPLATE_ID "TMPL3Z2fgiR0X"
#define BLYNK_TEMPLATE_NAME "Home Security"


#include <BlynkSimpleEsp8266.h>
#include <Adafruit_Fingerprint.h> 
#include <U8g2lib.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>

char auth[] = "_vV1qCx47WYUvhaoXlyYYlXCzRnoVaMH";

// Pin Definitions
#define FINGERPRINT_SENSOR_TX 0 // D3
#define FINGERPRINT_SENSOR_RX 2 // D4
#define RELAY_PIN 5  // D1 Pin connected to the relay
#define MODE_PIN 4   // D2 Pin to select mode (Low: Fingerprint, High: Keypad)
#define MASTER_UNLOCK_PIN 16 // D0

SoftwareSerial mySerial(FINGERPRINT_SENSOR_RX, FINGERPRINT_SENSOR_TX);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Initialize U8g2 for ST7920 LCD with hardware SPI
U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 12, /* data=*/ 13, /* CS=*/ 15 /* reset=*/ );

// Wi-Fi credentials
const char* ssid = "Redmi"; 
const char* password = "72580306"; 

const char* host = "script.google.com";
const int httpsPort = 443;
WiFiClientSecure client; 
String GAS_ID = "AKfycbz5ZtGUYEMDNxfhyCmxxSzDiTWvScMR7lWNN9YFG5btrLJ9q3dK4QgbvWpTDqIXmShQ"; 

uint8_t id;

SoftwareSerial megaSerial(3, 1); // RX, TX for communication with Arduino Mega

void setup() {
    Serial.begin(115200);
    mySerial.begin(57600); // Initialize software serial for fingerprint sensor
    megaSerial.begin(115200);  // Serial communication with Arduino Mega

 // Blynk initialization
    Blynk.begin(auth, ssid, password);

    u8g2.begin();  // Initialize the LCD
    u8g2.clearBuffer();  // Clear the buffer
    u8g2.setFont(u8g2_font_ncenB08_tr);  // Set font

    // Initialize Fingerprint Sensor
    Serial.println("Initializing fingerprint sensor...");
    finger.begin(57600);
    if (finger.verifyPassword()) {
        Serial.println("Fingerprint sensor initialized successfully!");
         u8g2.drawStr(0,10,"Fingerprint Initialized");  // Display the message on the first line
    } else {
        Serial.println("Failed to initialize fingerprint sensor. Check connections.");
        while (1);
    }

    // Initialize LED and relay pins
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(MODE_PIN, INPUT); // Mode selection pin
    digitalWrite(RELAY_PIN, HIGH);
    pinMode(MASTER_UNLOCK_PIN, OUTPUT);
    digitalWrite(MASTER_UNLOCK_PIN, LOW); // Default to LOW

    connectToWiFi(); // Connect to Wi-Fi
}

void loop() {
    Blynk.run(); // Handle Blynk communication
    int mode = digitalRead(MODE_PIN); // Read the state of MODE_PIN (D2)

    if (mode == HIGH) {
        // Fingerprint Unlock Mode
        Serial.println("Fingerprint Unlock Mode");
        detectFingerprint();
    } else {
        // Keypad Unlock Mode
        Serial.println("Keypad Unlock Mode");
        checkKeypadInput();
    }
    delay(500); // Delay before the next loop iteration
}

// Function to detect fingerprint
void detectFingerprint() {
    Serial.println("\nWaiting for a valid fingerprint...");
    displayMessages("Place the Finger on ","Fingerprint Sensor");
    delay(1000);
    uint8_t id = getFingerprintID();

    if (id != -1 && id != 255) {
        Serial.print("Fingerprint ID: ");
        displayMessages("Valid Fingerprint","Detected");
        Serial.println(id);
        logEntry(id);  // Log the entry to Google Sheets
    } else {
        Serial.println("No valid fingerprint detected or ID is 255.");
        displayMessages("No Valid ID, Try Again","");
    }
}

// Function to check keypad input (communicates with Arduino Mega)
void checkKeypadInput() {
    if (megaSerial.available()) {
        delay(10); // Brief delay to ensure message is fully received
        String message = megaSerial.readStringUntil('\n');  // Read the message from Arduino Mega
        Serial.println("Message from Mega: " + message);  // Debugging message

        // Split the message into two lines using a specific delimiter, e.g., "|"
        int delimiterIndex = message.indexOf('|'); // Using '|' as a delimiter
        String line1;
        String line2;

        // Check if the delimiter was found
        if (delimiterIndex != -1) {
            line1 = message.substring(0, delimiterIndex);  // Get the first part before the delimiter
            line2 = message.substring(delimiterIndex + 1);  // Get the second part after the delimiter
        } else {
            line1 = message;  // If no delimiter, use the entire message as line1
            line2 = "";       // Set line2 to empty
        }

        displayMessages(line1, line2);  // Display both lines on the LCD

        // Unlock door if the message indicates so
        if (line1 == "Welcome") {
            logEntry(128);
        }
    }
}

// Function to log fingerprint entry to Google Sheets
void logEntry(int userId) {
    Serial.println("==========");
    Serial.print("Connecting to ");
    Serial.println(host);

    if (!client.connect(host, httpsPort)) {
        Serial.println("Connection failed");
        displayMessages("Log Failed, Try Again","");
        return;
    } else {
        Serial.println("Door unlocked");
        unlockDoor();
    }

    String user_ID = String(userId);
    String url = "/macros/s/" + GAS_ID + "/exec?user=" + user_ID;

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
        "Host: " + host + "\r\n" +
        "User-Agent: BuildFailureDetectorESP8266\r\n" +
        "Connection: close\r\n\r\n");

    Serial.println("Request sent");
    Serial.println("Closing connection");
    Serial.println("==========");
}

// Function to unlock the door
void unlockDoor() {
    digitalWrite(RELAY_PIN, LOW);  // Unlock the door
    delay(3000);  // Keep the door unlocked for 3 seconds
    digitalWrite(RELAY_PIN, HIGH);  // Lock the door again
    displayMessages("Log Successfull","Door Unlocked");
}

// Function to get Fingerprint ID
uint8_t getFingerprintID() {
    uint8_t p = finger.getImage();
    switch (p) {
        case FINGERPRINT_OK:
            Serial.println("Fingerprint image taken.");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.println("No finger detected.");
            return -1;
        default:
            return -1;
    }

    p = finger.image2Tz();
    if (p == FINGERPRINT_OK) {
        p = finger.fingerFastSearch();
        if (p == FINGERPRINT_OK) {
            return finger.fingerID;
        }
    }
    return -1;
}

// Function to connect to Wi-Fi
void connectToWiFi() {
    WiFi.begin(ssid, password);
    Serial.println("Connecting to Wi-Fi...");
    displayMessages("Connecting to Wi-Fi","");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("Connected!");
    displayMessages("Connected","");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    client.setInsecure();  // Disable SSL verification for the client
}

// Blynk function to monitor V0
BLYNK_WRITE(V0) {
    int pinValue = param.asInt(); // Get the value of V0
    if (pinValue == 1) {
        digitalWrite(MASTER_UNLOCK_PIN, HIGH); // Signal to Arduino to exit Master Unlock
        delay(500);
        digitalWrite(MASTER_UNLOCK_PIN, LOW); // Signal to Arduino to exit Master Unlock

    } else {
        digitalWrite(MASTER_UNLOCK_PIN, LOW);
    }
}

// Function to display messages on LCD
void displayMessages(String line1, String line2) {
    u8g2.clearBuffer();  // Clear the buffer once

    // Get the widths of both messages
    int line1Width = u8g2.getStrWidth(line1.c_str());
    int line2Width = u8g2.getStrWidth(line2.c_str());

    // Calculate the x positions to center the text
    int xPosLine1 = (u8g2.getDisplayWidth() - line1Width) / 2;
    int xPosLine2 = (u8g2.getDisplayWidth() - line2Width) / 2;

    // Draw the centered messages
    u8g2.drawStr(xPosLine1, 20, line1.c_str());  // Display the message on the first line
    u8g2.drawStr(xPosLine2, 40, line2.c_str());  // Display the message on the second line

    u8g2.sendBuffer();  // Send buffer to display
}

