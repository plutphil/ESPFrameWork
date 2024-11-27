#include "IPAddress.h"

#include <WiFi.h>
#include <ArduinoHA.h>

#include "config.h"


//WiFiClient client;
HADevice device;
HAMqtt mqtt(client, device);

HASensorNumber ha_voltage("voltage", HASensorNumber::PrecisionP2);
HASensorNumber ha_amperage("amperage", HASensorNumber::PrecisionP2);
HASensorNumber ha_power("power", HASensorNumber::PrecisionP2);
HASensorNumber ha_powerfactor("powerfactor", HASensorNumber::PrecisionP2);
HASensorNumber ha_freqency("freqency", HASensorNumber::PrecisionP2);
HASwitch ha_switch("switch");
String mqtthost = "";
String mqttuser = "";
String mqttpw = "";
void mqttBegin(){
  mqtthost = configtxt.getSetDefault("MQTT_ADDR","");
  mqttuser = configtxt.getSetDefault("MQTT_USER",""); 
  mqttpw =  configtxt.getSetDefault("MQTT_PW","");
  mqtt.begin(
    mqtthost.c_str(), 
    mqttuser.c_str(), 
    mqttpw.c_str());
    printf("mqtt info'%s' '%s' '%s'\n",
    mqtthost.c_str(), 
    mqttuser.c_str(), 
    mqttpw.c_str());
}
void setupMQTT(){
  
  byte mac[6];
  WiFi.macAddress(mac);
  device.setUniqueId(mac, sizeof(mac));
  
  device.setName("ESP32_SEM6000_GateWay");
  device.setSoftwareVersion("1.0.0");

  ha_switch.setIcon("mdi:lightbulb");
  ha_switch.setName("My LED");
  //led.onCommand(onSwitchCommand);
    
  ha_voltage.setName("Voltage");
  ha_voltage.setUnitOfMeasurement("V");

  ha_amperage.setName("Amperage");
  ha_amperage.setUnitOfMeasurement("A");

  ha_power.setName("Power");
  ha_power.setUnitOfMeasurement("W");

  ha_powerfactor.setName("Power Factor");
  ha_powerfactor.setUnitOfMeasurement("#");

  ha_freqency.setName("Frequency");
  ha_freqency.setUnitOfMeasurement("Hz");
  
  mqttBegin();
}
long nexttime = 0;
bool onconnect = true;
//IPAddress BROKER_ADDR=IPAddress();
void mqttloop() {
  mqtt.loop();

  if (WiFi.status() == WL_CONNECTED){
    if(onconnect){
      onconnect=false;
      mqttBegin();
    }
  }else{
    delay(1000);
    Serial.print(".");
  }
  long now = millis();
  if(nexttime<now){
    nexttime = now+5000;
    if(mqtt.isConnected()){
    }else{
      Serial.println("MQTT! not connected!");
      mqttBegin();
    }
  }
  
}