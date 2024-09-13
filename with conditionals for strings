#include <RtcDS1302.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "Adafruit_GFX.h"
#include "Max72xxPanel.h"

ThreeWire myWire(4, 5, 2); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire);

const byte LDR_PIN = A2;
const byte DHT_PIN = 6; // Replace with the pin your DHT sensor is connected to
const byte CS_PIN = A3;
const byte H_DISPLAYS = 8; // Updated to 8 for eight displays
const byte V_DISPLAYS = 1;

DHT dht(DHT_PIN, DHT11);

Max72xxPanel matrix = Max72xxPanel(CS_PIN, H_DISPLAYS, V_DISPLAYS);

const byte WAIT = 60;
const byte SPACER = 1;
const byte FONT_WIDTH = 5 + SPACER;

void setup() {
  pinMode(LDR_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println(F(">> Arduino 64x8 LED Dot Matrix Clock with DHT11!"));
  Serial.println(F(">> Use <dd/mm/yyyy hh:mm:ss> format to set clock's date and hour!"));

  // Initialize RTC
  Rtc.Begin();

  // Check if RTC is connected
  if (!Rtc.GetIsRunning()) {
    Serial.println(F("RTC not running! Setting initial date and time."));
    // Set the date and time (only once)
    Rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));
  } else {
    Serial.println(F("RTC is running."));
  }

  // Print initial date and time to verify
  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);

  // Set positions and rotations for 8 displays
  for (byte i = 0; i < H_DISPLAYS; i++) {
    matrix.setPosition(i, i, 0); // Horizontal arrangement
    matrix.setRotation(i, 1);    // Adjust rotation if needed
  }

  dht.begin();
}

void loop() {
  int ldrValue = analogRead(LDR_PIN);      // Read the LDR value
  byte ledIntensity = ledIntensitySelect(ldrValue);  // Get LED intensity based on LDR value
  matrix.setIntensity(ledIntensity);       // Set the LED matrix intensity
  
  // Get clock output string
  String output = outputStrClock();
  
  // Display output on matrix
  for (int i = 0; i < FONT_WIDTH * output.length() + matrix.width() - 1 - SPACER; i++) {
    matrix.fillScreen(LOW);
    int letter = i / FONT_WIDTH;
    int x = (matrix.width() - 1) - i % FONT_WIDTH;
    int y = (matrix.height() - 8) / 2;
    while (x + FONT_WIDTH - SPACER >= 0 && letter >= 0) {
      if (letter < output.length()) {
        matrix.drawChar(x, y, output[letter], HIGH, LOW, 1);
      }
      letter--;
      x -= FONT_WIDTH;
    }
    matrix.write();
    delay(WAIT);
  }
  
  // Print temperature and humidity to Serial Monitor for debugging
  Serial.println(output);
}

// Function to select the LED intensity based on the mapped LDR value
byte ledIntensitySelect(int value) {
  // Map the LDR value (0-1023) to the desired range (1-5)
  int mappedValue = map(value, 0, 1023, 1, 5);

  Serial.print("Mapped LDR Value: "); // Print the mapped LDR value for debugging
  Serial.println(mappedValue);

  // Set LED intensity based on the mapped LDR value
  if (mappedValue >= 3) {
    return 5; // Brightest setting
  } else {
    return 1; // Dim setting
  }
}

// Updated function to generate the output string for the clock, including custom messages
String outputStrClock() {
  RtcDateTime now = Rtc.GetDateTime();
  char buffer[100]; // Buffer for date, time, and message
  char tempBuffer[10]; // Buffer for temperature
  char humidityBuffer[10]; // Buffer for humidity
  
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  String message = "";

  // Custom temperature messages
  if (temperature < 30) {
    message = "Too COLD!";
  } else if (temperature >= 30 && temperature < 32) {
    message = "Bruh.. Nice weather!";
  } else if (temperature >= 35 && temperature < 40) {
    message = "Getting warmer!";
  } else if (temperature >= 40) {
    message = "Man, It's Hot!";
  }else if (temperature >= 45) {
    message = "You Could Fry an Egg on My Head!";
  }

  // Custom humidity message
  if (humidity > 90) {
    message = "It is going to Rain.";
  }

  // Manually format the temperature and humidity into their own buffers
  dtostrf(temperature, 4, 1, tempBuffer); // Convert float to string with 1 decimal
  dtostrf(humidity, 4, 1, humidityBuffer); // Convert float to string with 1 decimal

  // Format the output string to show date, time, temperature, humidity, and custom message
  snprintf(buffer, sizeof(buffer), 
           "%02d/%02d/%04d %02d:%02d:%02d Temp:%sC RH:%s%% %s SurinLearningLab.com", 
           now.Day(), now.Month(), now.Year(), 
           now.Hour(), now.Minute(), now.Second(),
           tempBuffer, humidityBuffer, message.c_str());

  return String(buffer);
}


// Function to print date and time to the Serial Monitor
void printDateTime(const RtcDateTime& dt) {
  char datestring[20];
  snprintf_P(datestring, 
             sizeof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Day(),
             dt.Month(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  Serial.print(datestring);
  Serial.println();
}
