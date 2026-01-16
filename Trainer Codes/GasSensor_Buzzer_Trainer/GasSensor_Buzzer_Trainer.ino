#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize the LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int mq2Pin = A0;    // Gas Sensor pin
const int buzzerPin = 8;  // Buzzer pin

// Threshold value
const int gasThreshold = 300;  // Can be adjusted

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Print initial message
  lcd.setCursor(0, 0);
  lcd.print("Gas Detector");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
}

void loop() {
  int gasValue = analogRead(mq2Pin);  // Read the analog value from the sensor

  // Display the raw value on serial monitor
  Serial.print("Gas Sensor Value: ");
  Serial.println(gasValue);

  // Clear the LCD second line
  lcd.setCursor(0, 1);
  lcd.print("      ");

  // Update LCD based on gas reading
  lcd.setCursor(0, 1);
  if (gasValue > gasThreshold) {
    lcd.print("Gas Detected!");
    warningBeep();
    lcd.setCursor(0, 0);
    lcd.print("WARNING!        ");
  }

  else {
    lcd.print("Air Clean      ");
    lcd.setCursor(0, 0);
    lcd.print("Monitoring...   ");
  }
  delay(500);  // Wait for half a second before next reading
}  // end main loop

void warningBeep(void) {
  digitalWrite(buzzerPin, HIGH);
  delay(300);
  digitalWrite(buzzerPin, LOW);
  delay(300);
}  // end function