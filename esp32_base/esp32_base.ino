#include <Arduino_JSON.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include "WiFiCredentials.h"

//temp_sens
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 21
#define BCOEFFICIENT 3950
#define SERIESRESISTOR 10000
#define ledPin 32
#define tempPin 33
#define wlPin 35
#define motorPin 34
#define heaterPin 22


int wl_sens = 0;

float average;

//wifi
IPAddress ip;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(heaterPin, OUTPUT);
  Serial.begin(115200);
  wifi();
  delay(1000);  
}

void loop() {
  putjs();
  getjs();
/*  int temp_s  = measure_temp();
  int wl_s = measure_wl();
  if (temp_s < 24) low_temp();
  else digitalWrite(ledPin, LOW);
  if (wl_s < 20) low_wl();
  Serial.println(" ");
  Serial.println(" ");
  */
  delay(1000);
}


void wifi() {
  WiFi.begin(CUSTOM_SSID, CUSTOM_PASSWORD);
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

void getjs() {
  HTTPClient http;
  http.begin("https://kokoshkite.pythonanywhere.com/sensors");
  int httpCode = http.GET();
  String payload = http.getString();
  Serial.println(payload);
  JSONVar myObject = JSON.parse(payload);
  Serial.println(int(myObject["water_level"])+1);
  http.end();
  delay(1000);
}

void putjs() {
  HTTPClient http;
  http.begin("https://kokoshkite.pythonanywhere.com/sensors");
  //String payload = http.getString();
  JSONVar myObject;
  myObject["water_level"] = 50;
  myObject["temperature"] = 25;
  String data_t = JSON.stringify(myObject);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.PUT(data_t);
  Serial.println(http.getString());
  
  //Serial.println(payload);
  //JSONVar myObject = JSON.parse(payload);
  //Serial.println(int(myObject["water_level"])+1);
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
    Serial.print("Temperature: "); 
    Serial.print(temperature);
    Serial.println("°C");   
    return (temperature);
}

void low_temp() { 
  digitalWrite(ledPin, HIGH); //turn on the led
  Serial.println("The temperature is low!"); //send notification
  //turn on heater 
}

int measure_wl() {
  wl_sens = analogRead(wlPin) / 100;
  Serial.print("Water Level: ");
  Serial.println(wl_sens);
  return (wl_sens);
}

void low_wl() {
  Serial.println("The water level is low!");
  //digitalWrite(ledPin, HIGH);
}

void motor() {
  digitalWrite(motorPin, HIGH);
  Serial.println("The motor is on!");  
}

void heater() {
  digitalWrite(heaterPin, HIGH);
  Serial.println("The heater is on!");
  
}
