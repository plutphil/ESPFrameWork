#include "BLEHelper.hpp"
#include "HA.hpp"
BLEClient *pClient = BLEDevice::createClient();

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        Serial.println("dev conn");
    };

    void onDisconnect(BLEServer *pServer)
    {
        Serial.println("dev disconn");
    }
};
class MyNotifyCallback : public BLECharacteristicCallbacks
{
    void onNotify(BLECharacteristic *pCharacteristic)
    {
        Serial.print("Notification received for characteristic: ");
        Serial.println(pCharacteristic->getUUID().toString().c_str());
        Serial.print("Value: ");
        Serial.println(pCharacteristic->getValue().c_str());
    }
};
static void notifyCallback(
    BLERemoteCharacteristic *pBLERemoteCharacteristic,
    uint8_t *pData,
    size_t length,
    bool isNotify)
{
    /*Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    */
    Serial.print("data: ");
    Serial.print(bufferToHexString(pData, length));
    Serial.flush();

    if (pData[3] == 0 && pData[2] == 4)
    {
        int i = 4;
        int state = pData[i];
        i++;
        uint32_t w32 = pData[i] << 16;
        w32 |= pData[i + 1] << 8;
        w32 |= pData[i + 2];

        float watt = w32 / 1000.f;
        i += 3;
        int voltage = pData[i];
        i++;
        uint16_t a16 = pData[i] << 8 | pData[i + 1];
        float ampere = a16 / 1000.f;
        i += 2;
        int freq = pData[i];
        i++;

        float powerfactor = watt / (voltage != 0.f ? voltage : 1.f) / (ampere != 0 ? ampere : 1.f);
        Serial.printf(" Voltage: %d, Watt: %.3f, Amp: %.3f, Power Factor: %.3f, Frequency: %d, State: %d", voltage, watt, ampere, powerfactor, freq, state);
        ha_voltage.setValue(voltage);
        ha_power.setValue(watt);
        ha_powerfactor.setValue(powerfactor);
        ha_freqency.setValue(freq);
        ha_amperage.setValue(ampere);
        ha_switch.setCurrentState(state);
    }
    Serial.println();
}
int hexStringToBuffer(String hexString, byte *buffer, int bufferSize)
{
    int strLength = hexString.length();
    for (int i = 0; i < strLength; i += 2)
    {
        if (i / 2 < bufferSize)
        {
            String byteString = hexString.substring(i, i + 2);
            buffer[i / 2] = (byte)strtol(byteString.c_str(), NULL, 16);
        }
    }
    return strLength / 2;
}
BLERemoteCharacteristic *writeChar = NULL;
void listDev(BLEAdvertisedDevice myDevice)
{
    Serial.print("Connecting...");
    pClient->connect(&myDevice); // if you pass a true in the 2nd parameter, then it's a direct connection
    Serial.println("Connected");
    // Obtain a reference to the service we are after in the remote BLE server.
    std::map<std::string, BLERemoteService *> *services = pClient->getServices();
    for (auto const &servicePair : *services)
    {
        Serial.print("Service found: ");
        Serial.println(servicePair.first.c_str());

        // Get all characteristics of the service
        BLERemoteService *remoteService = servicePair.second;
        std::map<std::string, BLERemoteCharacteristic *> *characteristics = remoteService->getCharacteristics();
        for (auto const &charPair : *characteristics)
        {
            BLERemoteCharacteristic *remoteCharacteristic = charPair.second;
            Serial.print("Char: ");
            Serial.print(remoteCharacteristic->getUUID().toString().c_str());

            // If characteristic can notify, register for notifications
            if (remoteCharacteristic->canNotify())
            {
                remoteCharacteristic->registerForNotify(notifyCallback);
                Serial.print(" NOTIFY ");
            }
            if (remoteCharacteristic->canWrite())
            {
                Serial.print(" WRITE ");
            }
            if (remoteCharacteristic->canRead())
            {
                Serial.print(" READ ");
                Serial.print(remoteCharacteristic->readValue().c_str());
            }
            if (remoteCharacteristic->canWriteNoResponse())
            {
                Serial.println(" WRITE_NO_RES ");
                writeChar = remoteCharacteristic;
            }
            Serial.println();
        }
    }
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {

        Serial.printf("adv,%s,%d,", advertisedDevice.getName().c_str(), advertisedDevice.getRSSI());
        Serial.print(advertisedDevice.getAddress().toString().c_str());
        Serial.print(",");
        if (advertisedDevice.haveManufacturerData())
        {
            auto mandat = advertisedDevice.getManufacturerData();
            Serial.print(bufferToHexString((byte *)mandat.c_str(), mandat.size()));
        }
        Serial.print(",");
        Serial.print(bufferToHexString(advertisedDevice.getPayload(), advertisedDevice.getPayloadLength()));

        if (advertisedDevice.haveServiceUUID())
        {
        }
        Serial.print(",");
        if (advertisedDevice.haveServiceData())
        {
        }
        Serial.print(",");
        Serial.println();
        decodeAppleBLEData(advertisedDevice.getPayload(), advertisedDevice.getPayloadLength());

        if (String(advertisedDevice.getName().c_str()).startsWith("Voltcraft"))
        {
            // listDev(advertisedDevice);
        }
    }
};

void bleAppSetup()
{
    Serial.println("BLEinit...");
    BLEDevice::init("");
    Serial.println("get scan...");
    pBLEScan = BLEDevice::getScan(); // create new scan
    Serial.println("setting up scan");
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true); // active scan uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99); // less or equal setInterval value
    // pClient->setClientCallbacks(new MyClientCallback());
    setupMQTT();
}
void bleAppLoop()
{
    Serial.println("scan start");
    BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
    Serial.print("Devices found: ");
    Serial.println(foundDevices.getCount());
    Serial.println("+-------------------------+");
    Serial.println("|Scan done! Select Device |");
    Serial.println("+-------------------------+");
    for (int i = 0; i < foundDevices.getCount(); i++)
    {
        Serial.print(i);
        Serial.print(" ");
        auto dev = foundDevices.getDevice(i);
        Serial.print(dev.getAddress().toString().c_str());
        if (dev.haveName())
        {
            Serial.print(" name ");
            Serial.print(dev.getName().c_str());
            if (String(dev.getName().c_str()).startsWith("Voltcraft"))
            {
                listDev(dev);
                byte buf[100];
                int l = 0;
                l = hexStringToBuffer("0f0c170000000000000000000018ffff", (byte *)buf, 100);
                writeChar->writeValue(buf, l);
                delay(1000);
                writeChar->writeValue(buf, l);
                delay(1000);
                writeChar->writeValue(buf, l);
                delay(1000);
                l = hexStringToBuffer("0f0c0100093013100507e8000052ffff", (byte *)buf, 100);
                writeChar->writeValue(buf, l);
                delay(1000);
                l = hexStringToBuffer("0f050400000005ffff", (byte *)buf, 100);
                while (pClient->isConnected())
                {
                    writeChar->writeValue(buf, l);
                    mqttloop();
                    delay(3000);
                }
            }
        }
        if (dev.haveRSSI())
        {
            Serial.print(" rssi ");
            Serial.print(dev.getRSSI());
        }
        Serial.println();
    }
    pBLEScan->stop();
}