#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

#define FINGERPRINT_SENSOR_TX 12
#define FINGERPRINT_SENSOR_RX 14

SoftwareSerial mySerial(FINGERPRINT_SENSOR_RX, FINGERPRINT_SENSOR_TX);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
    Serial.begin(115200);
    mySerial.begin(57600); // Initialize software serial for fingerprint sensor

    // Initialize Fingerprint Sensor
    Serial.println("Initializing fingerprint sensor...");
    finger.begin(57600);
    if (finger.verifyPassword()) {
        Serial.println("Fingerprint sensor initialized successfully!");
    } else {
        Serial.println("Failed to initialize fingerprint sensor. Check connections.");
        while (1);
    }

    // Delete all fingerprints
    deleteAllFingerprints();
}

void loop() {
    // Do nothing
}

void deleteAllFingerprints() {
    Serial.println("Deleting all fingerprints from the sensor...");

    uint8_t result = finger.emptyDatabase();
    
    if (result == FINGERPRINT_OK) {
        Serial.println("All fingerprints deleted successfully!");
    } else {
        Serial.println("Failed to delete fingerprints.");
        Serial.print("Error code: ");
        Serial.println(result);
    }
}

