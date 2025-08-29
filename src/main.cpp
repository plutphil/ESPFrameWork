#include <Arduino.h>
void reload();
#include"ESPFrameWork/ESPFrameWork.hpp"
#include"TimeZoneTime.hpp"
#include "Button2.h"
#include "OLED.hpp"
#include "BMP.hpp"
#include "AHT.hpp"
#include "APISender.hpp"
//#include "BLEApp.hpp"
#define BUTTON_PIN_1  5
#define BUTTON_PIN_2  4
Button2 button_1, button_2;
void handler(Button2& btn) {
    switch (btn.getType()) {
        case single_click:
            break;
        case double_click:
            Serial.print("double ");
            break;
        case long_click:
            Serial.print("long ");
            break;
    }
    Serial.print("click ");
    Serial.print("on button #");
    Serial.print((btn == button_1) ? "1" : "2");
    Serial.println();
}

class Rect
{
public:
  int x, y, w, h;
  Rect()
  {
  }
  Rect(int x_, int y_, int w_, int h_) : x(x_), y(y_), w(w_), h(h_)
  {
  }
  Rect padding(int p)
  {
    return {x + p, y + p, w - p * 2, h - p * 2};
  }
  Rect grid(int gw, int gh, int gx, int gy, int gsw = 1, int gsh = 1)
  {
    return {
        x + w / gw * gx,
        y + h / gh * gy,
        w / gw * gsw,
        h / gh * gsh};
  }
  bool isIn(int px, int py)
  {
    return px >= x && py >= y && px <= x + w && py <= y + h;
  }
};


#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#define CS_PIN 7
XPT2046_Touchscreen ts(CS_PIN);

int p = 20;
class SwitchButton
{
public:
  Rect r={10,10,100,50};
  bool state=false;
  String text="Kitchen";
  String entity="";
  SwitchButton(){}

  SwitchButton(int x, int y, int w, int h, String text_, String entity_)
  {
    r = Rect(x, y, w, h);
    text = text_;
    entity = entity_;
  }
  void draw()
  {
    auto cp = r.padding(p);
    uint32_t col = state ? TFT_ORANGE : TFT_DARKGREY;
    tft.drawSmoothRoundRect(r.x, r.y, p, 0, r.w, r.h, col, TFT_BLACK);
    tft.fillRect(cp.x, cp.y, cp.w, cp.h, col);
    tft.fillRect(cp.x, cp.y, 10, 10, TFT_WHITE);
    tft.setTextColor(TFT_WHITE);
    
    tft.setTextFont(4);
    tft.setTextSize(1);

    tft.drawString(text, cp.x, cp.y + cp.h - 15 - p);
  }
};

bool state = true;
SwitchButton switchbutton1 = SwitchButton();
SwitchButton switchbutton2 = SwitchButton();
SwitchButton switchbutton3 = SwitchButton();

void init_buttons(){
  button_1.begin(BUTTON_PIN_1);
  button_1.setClickHandler(handler);
  button_1.setDoubleClickHandler(handler);
  button_1.setLongClickHandler(handler);

  button_2.begin(BUTTON_PIN_2);
  button_2.setClickHandler(handler);
  button_2.setDoubleClickHandler(handler);
  button_2.setLongClickHandler(handler);
}
hw_timer_t *timer = NULL;
bool doblink = false;
void IRAM_ATTR onTimer(TimerHandle_t xTimer) {
    //Serial.println("1 second elapsed");
    //if(doblink)
    //digitalWrite(8,!digitalRead(8));
    //displayData();
}
const unsigned long interval = 5000; // Interval of 1 second
unsigned long nextTriggerTime = 0;   // Next time to perform the task
#define TIMER_INTERVAL 1000 / portTICK_PERIOD_MS // Timer interval in milliseconds
#include "ESPFrameWork/I2CScan.hpp"

#include "loadSwitchButtons.hpp"


void reload(){
  tft.fillScreen(TFT_BLACK);
  loadSwitchButtons();
  for (auto &swb : switchbuttons) {
    swb.draw();
  }
}

void setup() {
  Serial.begin(115200);
  /*pinMode(8,OUTPUT);
  for (size_t i = 0; i < 6; i++)
  {
    digitalWrite(8,!digitalRead(8));
    delay(300);
  }*/
  


  framework_setup();
  Wire.begin(getSetDefaultInt("PIN_SDA",20),getSetDefaultInt("PIN_SCL",10));
  delay(5000);
  scanI2C();
  setup_timezonetime();
  Serial.println(getAddDefault("HTTP_USER",HTTP_USER).c_str());
  Serial.println(getAddDefault("HTTP_PW",HTTP_PW).c_str());
  init_buttons();
  setupOled();
  setupBMP();
  setupAHT();
  displayData();

  TimerHandle_t timer = xTimerCreate(
      "PeriodicTimer",         // Timer name
      TIMER_INTERVAL,          // Timer period (1 second)
      pdTRUE,                  // Auto-reload
      (void *)0,               // Timer ID (not used here)
      onTimer                  // Timer callback function
  );
  if (timer == NULL) {
      Serial.println("Timer creation failed");
      return;
  }

  // Start the timer
  if (xTimerStart(timer, 0) != pdPASS) {
      Serial.println("Timer start failed");
  }
  //bleAppSetup();

  
  tft.init();
  int orientation = 1;
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(orientation);
  tft.setTextFont(1);
  tft.setCursor(0, 0, 2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);

  if (ts.begin())
  {
    tft.println("TOUCH INIT SUCCESS");
  }
  else
  {
    tft.println("TOUCH INIT FAILED");
  }
  
  int W = 480;
  int H = 320;
  int p = 5;
  auto screenrect = Rect(0, 0, W, H);
  
  switchbutton1.r = screenrect.grid(4, 3, 0, 0).padding(p);
  switchbutton2.r = screenrect.grid(4, 3, 1, 0).padding(p);
  switchbutton1.draw();
  switchbutton2.draw();
  switchbutton3.draw();

  reload();
}



void drawWiFiIndicator(int16_t x, int16_t y) {
  // Get RSSI in dBm
  int32_t rssi = WiFi.RSSI();

  // Convert RSSI to 0–4 bars
  int bars = 0;
  if (rssi > -55) bars = 4;
  else if (rssi > -65) bars = 3;
  else if (rssi > -75) bars = 2;
  else if (rssi > -85) bars = 1;
  else bars = 0;

  // Clear area first
  tft.fillRect(x, y-20, 30, 20, TFT_BLACK);

  // Draw bars
  for (int i = 0; i < 4; i++) {
    int barHeight = (i+1) * 4;
    int barX = x + i*7;
    int barY = y - barHeight;
    uint16_t color = (i < bars) ? TFT_GREEN : TFT_DARKGREY;
    tft.fillRect(barX, barY, 5, barHeight, color);
  }
}


bool wastouched = false;
void loop() {
  framework_loop();
  if(WiFi.isConnected()){
    loop_timezontime();
  }
  long now = millis();
  if (now >= nextTriggerTime) {
    nextTriggerTime = now+interval;
    
    loopAHT();
    bmpLoop();

    servermessage = "aht: "+String(ahttemp)+" *C " 
    +String(ahthum)+" %rh <br>"+
    String("bmp: ")+String(temperature)+" *C "+String(pressure)+" hPa";
    displayData();

    if(WiFi.isConnected()){
      if (getSetDefaultInt("SENDAPI_EN", 0x0)){
        sendData();
      }
    }
    //bleAppLoop();

    drawWiFiIndicator(440, 20); // top right corner
  }
  //mqttloop();

  boolean istouched = ts.touched();
  if (istouched)
  {
    TS_Point p = ts.getPoint();
    if (!wastouched)
    {
      int x = map(p.x, 3800, 300, 0, 480);
      int y = map(p.y, 3650, 350, 0, 320);
      tft.fillRect(x, y, 20, 20, TFT_WHITE);
      //client.publish("zigbee2mqtt/Philipp Zimmer/set",state?"ON":"OFF");
      
      /*bool conn = client.connected();
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.setCursor(480*3/4, 0, 2);
      // print connection status
      tft.printf("Connected: %s\n", conn ? "Yes" : "No");
      // wifi rssir and status
      tft.printf("Wifi RSSI: %d\n", WiFi.RSSI());
      tft.printf("Wifi Status: %d\n", WiFi.status());

      // get_switch_entities();
      for(auto swb:{&switchbutton1,&switchbutton2,&switchbutton3}){
        if(swb->r.isIn(x,y)){
          //swb->state = !swb->state;
          swb->state ^= 1;
          swb->draw();
        }
      }
      state = !state;*/
      for(auto swb:{&switchbutton1,&switchbutton2,&switchbutton3}){
        if(swb->r.isIn(x,y)){
          //swb->state = !swb->state;
          swb->state ^= 1;
          swb->draw();
        }
      }
    }
  }
  wastouched = istouched;
}
