#include <Arduino.h>
#include <driver/i2s.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "MyWebPage.h"
#include <ArduinoJson.h>

// I2S configuration
#define I2S_SCK_PIN 13
#define I2S_WS_PIN 14
#define I2S_SD_PIN 15
#define I2S_LR_IO 26
#define I2S_PORT I2S_NUM_0
#define SAMPLE_RATE 16000
#define BUFFER_SIZE 512  // Reduced buffer size
#define led_indicator 5

// WiFi credentials - update these with your network info
const char *ssid = "Radiant TechLab";
const char *password = "@dean1656EE";

// Web server on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Buffer for audio samples
int32_t samples[BUFFER_SIZE];

// Variables for WebSocket rate limiting
unsigned long lastWsSendTime = 0;
const int wsMinSendInterval = 10;  // Send every 50ms
bool wsClientConnected = false;

void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                      AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      wsClientConnected = true;
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      if (ws.count() == 0) {
        wsClientConnected = false;
      }
      break;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(I2S_SCK_PIN, OUTPUT);
  pinMode(I2S_WS_PIN, OUTPUT);
  pinMode(I2S_SD_PIN, INPUT);
  pinMode(I2S_LR_IO, OUTPUT);
  pinMode(led_indicator, OUTPUT);
  delay(1000);
  Serial.println("ESP32 + INMP441 MEMS Microphone Test");

  // Initialize WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Setup I2S
  esp_err_t err;

  // I2S configuration
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,  // Use RIGHT for mono
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = BUFFER_SIZE,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  // I2S pin configuration
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK_PIN,  // SCK
    .ws_io_num = I2S_WS_PIN,    // WS
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD_PIN  // SD
  };

  // Install and start I2S driver
  err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("Failed installing I2S driver: %d\n", err);
    return;
  }

  err = i2s_set_pin(I2S_PORT, &pin_config);
  if (err != ESP_OK) {
    Serial.printf("Failed setting I2S pins: %d\n", err);
    return;
  }

  i2s_start(I2S_PORT);
  Serial.println("I2S driver installed and started");

  // Setup WebSocket
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  // Initialize web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", test_Page);
  });

  // Start server
  server.begin();
  Serial.println("Web server started");
  digitalWrite(I2S_LR_IO, 0);
  digitalWrite(led_indicator, LOW);
  disableCore0WDT();
  disableCore1WDT();
}

void loop() {
  size_t bytes_read = 0;

  // Read data from I2S
  esp_err_t result = i2s_read(I2S_PORT, &samples, sizeof(samples), &bytes_read, portMAX_DELAY);

  if (result == ESP_OK && bytes_read > 0) {
    // Number of samples read
    size_t num_samples = bytes_read / sizeof(int32_t);


    // Send data to WebSocket clients at controlled intervals
    unsigned long currentTime = millis();
    if (wsClientConnected && currentTime - lastWsSendTime >= wsMinSendInterval) {
      // Create a JSON array of samples - use a smaller size
      DynamicJsonDocument jsonDoc(2048);  // Smaller size to avoid memory issues
      JsonArray array = jsonDoc.to<JsonArray>();

      // Add a subset of samples to prevent overwhelming the websocket
      // Take fewer samples and skip more to reduce data rate
      for (int i = 0; i < num_samples; i += 1) {
        array.add(samples[i] >> 16);  // Convert 32-bit to 16-bit
      }

      String jsonString;
      serializeJson(jsonDoc, jsonString);

      // Send the JSON array to all connected clients
      ws.textAll(jsonString);
      lastWsSendTime = currentTime;
    }
  } else if (result != ESP_OK) {
    Serial.printf("I2S read error: %d\n", result);
    delay(1000);
  }

  // Add a small delay to prevent overwhelming the CPU
  delay(2);
}
