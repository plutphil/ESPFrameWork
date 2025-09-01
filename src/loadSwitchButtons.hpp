#include <LittleFS.h>
#include <ArduinoJson.h>

std::vector<SwitchButton> switchbuttons;

void loadSwitchButtons() {
  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return;
  }

  File file = LittleFS.open("/switches.json", "r");
  if (!file) {
    Serial.println("Failed to open switches.json");
    return;
  }

  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println("Failed to parse JSON");
    return;
  }
  switchbuttons.clear();
  for (JsonObject obj : doc.as<JsonArray>()) {
    int x = obj["x"] | 0;
    int y = obj["y"] | 0;
    int w = obj["w"] | 100;
    int h = obj["h"] | 50;
    String text = obj["text"] | "Unknown";
    String entity = obj["entity"] | "";
    switchbuttons.push_back(SwitchButton(x,y,w,h,text,entity));
    printf("Loaded button: %s at (%d, %d, %d, %d) for entity %s\n", text.c_str(), x, y, w, h, entity.c_str());
  }
  file.close();
}
