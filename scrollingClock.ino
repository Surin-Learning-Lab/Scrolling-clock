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
const byte H_DISPLAYS = 4;
const byte V_DISPLAYS = 1;

DHT dht(DHT_PIN, DHT11);

Max72xxPanel matrix = Max72xxPanel(CS_PIN, H_DISPLAYS, V_DISPLAYS);

const byte WAIT = 60;
const byte SPACER = 1;
const byte FONT_WIDTH = 5 + SPACER;

void setup() {
  pinMode(LDR_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  Serial.println(F(">> Arduino 32x8 LED Dot Matrix Clock with DHT11!"));
  Serial.println(F(">> Use <dd/mm/yyyy hh:mm:ss> format to set clock's date and hour!"));
  Rtc.Begin();
  matrix.setPosition(0, 0, 0);
  matrix.setPosition(1, 1, 0);
  matrix.setPosition(2, 2, 0);
  matrix.setPosition(3, 3, 0);
  matrix.setRotation(0, 1);    
  matrix.setRotation(1, 1);
  matrix.setRotation(2, 1);
  matrix.setRotation(3, 1);

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
  Serial.println(dht.readTemperature());
  Serial.println(dht.readHumidity());
}

byte ledIntensitySelect(int value) {
  return map(value, 0, 1023, 8, 0);
}

String outputStrClock() {
  RtcDateTime now = Rtc.GetDateTime();
  char buffer[40];
  snprintf(buffer, sizeof(buffer), "%02d/%02d/%04d %02d:%02d:%02d Temp:%0dC Hum:%d%%", 
           now.Day(), now.Month(), now.Year(), now.Hour(), now.Minute(), now.Second(),
           static_cast<int>(dht.readTemperature()), static_cast<int>(dht.readHumidity()));
  return String(buffer);
}

