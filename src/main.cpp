#include <Arduino.h>
#include"ESPFrameWork/ESPFrameWork.hpp"
#include"TimeZoneTime.hpp"
#include "Button2.h"
#include "OLED.hpp"
#include "BMP.hpp"
#include "AHT.hpp"
#include "APISender.hpp"
#include "BLEApp.hpp"
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
  bleAppSetup();
}
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
    bleAppLoop();
  }
  mqttloop();
}
