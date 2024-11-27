#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
void setupOled(){
    uint8_t addr = getSetDefaultInt("OLED_ADDRESS",0x3c);
    if(getSetDefaultInt("OLED_ENABLE",0)){
        if (!display.begin(SSD1306_SWITCHCAPVCC, addr)) {
            Serial.println(F("SSD1306 allocation failed"));
        }else{
            display.clearDisplay();
            display.setTextSize(1);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0, 0);
            display.println("BMP280 & OLED Test");
            display.display();
        }
    }
}