#include <WiFi.h>
#include <WiFiClient.h>
//#include <HttpClient.h>
#include <Arduino_JSON.h>
#include <HTTPClient.h>





//wifi
const char* ssid = "7702knuprebyC";
const char* password = "3>ciM6Q#~_";
IPAddress ip;

void setup() {
  Serial.begin(115200);
  wifi();
  delay(2000);
  Serial.println("opa");
//  json(); 
}

void loop() {
  HTTPClient http;
  http.begin("http://ip.jsontest.com/?callback=someFunction");
  int httpCode = http.GET();
  if (httpCode > 0) {
   
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    
  }
  http.end();

  delay(30000);
  
}


void wifi() {
  WiFi.begin(ssid, password);
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connected");  
    delay (1000);
    ip = WiFi.localIP();
    Serial.println(ip);
  } 
}

/*void json() { 
  
  client.get("http://ip.jsontest.com/?callback=someFunction");
  char c = client.read();
  Serial.print(c);
  Serial.print("MAIKATI");
}
*/
