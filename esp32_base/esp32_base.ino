#include <Arduino_JSON.h>
#include <WiFi.h>
#include <HTTPClient.h>


//wifi
char* ssid = "7702knuprebyC";
char* password = "3>ciM6Q#~_";
IPAddress ip;

void setup() {
  Serial.begin(115200);
  wifi();
  delay(1000);

}

void loop() {
  json();
}


void wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting...");
    delay (1000);
  }
  while(1) {
    if (WiFi.status() == WL_CONNECTED) {
        ip = WiFi.localIP();
        Serial.println(ip);
        break;
    }
  }
}

void json() {
  HTTPClient http;
  http.begin("http://ip.jsontest.com/?callback=someFunction");
  int httpCode = http.GET();
  String payload = http.getString();
  Serial.println(payload);
  http.end();
  delay(30000);
}
