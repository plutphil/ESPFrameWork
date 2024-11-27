#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#define BMP280_I2C_ADDRESS 0x76
Adafruit_BMP280 bmp;
String host = "https://api.sensor.community/v1/push-sensor-data/";
String sensorID =  "esp32-12390419";
String macID = "esp32-2020203232";
String softwareVersion = "NRZ-2024-135";
void loadConfig(){

  sensorID = getAddDefault("sensorID", "esp32-12390419");
  macID = getAddDefault("macID", "esp32-2020203232");
  softwareVersion = getAddDefault("softwareVersion", "NRZ-2024-135");
  host = getAddDefault("host", "https://api.sensor.community/v1/push-sensor-data/");

}
void setupBMP()
{
  uint8_t addr = getSetDefaultInt("BMP_ADDRESS", 0x76);
  if (getSetDefaultInt("BMP_ENABLE", 0x0))
  {
    if (!bmp.begin(addr))
    {
      Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    }
  }
  loadConfig();
  float pressure = 1011.09;
  float temperature = 28.70;
}
float temperature;
float pressure;

void bmpLoop(){
  if (getSetDefaultInt("BMP_ENABLE", 0x0))
  {
    temperature = bmp.readTemperature();
    pressure = bmp.readPressure() / 100.0F; // Convert Pa to hPa (millibar)

    // Print the data to Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");

    Serial.print("Pressure: ");
    Serial.print(pressure);
    
    Serial.println(" hPa");
  }
}
void displayData()
{
  
  if(getSetDefaultInt("OLED_ENABLE",0)){
    // Clear the display buffer
    display.clearDisplay();

    // Display temperature
    display.setCursor(0, 16);
    display.setTextSize(1);
    display.println("Temp:");
    display.setTextSize(2);
    display.print(temperature);

    display.setTextSize(1);
    display.print(" C");

    // Display pressure
    display.setCursor(0, 32 + 8);
    display.setTextSize(1);
    display.println("Pres:");
    display.setTextSize(2);
    display.print(pressure);

    display.setTextSize(1);
    display.print(" hPa");

    // Update the display with the new data
    display.display();
  }
  
}