#include <Arduino_JSON.h>
#include <WiFi.h>
#include <HTTPClient.h>

//temp_sens
#define THERMISTORNOMINAL 10000
#define TEMPERATURENOMINAL 23
#define BCOEFFICIENT 3950
#define SERIESRESISTOR 10000
#define ledPin 32
#define tempPin 33
#define wlPin 35



int wl_sens = 0;

float average;

//wifi
char* ssid = "??";
char* password = "??";
IPAddress ip;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  wifi();
  delay(1000);  
}

void loop() {
  //json();
  int temp_s  = measure_temp();
  int wl_s = measure_wl();
  if (temp_s < 24) low_temp();
  else digitalWrite(ledPin, LOW);
  if (wl_s < 20) low_wl();
  Serial.println(" ");
  Serial.println(" ");
  delay(1000);
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
