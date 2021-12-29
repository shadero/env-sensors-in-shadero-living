#include <BlueDot_BME280.h>

#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "SHT31.h"
#include <Arduino_JSON.h>

#define SHT31_ADDRESS   0x44

// Wi-Fi設定
const char *ssid = "YOUR_SSID";
const char *password = "YOUR_PWD";
// IP設定
IPAddress ip(192, 168, 0, 65);
ESP8266WebServer server(80);

SHT31 sht;

void setup() {
  // SHT31のセットアップ
  Wire.begin(0, 2);
  sht.begin(SHT31_ADDRESS);
  Wire.setClock(100000);
  Serial.println("SHT31 start.");

  // WiFiネットワーク接続
  Serial.print("Connecting to ");
  Serial.println(ssid); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // 固定IPアドレスの設定
  WiFi.config(ip, WiFi.gatewayIP(), WiFi.subnetMask());
  Serial.println(WiFi.localIP());

  // WEBサーバー開始
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("Server started");
}

void handle_OnConnect() {
  sht.read();
  JSONVar doc;
  doc["temperature"] = sht.getTemperature();
  doc["humidity"] = sht.getHumidity();
  server.send(200, "application/json", JSON.stringify(doc));
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}

void loop() {
  server.handleClient();
}
