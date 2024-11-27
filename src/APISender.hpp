#include <HTTPClient.h>
void sendData()
{

  String payload = "{";
  payload += "\"software_version\": \"" + softwareVersion + "\",";
  payload += "\"sensordatavalues\": [";
  String data = "";
  if (getSetDefaultInt("BMP_ENABLE", 0x0))
  {
  data += "{\"value_type\": \"pressure\", \"value\": \"" + String(pressure, 2) + "\"},";
  data += "{\"value_type\": \"temperature\", \"value\": \"" + String(temperature, 2) + "\"}";
  }
  if (getSetDefaultInt("AHT_ENABLE", 0x0))
  {
  //payload += "{\"value_type\": \"temperature2\", \"value\": \"" + String(ahttemp, 2) + "\"},";
  if(data.length()>0){
    data+=",";
  }
    data += "{\"value_type\": \"humidity\", \"value\": \"" + String(ahthum, 2) + "\"}";
  }
  payload+=data;
  payload += "]}";

  HTTPClient http;

  http.begin(host);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-Sensor", sensorID);
  http.addHeader("X-MAC-ID", macID);
  int httpResponseCode = http.POST(payload);

  if (httpResponseCode > 0)
  {
    String response = http.getString();
    Serial.println("HTTP Response code: " + String(httpResponseCode));
    Serial.println("Response: " + response);
  }
  else
  {
    Serial.println("Error on sending POST: " + String(httpResponseCode));
  }

  http.end();
}