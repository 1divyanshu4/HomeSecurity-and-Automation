#include <EEPROM.h>

void setup() {
  Serial.begin(115200); // Start serial communication for debugging

  // Clear EEPROM
  clearEEPROM();

  Serial.println("EEPROM cleared.");
}

void loop() {
  // Nothing to do here
}

void clearEEPROM() {
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.write(i, 0); // Write 0 to each address
  }
}
