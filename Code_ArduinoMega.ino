#include <Keypad.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>  // Include EEPROM library

// Keypad configuration
const byte ROWS = 4;  // Four rows
const byte COLS = 4;  // Four columns
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 47, 45, 43, 41 };  // Row pin connections
byte colPins[COLS] = { 39, 37, 35, 33 };  // Column pin connections

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Password management variables
String newPassword = "";
String enteredPassword = "";
const byte maxPasswordLength = 6;
bool isPasswordSet = false;
bool isMasterUnlockMode = false;  // New variable to track Master Unlock mode
byte attemptCount = 0;
const int masterUnlockPin = 31;  // Pin number for the signal from ESP (D0)


// Serial communication
SoftwareSerial espSerial(18, 19);  // RX, TX for ESP8266
const int passwordAddress = 0;     // EEPROM address for password storage

// Switch configuration
const int switchPin = 27;         // Pin number for the switch
bool previousSwitchState = HIGH;  // Track the previous state of the switch

void setup() {
  Serial.begin(115200);     // Serial communication with computer
  espSerial.begin(115200);  // Communication with ESP8266

  // Setup pin modes
  pinMode(masterUnlockPin, INPUT);  // Set the master unlock pin as input
  pinMode(switchPin, INPUT);

  loadPasswordFromEEPROM();  // Load password from EEPROM on startup

  if (isPasswordSet) {
    espSerial.println("Password Loaded. | Enter Password");
  } else {
    espSerial.println("Set New Password");  // Prompt to set a new password
  }
}

void loop() {

  // Check the switch state
  bool currentSwitchState = digitalRead(switchPin);

  // Check for transition from high to low (fingerprint mode to keypad mode)
  if (currentSwitchState == LOW && previousSwitchState == HIGH) {
    resetEnteredPassword();  // Reset the entered password when switching back to keypad mode
    if (isPasswordSet) {
      espSerial.println("Enter Password");
    }
  }

  // Update the previous state
  previousSwitchState = currentSwitchState;

  // Only process keypad input if in keypad mode (switch is low)
  if (currentSwitchState == LOW) {
    char key = keypad.getKey();  // Get the pressed key from the keypad

    if (key) {
      if (!isPasswordSet) {
        handlePasswordSetting(key);  // Handle password setting process
      } else if (!isMasterUnlockMode) {
        handlePasswordEntry(key);  // Handle password entry process if not in Master Unlock mode
      }
    }
  }
}

// Function to handle setting a new password
void handlePasswordSetting(char key) {
  espSerial.println("Set New Password: |");
  if (((key >= '0' && key <= '9') || (key >= 'A' && key <= 'D')) && newPassword.length() < maxPasswordLength) {
    newPassword += key;  // Add key to new password
    espSerial.print("New Password: |");
    espSerial.println(newPassword);  // Send new password to ESP8266 for display
  }

  if (key == '#') {  // Confirm password
    isPasswordSet = true;
    savePasswordToEEPROM(newPassword);  // Save the password to EEPROM
    espSerial.println("Password Set | Enter Password");
    newPassword.trim();  // Remove leading and trailing whitespace
  }
}

// Function to handle password entry
void handlePasswordEntry(char key) {
  if (((key >= '0' && key <= '9') || (key >= 'A' && key <= 'D')) && enteredPassword.length() < maxPasswordLength) {
    enteredPassword += key;  // Add key to entered password
    espSerial.print("Entered Password: |");
    espSerial.println(enteredPassword);  // Send entered password to ESP8266 for display
  }

  if (key == '#') {
    checkPassword();  // Check if entered password is correct
  }

  // Reset entered password if '*' is pressed
  if (key == '*') {
    resetEnteredPassword();
  }
}

// Function to check the entered password
void checkPassword() {
  if (enteredPassword == newPassword) {
    espSerial.println("Correct Password!");
    delay(500);
    espSerial.println("Welcome|Door Unlocked");
    delay(3000);             // Wait for 3 seconds before resetting
    resetEnteredPassword();  // Reset entered password
    attemptCount = 0;        // Reset attempt count
  } else {
    handleWrongPassword();  // Handle wrong password attempt
  }
}

// Function to handle wrong password attempts
void handleWrongPassword() {
  attemptCount++;

  if (attemptCount == 1) {
    espSerial.println("Wrong Password, | 2 chances left.");
    resetEnteredPassword();
    delay(500);
    espSerial.println("Enter Password.");

  } else if (attemptCount == 2) {
    espSerial.println("Wrong Password, | 1 chance left.");
    resetEnteredPassword();
    delay(500);
    espSerial.println("Enter Password.");

  } else if (attemptCount >= 3) {
    enterMasterUnlockMode();  // Enter Master Unlock mode after 3 failed attempts
  }
}

// Function to enter Master Unlock mode
void enterMasterUnlockMode() {
  isMasterUnlockMode = true;  // Set flag to indicate Master Unlock mode
  espSerial.println("No more tries|MasterLocked");

 // Prompt the user to enter '000' to reset the password or check for the master unlock signal from pin 31
while (isMasterUnlockMode) {
    espSerial.println("Master Unlock | Required");  // Prompt message

    // Check if the signal on masterUnlockPin (pin 31) is HIGH
    int signal = digitalRead(masterUnlockPin);
    if (signal == HIGH) {
        espSerial.println("MasterUnlock Triggered| by Blynk"); // Indicate it was triggered by pin
        resetPassword();                              // Reset the password
        espSerial.println("Set New Password: |");     // Prompt to set a new password
        isMasterUnlockMode = false;                   // Exit Master Unlock mode
        break;                                        // Break out of the loop
    }

    // Check for '000' input from the keypad
    if (checkResetPassword()) {
        espSerial.println("MasterUnlock Triggered| by Input '000'"); // Indicate it was triggered by keypad input
        resetPassword();                              // Reset password
        espSerial.println("Set New Password: |");     // Prompt to set a new password
        isMasterUnlockMode = false;                   // Exit Master Unlock mode
    }
}
}

// Function to exit Master Unlock mode
void exitMasterUnlockMode() {
  isMasterUnlockMode = false;
  espSerial.println("Master Unlock Mode Exited");
  attemptCount = 0;  // Reset attempt count
  resetPassword();
}


// Function to check for '000' entry to reset the password
bool checkResetPassword() {
  String zeros = "";
  for (int i = 0; i < 3; i++) {
    char key = keypad.getKey();  // Use getKey() for non-blocking checks
    while (!key) {               // Loop until a key is pressed
      key = keypad.getKey();
    }

    if (key == '0') {
      zeros += key;  // Add zero to the string
    } else {
      return false;  // If any key is not '0', exit
    }
  }
  return (zeros == "000");  // Return true if '000' was entered
}

// Function to reset the password
void resetPassword() {
  newPassword = "";        // Clear the new password
  isPasswordSet = false;   // Mark the password as not set
  attemptCount = 0;        // Reset the attempt count
  resetEnteredPassword();  // Clear the entered password for security
}

// Function to reset the entered password
void resetEnteredPassword() {
  enteredPassword = "";  // Clear entered password
}

// Function to save the password to EEPROM
void savePasswordToEEPROM(String password) {
  for (int i = 0; i < maxPasswordLength; i++) {
    if (i < password.length()) {
      EEPROM.write(passwordAddress + i, password[i]);  // Write each character of the password
    } else {
      EEPROM.write(passwordAddress + i, '\0');  // Null terminate if password is shorter
    }
  }
}

// Function to load the password from EEPROM
void loadPasswordFromEEPROM() {
  char password[maxPasswordLength + 1];  // +1 for null terminator
  for (int i = 0; i < maxPasswordLength; i++) {
    password[i] = EEPROM.read(passwordAddress + i);
  }
  password[maxPasswordLength] = '\0';          // Null terminate the string
  newPassword = String(password);              // Assign read password to newPassword
  isPasswordSet = (newPassword.length() > 0);  // Set isPasswordSet flag based on read password
}
