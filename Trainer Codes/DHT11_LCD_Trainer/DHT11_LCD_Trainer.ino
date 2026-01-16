#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// Pin Definitions
#define DHT_PIN 4       // DHT11 data pin
#define RELAY_PIN 8    // Relay control pin
#define DHT_TYPE DHT11  // DHT sensor type

// Constants
#define TEMP_THRESHOLD 30.0  // Temperature threshold for fan control (Celsius)

// Initialize DHT sensor
DHT dht(DHT_PIN, DHT_TYPE);

// Initialize LCD with I2C address
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variables
float temperature = 0.0;
float humidity = 0.0;
bool fanStatus = false;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial.println("ESP32 DHT11 Temperature Control System");

  // Initialize DHT sensor
  dht.begin();

  // Initialize relay pin as output
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, !LOW);  // Initially turn off the fan

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Display welcome message
  lcd.setCursor(0, 0);
  lcd.print("Temp Control Sys");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  delay(1000);  // Wait for 2 seconds
}

void loop() {
  // Read temperature and humidity from DHT11
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  // Check if reading failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sensor Error!");
    return;
  }

  // Print readings to serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Control the fan based on temperature threshold
  if (temperature >= TEMP_THRESHOLD && !fanStatus) {
    digitalWrite(RELAY_PIN, !HIGH);  // Turn on the fan
    fanStatus = true;
    Serial.println("Fan turned ON");
  } else if (temperature <= TEMP_THRESHOLD && fanStatus) {
    digitalWrite(RELAY_PIN, !LOW);  // Turn off the fan
    fanStatus = false;
    Serial.println("Fan turned OFF");
  }

  // Display temperature, humidity and fan status on LCD
  lcd.clear();

  // First row: Temperature and humidity
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C ");
  lcd.print("H:");
  lcd.print(humidity, 1);
  lcd.print("%");

  // Second row: Fan status
  lcd.setCursor(0, 1);
  lcd.print("Fan: ");
  lcd.print(fanStatus ? "ON" : "OFF");

  // Wait before taking the next reading
  delay(500);
}