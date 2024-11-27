
#include <ESPAsyncWebServer.h>
#include <Wire.h>
AsyncWebServer server(80);
AsyncEventSource events("/events");

int scanResult;
bool shouldscan = false;
// flag to use from web update to reboot the ESP
bool shouldReboot = false;

String servermessage = "";

#include "HTMLHelper.hpp"
void senderrmesg(AsyncWebServerRequest *request, String msg, String redirect = "/", int redirecttime = 3)
{
  request->send(201, "text/html", metarefresh(redirecttime, redirect) + "<h1 style=\"color:red;\">" + msg + "</h1>");
}
void sendmesg(AsyncWebServerRequest *request, String msg, String redirect = "/", int redirecttime = 3)
{
  request->send(200, "text/html", metarefresh(redirecttime, redirect) + "<h1>" + msg + "</h1>");
}

bool checkUserWebAuth(AsyncWebServerRequest *request)
{
  if (getAddDefault("HTTP_PW", HTTP_PW) == "")
    return true;
  if (
      request->authenticate(
          getAddDefault("HTTP_USER", HTTP_USER).c_str(),
          getAddDefault("HTTP_PW", HTTP_PW).c_str()))
  {
    logdbg("is authenticated via username and password");
    return true;
  }

  request->requestAuthentication();
  return false;
}
#include "HTTPUpdate.hpp"
#include "WifiServer.hpp"
#include "FileEditor.hpp"
#include "UpdateServer.hpp"
#include "ConfigEditor.hpp"
void start_server()
{
  //"mount" littlefs filesystem
  server.serveStatic("/", LittleFS, "/").setDefaultFile("index.htm");

  // override config.txt for protection
  server.on("/config.txt", [](AsyncWebServerRequest *request)
            {
    if (checkUserWebAuth(request)) {
      request->send(LittleFS, "/config.txt");
    } });

  // root handling
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (checkUserWebAuth(request)) {
      //request->send(LittleFS, "/index.htm");
      request->send(201, "text/html",
        ahref("/sc","Scan WiFis")+br
        +ahref("/ah","Add WiFi")+br
        +ahref("/ls","File Editor")+br
        +ahref("/lc","Config Editor")+br
        +ahref("/r","Reboot")+br
        +form("/br","Brightness"+inputnice("v",""))+br
        +servermessage
        );
    } });

  // on not found: debug output
  server.onNotFound([](AsyncWebServerRequest *request)
                    {
    Serial.printf("NOT_FOUND: ");
    if(request->method() == HTTP_GET)
      Serial.printf("GET");
    else if(request->method() == HTTP_POST)
      Serial.printf("POST");
    else if(request->method() == HTTP_DELETE)
      Serial.printf("DELETE");
    else if(request->method() == HTTP_PUT)
      Serial.printf("PUT");
    else if(request->method() == HTTP_PATCH)
      Serial.printf("PATCH");
    else if(request->method() == HTTP_HEAD)
      Serial.printf("HEAD");
    else if(request->method() == HTTP_OPTIONS)
      Serial.printf("OPTIONS");
    else
      Serial.printf("UNKNOWN");
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if(request->contentLength()){
      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for(i=0;i<headers;i++){
      AsyncWebHeader* h = request->getHeader(i);
      Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();
    for(i=0;i<params;i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()){
        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if(p->isPost()){
        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    request->send(404); });

  // restart/reset microcontroller
  server.on("/r", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (checkUserWebAuth(request)) {
      sendmesg(request,"Rebooting...");
      shouldReboot=true;
      //ESP.restart();
    } });

  // logout
  server.on("/o", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    request->requestAuthentication();
    request->send(401); });
  server.on("/i2c", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    if (!checkUserWebAuth(request))return;
    AsyncResponseStream *response = request->beginResponseStream("text/plain");
    
    response->print("\nI2C Scanner\n");
    response->print("Scanning...\n");

    byte error, address;
    int nDevices = 0;

    for(address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            response->print("I2C device found at address 0x");
            if (address < 16) response->print("0");
            response->printf("%x", address);
            response->print("  !\n");

            nDevices++;
        } else if (error == 4) {
            response->print("Unknown error at address 0x");
            if (address < 16) response->print("0");
            response->printf("%x", address);
            response->print("\n");
        }

        // This delay allows for real-time streaming output to the client.
        delay(50); // Optional: Can reduce this if needed.
    }

    if (nDevices == 0) {
        response->print("No I2C devices found\n");
    } else {
        response->print("done\n");
    }

    // Send the response
    request->send(response); });

  // events stuff
  events.onConnect([](AsyncEventSourceClient *client)
                   { client->send("hi", NULL, millis(), 1000); });
  registerFileEditor();
  setup_httpupdate();
  setupUpdateServer();
  wifi_server();
  registerConfigEditor();
  /*server.onFileUpload([](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
    if(!index)
      Serial.printf("UploadStart: %s\n", filename.c_str());
    Serial.printf("%s", (const char*)data);
    if(final)
      Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index+len);
  });*/
  /*
  server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if(!index)
      Serial.printf("BodyStart: %u\n", total);
    Serial.printf("%s", (const char*)data);
    if(index + len == total)
      Serial.printf("BodyEnd: %u\n", total);
  });*/
  server.addHandler(&events);
  server.begin();
}
