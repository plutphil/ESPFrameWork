#include <Adafruit_AHTX0.h>

Adafruit_AHTX0 aht;

void setupAHT()
{
  uint8_t addr = getSetDefaultInt("AHT_ADDRESS", 0x76);
  if (getSetDefaultInt("AHT_ENABLE", 0x0))
  {
    if (!aht.begin())
    {
      Serial.println("Could not find AHT? Check wiring");
    }
  }
  Serial.println("AHT10 or AHT20 found");
}
float ahttemp = 0.f;
float ahthum = 0.f;
void loopAHT()
{
  if (getSetDefaultInt("AHT_ENABLE", 0x0))
  {
    sensors_event_t humidity, temp;
    aht.getEvent(&humidity, &temp); // populate temp and humidity objects with fresh data
    Serial.print("Temperature: ");
    Serial.print(temp.temperature);
    Serial.println(" degrees C");
    Serial.print("Humidity: ");
    Serial.print(humidity.relative_humidity);
    Serial.println("% rH");
    ahttemp = temp.temperature;
    ahthum = humidity.relative_humidity;
  }
}