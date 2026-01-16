// Feedback project with bluetooth
#include <SoftwareSerial.h>
#include <Servo.h>

// Buetooth object
const int txd = 2;  // Connect bluetooth's RXD pin
const int rxd = 3;  // Connect bluetooth's TXD pin
SoftwareSerial myBluetooth(rxd, txd);
String incomingData, fromSerial;

// create an object for servo-motor
Servo myServo;
const int servoPin = 9;
int servoAngle = 0;
const int relayPin = 12;

void setup() {
  myBluetooth.begin(9600);  // begin bluetooth communication
  Serial.begin(9600);       //begin Serialcommunication

  // initialize servo motor
  pinMode(servoPin, OUTPUT);
  myServo.attach(servoPin);
  myServo.write(servoAngle);

  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);

}  // end setup

void loop() {
  int prompt = 0;

  // fetching commands
  if (myBluetooth.available() > 0) {
    incomingData = myBluetooth.readString();
    Serial.print("Incoming Data: ");
    Serial.println(incomingData);  //Print it on the Serial monitor

    // Control with commands from bluetooth module
    if (incomingData == "light on" || incomingData == "Light on" || incomingData == "lights on") {
      prompt = 1;
    }  // end if

    if (incomingData == "light off" || incomingData == "lights off" || incomingData == "Light off") {
      prompt = 2;
    }  // end if

    if (incomingData == "Go Up" || incomingData == "go up" || incomingData == "Go Up") {
      prompt = 3;
    }  // end if

    if (incomingData == "Down " || incomingData == "down" || incomingData == "down ") {
      prompt = 4;
    }  // end if
  }    // end if

  switch (prompt) {
    case 1:
      digitalWrite(relayPin, LOW);
      myBluetooth.println("Light bulb is on!");
      break;

    case 2:
      digitalWrite(relayPin, HIGH);
      myBluetooth.println("Light bulb is off!");
      break;

    case 3:
      myServo.write(90);
      delay(10);
      break;

    case 4:
      myServo.write(0);
      delay(10);
      break;

    case 5:
      // doSomethingElse()
      break;

    default:
      break;
  }
}