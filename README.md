# ESP8266-Based Home Security and Automation System

This project consists of a Home Security System and an Automation System using the ESP8266 microcontroller. The project is divided into two main parts:

1. **Home Security System** - Implements a fingerprint-based and keypad-based locking mechanism, with features like master lock mode, password reset, and logging.
2. **Automation System** - Adds automation features to control home devices remotely using Blynk.

## Table of Contents

- [Overview](#overview)
- [Components Required](#components-required)
- [Hardware Setup](#hardware-setup)
- [Software Installation](#software-installation)
- [Code Structure](#code-structure)
  - [Home Security](#home-security)
  - [Automation](#automation)
- [Features](#features)
- [Usage](#usage)
- [Troubleshooting](#troubleshooting)
- [Credits](#credits)

## Overview

This project integrates a biometric fingerprint sensor and a keypad for home security, along with IoT capabilities for automation using the ESP8266. The system can unlock a door, enter master lock mode after incorrect attempts, and communicate over Wi-Fi for remote access.

## Components Required

- ESP8266
- Arduino
- Fingerprint Sensor
- Keypad
- Relay Module
- Blynk IoT Platform

## Hardware Setup

1. Connect everything as per the Circuit Diagram
4. Set up the ESP8266 for Wi-Fi connectivity to interface with the Blynk app.
5. Make sure to configure the pins according to the code provided.

## Software Installation

### 1. Arduino IDE
- Download and install the [Arduino IDE](https://www.arduino.cc/en/software).
- Install the ESP8266 board manager using the Boards Manager in the IDE.
- Install necessary libraries:
  - `Adafruit Fingerprint Sensor Library`
  - `Keypad Library`
  - `ESP8266WiFi`
  - `Blynk Library`
  - Other necessary libraries for Wi-Fi connectivity and HTTP handling are Already installed.

### 2. Blynk Setup
- Create a Blynk project in the Blynk app.
- Add the appropriate widgets for controlling and monitoring your home system.
- Use the authentication token provided by Blynk in your code.


## Code Structure
This Project Heavily relies on Serial Communication Between Arduino Mega and ESP8266 So It'll be helpfull for those who want to learn Communication Between two microcontrollers

### Home Security

#### **`Code_ES8266.ino`**
- Implements Wi-Fi connectivity using the `connectToWiFi()` function.
- Manages fingerprint sensor initialization and user enrollment.
- Controls the locking mechanism with a relay when fingerprint or password is authenticated.
- Integrates both fingerprint and keypad for multi-factor authentication. 

#### **`Code_ArduinoMega.ino`**
- Handles the Keypad unlocking part
- Handles the Master Lock mode, which activates after three wrong password attempts.
- Allows reset of the master lock mode via Blynk or Pushbullet commands.


### Automation

#### **`Automation.ino`**
- Connects the ESP8266 to Blynk for home automation.
- Controls various devices using Blynk's virtual datastream.
- Monitors the status of devices and allows remote control from a smartphone.

## Features

### Home Security
- **Fingerprint Authentication**: Unlock the door using registered fingerprints.
- **Keypad Authentication**: Enter a password to unlock the door.
- **Google Sheets Logging**: Every entry is logged there 
- **Master Lock Mode**: Activates after multiple wrong attempts, requiring a reset through Blynk.
- **Remote Control**: Use Blynk or Pushbullet to reset the lock or manage settings.

### Automation
- **Remote Device Control**: Manage home devices from a smartphone using the Blynk app.
- **IoT Integration**: Seamlessly connect the ESP8266 to the Blynk IoT platform for smart automation.

## Usage

1. Upload the **Home Security** and **Automation** code separately to the ESP8266 using the Arduino IDE.
2. Configure the Blynk and Pushbullet settings in the code.
3. Use the keypad or fingerprint sensor to control the locking mechanism.
4. Use the Blynk app for remote device control and to manage the lock.

## Troubleshooting

- **Wi-Fi Connectivity Issues**: Check the Wi-Fi credentials and ensure the ESP8266 is in range.
- **Fingerprint Sensor Not Responding**: Ensure proper connections and that the sensor library is correctly installed.
- **Relay Not Activating**: Double-check the pin configuration in the code.
- **Master Lock Not Resetting**: Ensure Blynk datastream V0 is configured properly and Pushbullet communication is set up.

## Credits

This project was developed by Divyanshu Lakra & Gunupuru Tejasri. Special thanks to the open-source community and libraries used in this project.

