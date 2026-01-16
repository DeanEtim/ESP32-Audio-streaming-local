/*
* Author: Dean Etim
* Last Modified: 03 / May / 2025

* This is a simple program to illustrate how a PIR sensor works.
* The LCD screen is used to display the sensor data received, and the buzzer is used to 
* make an alert when there is a change in state in the sensor data.

* CONNECTIONS:
* Hardware component          ARDUINO UNO
* Buzzer                      pin 11
* PIR sensor                  pin 6
*/

#include <LiquidCrystal_I2C.h >
#include <Wire.h>

// Create an instance of the class
LiquidCrystal_I2C myScreen(0x27, 16, 2);

// Sensor parameters
#define PIR_SensorPin 6
bool sensorData = false;

// buzzer
const int buzzerPin = 11;

void setup() {
  Serial.begin(115200);
  myScreen.init();
  myScreen.backlight();

  // set the pinMode for the sensor and buzzer
  pinMode(PIR_SensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  myScreen.setCursor(0, 0);
  myScreen.print("PIR Sensor Test");
  myScreen.setCursor(0, 1);
  myScreen.print("Initializing...");
  delay(2000);
}

void loop() {
  sensorData = digitalRead(PIR_SensorPin);
  myScreen.clear();
  myScreen.setCursor(0, 0);
  myScreen.print("Sensor Data: ");
  myScreen.print(sensorData);

  if (sensorData == true) {
    digitalWrite(buzzerPin, HIGH);
    Serial.println("Motion Detected");
    delay(100);
  }

  else {
    digitalWrite(buzzerPin, LOW);
    Serial.println("No Motion Detected");
    delay(100);
  }
  delay(200);
}
