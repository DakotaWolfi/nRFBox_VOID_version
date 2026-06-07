/* ____________________________
   This software is licensed under the MIT License:
   https://github.com/cifertech/nrfbox
   ________________________________________ */
   
#include "setting.h"
#include "config.h"

extern Adafruit_NeoPixel pixels;

void neopixelSetup() {
  EEPROM.begin(512); 
  neoPixelActive = EEPROM.read(0);
  neoPixelBrightness = EEPROM.read(4);
  if (neoPixelBrightness == 0 || neoPixelBrightness > 255) neoPixelBrightness = 100;

  pixels.begin();
  pixels.clear();
  pixels.show();
}

void neopixelLoop();

void setNeoPixelColour(const std::string& colour) {
  uint32_t colorValue = 0;
  uint8_t brightness = (neoPixelBrightness * 5) / 255;  // Scale 0-255 to 0-5 range

  if (!neoPixelActive && colour != "0" && colour != "null") {
    return;
  }

  if (colour == "red") {
    colorValue = pixels.Color(brightness, 0, 0);
  } else if (colour == "green") {
    colorValue = pixels.Color(0, brightness, 0);
  } else if (colour == "blue") {
    colorValue = pixels.Color(0, 0, brightness);
  } else if (colour == "yellow") {
    colorValue = pixels.Color(brightness, brightness, 0);
  } else if (colour == "purple") {
    colorValue = pixels.Color(brightness, 0, brightness);
  } else if (colour == "cyan") {
    colorValue = pixels.Color(0, brightness, brightness);
  } else if (colour == "white") {
    colorValue = pixels.Color(brightness, brightness, brightness);
  } else if (colour == "null") {
    colorValue = pixels.Color(0, 0, 0);
  } else if (colour == "0") {
    colorValue = pixels.Color(0, 0, 0);
  }

  pixels.setPixelColor(0, colorValue);
  pixels.show();
}

void flash(int numberOfFlashes, const std::vector<std::string>& colors, const std::string& finalColour) {
  if (numberOfFlashes <= 0 || colors.empty()) {
    Serial.println("Invalid parameters for flash: Check numberOfFlashes or colors vector.");
    return;
  }

  for (int i = 0; i < numberOfFlashes; ++i) {
    for (const auto& color : colors) {
      setNeoPixelColour(color);
      delay(500);
    }
  }
  setNeoPixelColour(finalColour);
}
