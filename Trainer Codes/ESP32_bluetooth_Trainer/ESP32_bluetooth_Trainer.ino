// Include the necessary libraries
#include "BluetoothSerial.h"
#include <ESP32Servo.h>

// Servo Object
Servo servo1;
#define servoPin 15

// Password
#define USE_PIN            // Comment this out to disable PIN use during pairing.
const char *pin = "1234";  // You can change this to a more secure PIN.

// Bluetooth Object
BluetoothSerial myBluetooth;
String device_name = "Embedded_Systems Trainer";
String incomingMessage = "";

// Checking if the Buetooth feature is available on the ESP32 board you're using
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

void setup() {
  Serial.begin(115200);
  myBluetooth.begin(device_name);  //Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
#ifdef USE_PIN
  myBluetooth.setPin(pin);
  Serial.println("Using PIN");
#endif
  servo1.attach(servoPin);
}  // end setup

void loop() {
  if (myBluetooth.available()) {
    incomingMessage = myBluetooth.readString();
    Serial.println(incomingMessage);

    if (incomingMessage == "sweep right" || incomingMessage == "Sweep right" || "r") {
      sweep_Right();
      Serial.println("Sweeping right");
      myBluetooth.println("Doing just that...");
    }  // end if

    else if (incomingMessage == "sweep left" || incomingMessage == "Sweep left" || incomingMessage == "l") {
      sweep_Left();
      Serial.println("Sweeping left");
      myBluetooth.println("Doing just that...");
    }  // end else if
  }    // end if
  delay(20);
}

void sweep_Right() {
  unsigned int p = 0;
  for (p = 0; p <= 180; p++) {
    servo1.write(p);
    delay(10);
  }  // end for
  incomingMessage = "";
}  // end sweep_Right

void sweep_Left() {
  unsigned int p = 180;
  for (p = 180; p >= 0; p--) {
    servo1.write(p);
    delay(10);
  }  // end for
  incomingMessage = "";
}  // end sweep_Left
