#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient mqttclient(espClient);
String mqtt_server = "";
String mqtt_user = "";
String mqtt_password = "";
bool mqttConnected = false;

void printMsgAt(String str, int y = 0)
{
    tft.setTextSize(1);
    tft.setTextFont(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
    tft.drawString(str, 3, y);
    tft.setTextColor(TFT_WHITE, TFT_BLACK, false);
}
int cury = 0;
void msg(String str)
{
    Serial.println(str);
    tft.fillRect(0, cury, 320, 8, TFT_BLACK);
    // tft.drawString(str, 3, cury);
    printMsgAt(str, cury);
    cury += 9;
    if (cury >= 240)
    {
        cury = 0;
    }
}
// MQTT Callback
void callback(char *topic_, byte *payload_, unsigned int length)
{
    String topic(topic_);
    String payload = String((char *)payload_, length);
    Serial.printf("MQTT IN [%s]: %s\n", topic.c_str(), payload.c_str());

    // Example: update local switchbutton state if topic matches entity
    for (auto &swb : switchbuttons) {
        if (topic == swb.entity) {
        swb.state = (payload ==swb.onstate);
        swb.draw();
        }
    }
}

// MQTT Connection Function
void connectMQTT()
{
    if (!mqttclient.connected())
    {
        msg("Connecting to MQTT...");

        if (mqttclient.connect("ESP32mqttclient", mqtt_user.c_str(), mqtt_password.c_str()))
        {
            mqttConnected = true;
            msg("MQTT Connected!");
            for (auto &swb : switchbuttons)
            {
                String stateTopic = swb.entity; // adjust if your HA uses .../state
                mqttclient.subscribe(stateTopic.c_str());
            }
        }
        else
        {
            msg("MQTT Connection Failed: " + String(mqttclient.state()));
        }
    }
}

void setupMqtt()
{
    mqtt_server = getAddDefault("MQTT_SERVER", "");
    mqtt_user = getAddDefault("MQTT_USER", "");
    mqtt_password = getAddDefault("MQTT_PASSWORD", "");

    mqttclient.setServer(mqtt_server.c_str(), 1883);
    mqttclient.setCallback(callback);
    
}

void mqttLoop()
{
    if (mqttclient.connected())
    {
        mqttclient.loop();
    }
    else
    {
        mqttConnected = false;
        // msg("MQTT Disconnected!");
        //
        connectMQTT();
        delay(1000);
    }
}