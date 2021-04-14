#include <Arduino_JSON.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "WiFiCredentials.h"

//temp_sens
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 15
#define BCOEFFICIENT 3950
#define SERIESRESISTOR 10000
#define ledPin 32
#define tempPin 34
#define wlPin 35
#define motorPin 33
#define heaterPin 22

int wl_sens = 0;

float average;

int measure_temp();
int measure_wl();

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(heaterPin, OUTPUT);
  Serial.begin(115200);
  wifi();
  delay(1000);  
}

void loop() {
  int temp_s  = measure_temp();
  int wl_s = measure_wl();
  if (temp_s < 18) low_temp();
  else heater_off();
  if (wl_s < 10) low_wl();
  else wl_ok();
  put_sens(temp_s, wl_s);
  get_motorset();
  Serial.println(" ");
  Serial.println(" ");  
  delay(15000);
}

void wifi() {
  WiFi.begin(CUSTOM_SSID, CUSTOM_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting...");
    delay (1000);
  }
  while(1) {
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println(WiFi.localIP());
        break;
    }
  }
}

void get_motorset() {
  HTTPClient http;
  http.begin("https://kokoshkite.pythonanywhere.com/notifications");
  int httpCode = http.GET();
  String payload = http.getString();
  Serial.println(payload);
  JSONVar myObject = JSON.parse(payload);
  Serial.println(myObject["is_motor_on"]);
  String value = JSON.stringify(myObject["is_motor_on"]);
  if (value == "true") {
      digitalWrite(motorPin, HIGH);
      Serial.println("The motor is on!");  
  }
  else digitalWrite(motorPin, LOW);
  http.end();
  delay(1000);  

}
void put_sens(int temp_s, int wl_s) {
  HTTPClient http;
  http.begin("https://kokoshkite.pythonanywhere.com/sensors");
  JSONVar myObject;
  myObject["water_level"] = wl_s;
  myObject["temperature"] = temp_s;
  String data_t = JSON.stringify(myObject);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.PUT(data_t);
  Serial.println(http.getString());
  http.end();
  delay(1000);
}


int measure_temp() {
    average = analogRead(tempPin);
    average = 4095 / average - 1;
    average = SERIESRESISTOR / average;
    float temperature;
    temperature = average / THERMISTORNOMINAL; 
    temperature = log(temperature);
    temperature /= BCOEFFICIENT;
    temperature += 1.0 / (TEMPERATURENOMINAL + 273.15); 
    temperature = 1.0 / temperature;
    temperature -= 273.15;  
    return (temperature);
}

void low_temp() { 
  digitalWrite(ledPin, HIGH); 
  digitalWrite(heaterPin, HIGH);
  //Serial.println("The temperature is low!"); 
  Serial.println("The heater is on!");
  
  HTTPClient http;
  http.begin("https://kokoshkite.pythonanywhere.com/notifications");
  JSONVar myObject;
  myObject["is_heater_on"] = "True";
  String data_t = JSON.stringify(myObject);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.PUT(data_t);
  //Serial.println(http.getString());
  http.end();
  delay(1000);
  
}

int measure_wl() {
  wl_sens = analogRead(wlPin) / 100;
  return (wl_sens);
}

void low_wl() {
  Serial.println("The water level is low!");
  HTTPClient http;
  http.begin("https://kokoshkite.pythonanywhere.com/notifications");
  JSONVar myObject;
  myObject["has_not_water"] = "True";
  String data_t = JSON.stringify(myObject);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.PUT(data_t);
  //Serial.println(http.getString());
  http.end();
  delay(1000);
}

void heater_off() {
  digitalWrite(ledPin, LOW);
  digitalWrite(heaterPin, LOW);
  HTTPClient http;
  http.begin("https://kokoshkite.pythonanywhere.com/notifications");
  JSONVar myObject;
  myObject["is_heater_on"] = "False";
  String data_t = JSON.stringify(myObject);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.PUT(data_t);
  //Serial.println(http.getString());
  http.end();
  delay(1000);
}

void wl_ok() {
  HTTPClient http;
  http.begin("https://kokoshkite.pythonanywhere.com/notifications");
  JSONVar myObject;
  myObject["has_not_water"] = "False";
  String data_t = JSON.stringify(myObject);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.PUT(data_t);
  //Serial.println(http.getString());
  http.end();
  delay(1000);
}
