// Make sure the Libary is installed before compiling the code
#include <TM1637Display.h>

// Define the connection pins
#define CLK 2
#define DIO 3

// Create display object
TM1637Display display(CLK, DIO);

// Variables for tracking time
unsigned long currentMillis;
unsigned int elapsedSeconds = 0;

void setup() {
  // Set brightness (0-7)
  display.setBrightness(5);

  // Clear the display
  display.clear();
}

void loop() {
  // Get current time
  currentMillis = millis();

  // Calculate elapsed seconds
  unsigned int elapsedSeconds = currentMillis / 1000;
  display.showNumberDec(elapsedSeconds);  // Display the elapsed seconds

  // Reset to 0 after 9999 seconds
  if (elapsedSeconds > 9999) {
    elapsedSeconds = 0;  // Reset the start time
    display.showNumberDec(0);
  }
}