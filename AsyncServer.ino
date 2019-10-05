#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"
#include "AsyncJson.h"
#include "ArduinoJson.h"

#include "Secrets.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
int t;
uint8_t dmx[8];

// AsyncEventSource events("/events");

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){

  if(type == WS_EVT_CONNECT){

    Serial.println("Websocket client connection received");
    client->text("Hello from ESP32 Server");

  } else if(type == WS_EVT_DISCONNECT){
    Serial.println("Client disconnected");

  } else if (type == WS_EVT_DATA) {
    Serial.println("ws data received!");
    Serial.println((char*)data);
    Serial.println();
  }
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(WiFiName, WiFiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    AsyncResponseStream *response = request->beginResponseStream("application/json");
    StaticJsonDocument<200> doc;
    JsonArray dmxDoc = doc.createNestedArray("dmx");
    for (uint8_t & d : dmx) {
      dmxDoc.add(d);
    }
    // request->send(200, "application/json", doc);

    serializeJson(doc, *response);
  });

  server.on("/count", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(t++));
  });


   ws.onEvent(onWsEvent);
   server.addHandler(&ws);

  server.begin();
}

void loop(){}
