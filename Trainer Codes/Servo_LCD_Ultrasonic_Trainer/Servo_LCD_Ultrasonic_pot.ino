/*
* Author: Engr. Dean Etim
* Last Updated: 01/05/2025

* This simple program reads analog sensor data from
* a potentiometer attached to pin A6
* and uses it to control the angle of rotation of a servo motor attached to pin D9.
* An ultrasonic sensor is also used to calculate the distance of an object.
* The Object distance and Servo position is displayed on the LCD screen in real-time
*
*/
#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
unsigned long then = 0;
unsigned long interval = 500;

// create an object to represent the screen
LiquidCrystal_I2C myScreen(0x27, 16, 2);

// create an object for servomotor
Servo myServo;
const int servoPin = 9;
int servoAngle = 0;

// Ultrasonic sensor
int trigPin = 3;
int echoPin = 4;
float proximity = 0.0;

//potentiometer
#define potPin A6
int rawValue = 0;

void setup() {
  // initialize servo motor
  pinMode(servoPin, OUTPUT);
  myServo.attach(servoPin);
  myServo.write(servoAngle);

  // setup the pinModes for the ultrasonic sensor pins
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);

  // initialize LCD screen
  myScreen.init();
  myScreen.backlight();

  // Display welcome message
  myScreen.print("Hello World!");
  delay(1000);
  myScreen.clear();
  myScreen.print("Just a moment...");
  delay(2000);
}  //  end setup

void loop() {
  unsigned long now = millis();

  // read analog input
  rawValue = analogRead(potPin);
  servoAngle = map(rawValue, 0, 1023, 0, 180);
  proximity = measureDistance();  // calculate distance

  if (now - then >= interval) {
    // display Servo position
    myScreen.clear();
    myScreen.setCursor(0, 0);
    myScreen.print("Servo Position: ");
    myScreen.print(servoAngle);
    myScreen.println("degrees");

    //display object distance on a new line
    myScreen.setCursor(0, 1);
    myScreen.print("Object distance: ");
    myScreen.print(proximity, 2);
    myScreen.println("cm");
    then = now;
  }  // end if

  // control servo with potentiometer
  myServo.write(servoAngle);
  delay(10);
}  //end loop


float measureDistance(void) {
  float distance = 0.0;
  float duration = 0.0;

  // send out ultrasonic pulse signal
  digitalWrite(trigPin, LOW);
  delay(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Calculate distance
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.0171;
  return distance;
}  // end function
