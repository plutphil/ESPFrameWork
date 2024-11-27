#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 5; //In seconds
BLEScan* pBLEScan;
String bufferToHexString(byte* buffer, int length) {
  String hexString = "";
  for (int i = 0; i < length; i++) {
    if (buffer[i] < 0x10) {
      hexString += "0"; // Add leading zero if needed
    }
    hexString += String(buffer[i], HEX);
  }
  return hexString;
}
#define APPLE_MANUFACTURER_ID 0x004C
void decodeAppleBLEData(uint8_t *advData, int advDataLen) {
    // Function to extract and print BLE advertisement details
    Serial.println(F("Decoding Apple BLE Advertisement Data:"));

    int i = 0;
    while (i < advDataLen) {
        uint8_t length = advData[i];
        if (length == 0) break;

        uint8_t type = advData[i + 1];
        uint8_t *data = &advData[i + 2];

        if (type == 0xFF) {  // Manufacturer specific data
            uint16_t manufacturerID = (data[1] << 8) | data[0];
            if (manufacturerID == APPLE_MANUFACTURER_ID) {
                Serial.print(F("Apple Manufacturer Data: "));
                for (int j = 0; j < length - 1; j++) {
                    Serial.print(data[j], HEX);
                    Serial.print(" ");
                }
                Serial.println();

                // Decode specific Apple device types
                if (data[2] == 0x01 && data[3] == 0x00) {
                    Serial.println(F("Likely Device: Apple iBeacon"));
                } else if (data[2] == 0x09 && data[3] == 0x08) {
                    Serial.println(F("Likely Device: Apple AirPods"));
                } else if (data[2] == 0x0C && data[3] == 0x0E) {
                    Serial.println(F("Likely Device: Apple Watch"));
                } else {
                    Serial.println(F("Unknown Apple Device"));
                }
            }
        }

        i += length + 1;
    }
}