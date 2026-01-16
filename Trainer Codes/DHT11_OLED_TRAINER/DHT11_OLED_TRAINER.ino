/*
  Simple DHT11 Temperature & Humidity Monitor with Fan Control
  - Displays temperature and humidity on 0.96" OLED
  - Controls a 220V AC fan via ACTIVE LOW relay when temp > 30°C
  
  Hardware:
  - Arduino Uno
  - DHT11 temperature/humidity sensor on pin 4
  - 0.96" OLED display (I2C)
  - Active LOW Relay module connected to pin 8
  - 220V AC fan connected to relay
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// Define pins
#define DHTPIN 4
#define RELAY_PIN 8
#define DHTTYPE DHT11

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

// Initialize objects
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DHT dht(DHTPIN, DHTTYPE);

// Temperature threshold for fan control
#define TEMP_THRESHOLD 30.0

void setup() {
  Serial.begin(9600);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  // Initialize DHT sensor
  dht.begin();

  // Set up relay pin
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // For active LOW relay, set HIGH to ensure fan is initially OFF

  // Display startup message
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println(F("TEMP CTRL"));
  display.setTextSize(1);
  display.setCursor(25, 40);
  display.println(F("System Ready"));
  display.display();
  delay(1000);
}

void loop() {
  // Read sensor data
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if reading was successful
  if (isnan(humidity) || isnan(temperature)) {
    displayError();
    return;
  }

  // Control fan based on temperature - INVERTED LOGIC for active LOW relay
  if (temperature > TEMP_THRESHOLD) {
    digitalWrite(RELAY_PIN, LOW);  // For active LOW relay, LOW turns the fan ON
  } else {
    digitalWrite(RELAY_PIN, HIGH);  // For active LOW relay, HIGH turns the fan OFF
  }

  // Update display
  updateDisplay(temperature, humidity);

  // Print values to serial monitor
  Serial.print(F("Temperature: "));
  Serial.print(temperature);
  Serial.print(F("°C, Humidity: "));
  Serial.print(humidity);
  Serial.println(F("%"));
  delay(500);  // Update every 0.5 seconds
}

void displayError() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("SENSOR ERROR"));
  display.setCursor(0, 20);
  display.println(F("Check connections"));
  display.display();
  delay(1000);
}

void updateDisplay(float temperature, float humidity) {
  display.clearDisplay();

  // Title
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Temperature Humidity"));
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);

  // Temperature
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print(F("T: "));
  display.print(temperature, 1);
  display.print(F(" C"));

  // Humidity
  display.setCursor(0, 45);
  display.print(F("H: "));
  display.print((int)humidity);
  display.print(F(" %"));

  display.display();
}