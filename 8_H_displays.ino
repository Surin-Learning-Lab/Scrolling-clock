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
    matrix.setRotation(i, 1); // Adjust rotation if needed
  }

  dht.begin();
}

void loop() {
  byte ledIntensity = ledIntensitySelect(analogRead(LDR_PIN));
  matrix.setIntensity(ledIntensity);
  String output = outputStrClock();

  for (int i = 0; i < FONT_WIDTH * output.length() + matrix.width() - 1 - SPACER; i++) {
    matrix.fillScreen(LOW);
    output = outputStrClock();
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
  Serial.print(F("Temperature: "));
  Serial.println(dht.readTemperature());
  Serial.print(F("Humidity: "));
  Serial.println(dht.readHumidity());
}

byte ledIntensitySelect(int value) {
  return map(value, 0, 1023, 8, 0);
}

String outputStrClock() {
  RtcDateTime now = Rtc.GetDateTime();
  char buffer[40];
  snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d %02d:%02d:%02d Temp:%0dC RH:%d%%", 
           now.Day(), now.Month(), now.Year(), now.Hour(), now.Minute(), now.Second(),
           static_cast<int>(dht.readTemperature()), static_cast<int>(dht.readHumidity()));
  return String(buffer);
}

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
