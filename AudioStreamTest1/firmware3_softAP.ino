#include <Arduino.h>
#include <driver/i2s.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "MyWebPage.h"

// I2S configuration
#define I2S_SCK_PIN 13
#define I2S_WS_PIN 14
#define I2S_SD_PIN 15
#define I2S_LR_IO 26
#define I2S_PORT I2S_NUM_0
#define SAMPLE_RATE 44100
#define BUFFER_COUNT 10
#define BUFFER_SIZE 1024
#define led_indicator 5

// Soft AP Configuration
const char *ap_ssid = "NoviBell Audio Stream";
const char *ap_password = "26461656";

// Web server on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Buffer for audio sample
int16_t audioBuffer[BUFFER_SIZE];

// Control variables
bool wsClientConnected = false;
bool audioStreamingActive = false;

// Task handle for audio processing
TaskHandle_t audioTaskHandle = NULL;

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
        audioStreamingActive = false;
      }
      break;
    case WS_EVT_DATA:
      if (len) {
        char *message = (char *)malloc(len + 1);
        memcpy(message, data, len);
        message[len] = '\0';

        String command = String(message);
        if (command == "play") {
          audioStreamingActive = true;
          digitalWrite(led_indicator, HIGH);
          Serial.println("Audio streaming started");
        } else if (command == "stop") {
          audioStreamingActive = false;
          digitalWrite(led_indicator, LOW);
          Serial.println("Audio streaming stopped");
        }

        free(message);
      }
      break;
  }
}// end onWebSocketEvent()

void setupI2S() {
  // I2S configuration
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,  // Changed to 16-bit
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,  // Using the LEFT channel
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = BUFFER_COUNT,
    .dma_buf_len = BUFFER_SIZE,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  // I2S pin configuration
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK_PIN,
    .ws_io_num = I2S_WS_PIN,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD_PIN
  };

  // Install and configure I2S
  esp_err_t err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
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
}// end setupI2S()

void audioTask(void *parameter) {
  setupI2S();

  size_t bytesRead = 0;
  while (true) {
    // Only process audio if streaming is active and we have connected clients
    if (audioStreamingActive && wsClientConnected) {
      esp_err_t result = i2s_read(I2S_PORT, &audioBuffer, sizeof(audioBuffer), &bytesRead, portMAX_DELAY);

      if (result == ESP_OK && bytesRead > 0) {
        // Send binary data directly
        ws.binaryAll((const char *)audioBuffer, bytesRead);
      } else if (result != ESP_OK) {
        Serial.printf("I2S read error: %d\n", result);
        delay(100);
      }
    } else {
      // If not streaming, yield to other tasks
      delay(100);
    }
  }
}// end audio task()

void setup() {
  Serial.begin(115200);
  pinMode(I2S_SCK_PIN, OUTPUT);
  pinMode(I2S_WS_PIN, OUTPUT);
  pinMode(I2S_SD_PIN, INPUT);
  pinMode(I2S_LR_IO, OUTPUT);
  pinMode(led_indicator, OUTPUT);
  delay(1000);

  // Initialize Soft AP
  WiFi.softAP(ap_ssid, ap_password);

  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("SSID: ");
  Serial.println(ap_ssid);
  Serial.print("Password: ");
  Serial.println(ap_password);

  // Setup WebSocket
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  // Initialize web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", test_page);
  });

  // Start server
  server.begin();
  Serial.println("Web server started");
  digitalWrite(I2S_LR_IO, 0);
  digitalWrite(led_indicator, LOW);

  // Create audio processing task on core 1
  xTaskCreatePinnedToCore(
    audioTask,         // Task function
    "audioTask",       // Task name
    10000,             // Stack size
    NULL,              // Parameters
    1,                 // Priority
    &audioTaskHandle,  // Task handle
    1                  // Core 1
  );

  Serial.println("Audio task created on core 1");
}// end setup()

void loop() {
  // Main loop can handle other tasks
  delay(1000);
}//end main loop()
