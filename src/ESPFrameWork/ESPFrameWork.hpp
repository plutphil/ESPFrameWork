#include "smalllog.hpp"
#include "MyFS.hpp"
#include "Defaults.h"
#include "Config.hpp"
#include "WebServer.hpp"
#include "OTA.hpp"
#include "WIFI.hpp"
void framework_setup(){
  config_setup();
  wifi_setup();
}
void framework_loop(){
  ArduinoOTA.handle();
  wifi_loop();
  httpUpdateLoop();
  if(shouldReboot){
    Serial.println("Rebooting...");
    delay(100);
    ESP.restart();
  }
  while(Serial.available()>0){
    char c = Serial.read();
    Serial.write(c);
  }
}
