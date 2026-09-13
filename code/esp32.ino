#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "esp_wifi.h"

#define NODE_ID 1
#define LED_BUILTIN 13

const char* FIREBASE_URL = "http***********************se.app";

const uint16_t ALERT_THRESHOLD = 500;
const unsigned long UPLOAD_INTERVAL = 5000;
const unsigned long HEARTBEAT_INTERVAL = 30000;

volatile uint32_t wirelessPackets = 0;

unsigned long lastUpload = 0;
unsigned long lastHeartbeat = 0;

void ledOn() {
  digitalWrite(LED_BUILTIN, HIGH);
}

void ledOff() {
  digitalWrite(LED_BUILTIN, LOW);
}

void ledBlink(int count, int delayTime) {
  for (int i = 0; i < count; i++) {
    ledOn();
    delay(delayTime);
    ledOff();
    delay(delayTime);
  }
}

void IRAM_ATTR wifiSnifferCallback(void* buffer, wifi_promiscuous_pkt_type_t type) {
  if (buffer == nullptr) return;

  if (type == WIFI_PKT_MGMT || type == WIFI_PKT_DATA) {
    wirelessPackets++;
  }
}

void startWirelessMonitor() {
  esp_wifi_set_promiscuous(false);
  esp_wifi_set_promiscuous_rx_cb(wifiSnifferCallback);
  esp_wifi_set_promiscuous(true);
}

void stopWirelessMonitor() {
  esp_wifi_set_promiscuous(false);
}

bool connectWiFi() {
  WiFiManager wm;

  String apName = "ExaShield-Setup-" + String(NODE_ID);

  bool result = wm.autoConnect(apName.c_str());

  if (!result) return false;

  return true;
}

String firebaseURL(const String& path) {
  return String(FIREBASE_URL) + path + ".json";
}

bool firebasePUT(const String& path, const String& json) {
  if (WiFi.status() != WL_CONNECTED) return false;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  String url = firebaseURL(path);

  if (!http.begin(client, url)) return false;

  http.addHeader("Content-Type", "application/json");

  int response = http.PUT(json);

  http.end();

  return response >= 200 && response < 300;
}

bool firebasePOST(const String& path, const String& json) {
  if (WiFi.status() != WL_CONNECTED) return false;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  String url = firebaseURL(path);

  if (!http.begin(client, url)) return false;

  http.addHeader("Content-Type", "application/json");

  int response = http.POST(json);

  http.end();

  return response >= 200 && response < 300;
}

uint32_t getActivity() {
  uint32_t packets;

  noInterrupts();
  packets = wirelessPackets;
  wirelessPackets = 0;
  interrupts();

  return packets;
}

String getActivityLevel(uint32_t activity) {
  if (activity >= 500) return "HIGH";
  if (activity >= 200) return "MEDIUM";
  return "LOW";
}

void uploadSensorStatus(uint32_t activity) {
  String node = "node" + String(NODE_ID);
  String level = getActivityLevel(activity);

  String json =
    "{"
      "\"nodeId\":" + String(NODE_ID) + ","
      "\"status\":\"ONLINE\","
      "\"activity\":" + String(activity) + ","
      "\"packets\":" + String(activity) + ","
      "\"level\":\"" + level + "\","
      "\"channel\":" + String(WiFi.channel()) + ","
      "\"wifiRssi\":" + String(WiFi.RSSI()) + ","
      "\"uptime\":" + String(millis() / 1000) + ","
      "\"lastSeen\":{\".sv\":\"timestamp\"}"
    "}";

  firebasePUT("/sensors/" + node, json);
}

void uploadEvent(uint32_t activity) {
  String level = getActivityLevel(activity);

  String json =
    "{"
      "\"nodeId\":" + String(NODE_ID) + ","
      "\"activity\":" + String(activity) + ","
      "\"packets\":" + String(activity) + ","
      "\"level\":\"" + level + "\","
      "\"channel\":" + String(WiFi.channel()) + ","
      "\"wifiRssi\":" + String(WiFi.RSSI()) + ","
      "\"timestamp\":{\".sv\":\"timestamp\"}"
    "}";

  firebasePOST("/events", json);
}

void uploadAlert(uint32_t activity) {
  String json =
    "{"
      "\"nodeId\":" + String(NODE_ID) + ","
      "\"activity\":" + String(activity) + ","
      "\"packets\":" + String(activity) + ","
      "\"level\":\"HIGH\","
      "\"verification\":\"PENDING\","
      "\"message\":\"High wireless activity detected. Human verification required.\","
      "\"channel\":" + String(WiFi.channel()) + ","
      "\"wifiRssi\":" + String(WiFi.RSSI()) + ","
      "\"timestamp\":{\".sv\":\"timestamp\"}"
    "}";

  firebasePOST("/alerts", json);
}

void uploadHeartbeat() {
  String node = "node" + String(NODE_ID);

  String json =
    "{"
      "\"nodeId\":" + String(NODE_ID) + ","
      "\"status\":\"ONLINE\","
      "\"ip\":\"" + WiFi.localIP().toString() + "\","
      "\"channel\":" + String(WiFi.channel()) + ","
      "\"rssi\":" + String(WiFi.RSSI()) + ","
      "\"lastSeen\":{\".sv\":\"timestamp\"}"
    "}";

  firebasePUT("/nodes/" + node, json);
}

void setup() {
  Serial.begin(115200);

  delay(1000);

  pinMode(LED_BUILTIN, OUTPUT);

  ledOff();

  if (!connectWiFi()) {
    delay(3000);
    ESP.restart();
  }

  ledOn();

  uploadHeartbeat();

  startWirelessMonitor();

  lastUpload = millis();
  lastHeartbeat = millis();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    ledOff();

    stopWirelessMonitor();

    delay(1000);

    if (connectWiFi()) {
      ledOn();
      startWirelessMonitor();
    }
  }

  if (millis() - lastUpload >= UPLOAD_INTERVAL) {
    lastUpload = millis();

    uint32_t activity = getActivity();

    uploadSensorStatus(activity);
    uploadEvent(activity);

    if (activity >= ALERT_THRESHOLD) {
      uploadAlert(activity);

      ledBlink(3, 120);

      ledOn();
    }
  }

  if (millis() - lastHeartbeat >= HEARTBEAT_INTERVAL) {
    lastHeartbeat = millis();

    uploadHeartbeat();
  }

  delay(20);
}