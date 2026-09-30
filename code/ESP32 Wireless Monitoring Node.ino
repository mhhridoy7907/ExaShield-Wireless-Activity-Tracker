// ExaShield ESP32 Wireless Monitoring Node

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "esp_wifi.h"

#define NODE_ID 1

#define BLUE_LED_PIN 2
#define RED_LED_PIN 4

const char* FIREBASE_URL =  "https://hri*****************************************************e.app";


const unsigned long UPLOAD_INTERVAL = 5000;
const unsigned long HEARTBEAT_INTERVAL = 15000;
const unsigned long DEVICE_TIMEOUT = 15000;

#define MAX_DEVICES 40

struct DeviceInfo {
  uint32_t id;
  uint32_t packets;
  unsigned long lastSeen;
};

DeviceInfo devices[MAX_DEVICES];

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
volatile uint32_t totalPackets = 0;

unsigned long lastUpload = 0;
unsigned long lastHeartbeat = 0;
unsigned long lastLedBlink = 0;

bool blueState = false;
bool redState = false;

void blueOn() {
  digitalWrite(BLUE_LED_PIN, HIGH);
  blueState = true;
}

void blueOff() {
  digitalWrite(BLUE_LED_PIN, LOW);
  blueState = false;
}

void redOn() {
  digitalWrite(RED_LED_PIN, HIGH);
  redState = true;
}

void redOff() {
  digitalWrite(RED_LED_PIN, LOW);
  redState = false;
}

void blueBlink(int count, int delayTime) {
  for (int i = 0; i < count; i++) {
    blueOn();
    delay(delayTime);
    blueOff();
    delay(delayTime);
  }
}

void redBlink(int count, int delayTime) {
  for (int i = 0; i < count; i++) {
    redOn();
    delay(delayTime);
    redOff();
    delay(delayTime);
  }
}

void startupLED() {

  blueOff();
  redOff();

  for (int i = 0; i < 3; i++) {

    blueOn();
    redOn();

    delay(120);

    blueOff();
    redOff();

    delay(120);
  }
}

void onlineLED() {

  blueOn();
  redOff();
}

void offlineLED() {

  blueOff();
  redOn();
}

uint32_t hashMAC(const uint8_t* mac) {

  uint32_t hash = 2166136261UL;

  for (int i = 0; i < 6; i++) {
    hash ^= mac[i];
    hash *= 16777619UL;
  }

  return hash;
}

void addDevice(uint32_t deviceId) {

  unsigned long now = millis();

  portENTER_CRITICAL_ISR(&mux);

  totalPackets++;

  int freeSlot = -1;
  int oldestSlot = 0;

  for (int i = 0; i < MAX_DEVICES; i++) {

    if (devices[i].id == deviceId) {

      devices[i].packets++;
      devices[i].lastSeen = now;

      portEXIT_CRITICAL_ISR(&mux);
      return;
    }

    if (
      devices[i].id == 0 &&
      freeSlot == -1
    ) {
      freeSlot = i;
    }

    if (
      devices[i].lastSeen <
      devices[oldestSlot].lastSeen
    ) {
      oldestSlot = i;
    }
  }

  int slot = freeSlot;

  if (slot == -1) {
    slot = oldestSlot;
  }

  devices[slot].id = deviceId;
  devices[slot].packets = 1;
  devices[slot].lastSeen = now;

  portEXIT_CRITICAL_ISR(&mux);
}

void IRAM_ATTR wifiSnifferCallback(
  void* buffer,
  wifi_promiscuous_pkt_type_t type
) {

  if (buffer == nullptr) {
    return;
  }

  if (
    type != WIFI_PKT_MGMT &&
    type != WIFI_PKT_DATA &&
    type != WIFI_PKT_CTRL
  ) {
    return;
  }

  wifi_promiscuous_pkt_t* packet =
    (wifi_promiscuous_pkt_t*)buffer;

  uint8_t* payload =
    packet->payload;

  uint8_t mac[6];

  memcpy(
    mac,
    payload + 10,
    6
  );

  uint32_t deviceId =
    hashMAC(mac);

  if (deviceId == 0) {
    deviceId = 1;
  }

  addDevice(deviceId);
}

void clearDevices() {

  portENTER_CRITICAL(&mux);

  for (int i = 0; i < MAX_DEVICES; i++) {

    devices[i].id = 0;
    devices[i].packets = 0;
    devices[i].lastSeen = 0;
  }

  totalPackets = 0;

  portEXIT_CRITICAL(&mux);
}

void startWirelessMonitor() {

  esp_wifi_set_promiscuous(false);

  esp_wifi_set_promiscuous_rx_cb(
    wifiSnifferCallback
  );

  esp_wifi_set_promiscuous(true);

  Serial.println(
    "Wireless monitor started."
  );
}

void stopWirelessMonitor() {

  esp_wifi_set_promiscuous(false);

  Serial.println(
    "Wireless monitor stopped."
  );
}

bool connectWiFi() {

  WiFi.mode(WIFI_STA);

  WiFi.setAutoReconnect(true);
  WiFi.persistent(false);

  WiFiManager wm;

  const char* AP_NAME =
    "ExaShield-Setup-1";

  wm.setConfigPortalTimeout(300);

  Serial.println();
  Serial.println(
    "================================"
  );
  Serial.println(
    "        WIFI SETUP MODE"
  );
  Serial.println(
    "================================"
  );

  Serial.print(
    "Connect to: "
  );

  Serial.println(AP_NAME);

  Serial.println(
    "Open 192.168.4.1"
  );

  Serial.println();

  bool result =
    wm.startConfigPortal(
      AP_NAME
    );

  if (!result) {

    Serial.println(
      "WiFi configuration failed."
    );

    return false;
  }

  Serial.println();
  Serial.println(
    "================================"
  );
  Serial.println(
    "         WIFI CONNECTED"
  );
  Serial.println(
    "================================"
  );

  Serial.print(
    "SSID: "
  );

  Serial.println(
    WiFi.SSID()
  );

  Serial.print(
    "IP: "
  );

  Serial.println(
    WiFi.localIP()
  );

  Serial.print(
    "Channel: "
  );

  Serial.println(
    WiFi.channel()
  );

  Serial.print(
    "RSSI: "
  );

  Serial.println(
    WiFi.RSSI()
  );

  Serial.println(
    "================================"
  );

  return true;
}

String firebaseURL(
  const String& path
) {

  return String(FIREBASE_URL) +
         path +
         ".json";
}

bool firebasePUT(
  const String& path,
  const String& json
) {

  if (
    WiFi.status() !=
    WL_CONNECTED
  ) {
    return false;
  }

  WiFiClientSecure client;

  client.setInsecure();

  HTTPClient http;

  String url =
    firebaseURL(path);

  if (
    !http.begin(
      client,
      url
    )
  ) {
    return false;
  }

  http.setTimeout(10000);

  http.addHeader(
    "Content-Type",
    "application/json"
  );

  int response =
    http.PUT(json);

  http.end();

  return (
    response >= 200 &&
    response < 300
  );
}

bool firebasePOST(
  const String& path,
  const String& json
) {

  if (
    WiFi.status() !=
    WL_CONNECTED
  ) {
    return false;
  }

  WiFiClientSecure client;

  client.setInsecure();

  HTTPClient http;

  String url =
    firebaseURL(path);

  if (
    !http.begin(
      client,
      url
    )
  ) {
    return false;
  }

  http.setTimeout(10000);

  http.addHeader(
    "Content-Type",
    "application/json"
  );

  int response =
    http.POST(json);

  http.end();

  return (
    response >= 200 &&
    response < 300
  );
}

uint32_t getTotalPackets() {

  uint32_t value;

  portENTER_CRITICAL(&mux);

  value = totalPackets;

  portEXIT_CRITICAL(&mux);

  return value;
}

String getLevel(
  uint32_t packets
) {

  if (packets >= 500) {
    return "HIGH";
  }

  if (packets >= 200) {
    return "MEDIUM";
  }

  return "LOW";
}

void uploadDevices() {

  String node =
    "node" +
    String(NODE_ID);

  String json = "{";

  uint32_t total = 0;
  int count = 0;

  unsigned long now =
    millis();

  portENTER_CRITICAL(&mux);

  for (
    int i = 0;
    i < MAX_DEVICES;
    i++
  ) {

    if (
      devices[i].id == 0
    ) {
      continue;
    }

    if (
      now -
      devices[i].lastSeen >
      DEVICE_TIMEOUT
    ) {
      continue;
    }

    total +=
      devices[i].packets;

    count++;
  }

  portEXIT_CRITICAL(&mux);

  json +=
    "\"nodeId\":" +
    String(NODE_ID);

  json +=
    ",\"deviceCount\":" +
    String(count);

  json +=
    ",\"totalPackets\":" +
    String(total);

  json +=
    ",\"timestamp\":{\".sv\":\"timestamp\"}";

  json += "}";

  firebasePUT(
    "/sensors/" +
    node,
    json
  );

  for (
    int i = 0;
    i < MAX_DEVICES;
    i++
  ) {

    uint32_t id;
    uint32_t packets;
    unsigned long lastSeen;

    portENTER_CRITICAL(&mux);

    id =
      devices[i].id;

    packets =
      devices[i].packets;

    lastSeen =
      devices[i].lastSeen;

    portEXIT_CRITICAL(&mux);

    if (id == 0) {
      continue;
    }

    if (
      millis() -
      lastSeen >
      DEVICE_TIMEOUT
    ) {
      continue;
    }

    String deviceKey =
      "device" +
      String(i + 1);

    String deviceJson = "{";

    deviceJson +=
      "\"deviceNumber\":" +
      String(i + 1);

    deviceJson +=
      ",\"packets\":" +
      String(packets);

    deviceJson +=
      ",\"level\":\"" +
      getLevel(packets) +
      "\"";

    deviceJson +=
      ",\"lastSeen\":{\".sv\":\"timestamp\"}";

    deviceJson += "}";

    firebasePUT(
      "/devices/" +
      node +
      "/" +
      deviceKey,
      deviceJson
    );
  }

  String eventJson = "{";

  eventJson +=
    "\"nodeId\":" +
    String(NODE_ID);

  eventJson +=
    ",\"deviceCount\":" +
    String(count);

  eventJson +=
    ",\"totalPackets\":" +
    String(total);

  eventJson +=
    ",\"channel\":" +
    String(WiFi.channel());

  eventJson +=
    ",\"rssi\":" +
    String(WiFi.RSSI());

  eventJson +=
    ",\"timestamp\":{\".sv\":\"timestamp\"}";

  eventJson += "}";

  firebasePOST(
    "/events",
    eventJson
  );
}

void uploadHeartbeat() {

  String node =
    "node" +
    String(NODE_ID);

  String json = "{";

  json +=
    "\"nodeId\":" +
    String(NODE_ID);

  json +=
    ",\"status\":\"ONLINE\"";

  json +=
    ",\"ip\":\"" +
    WiFi.localIP().toString() +
    "\"";

  json +=
    ",\"channel\":" +
    String(WiFi.channel());

  json +=
    ",\"rssi\":" +
    String(WiFi.RSSI());

  json +=
    ",\"uptime\":" +
    String(
      millis() / 1000
    );

  json +=
    ",\"lastSeen\":{\".sv\":\"timestamp\"}";

  json += "}";

  firebasePUT(
    "/nodes/" +
    node,
    json
  );
}

void setup() {

  Serial.begin(115200);

  delay(1500);

  pinMode(
    BLUE_LED_PIN,
    OUTPUT
  );

  pinMode(
    RED_LED_PIN,
    OUTPUT
  );

  blueOff();
  redOff();

  clearDevices();

  Serial.println();
  Serial.println(
    "================================"
  );
  Serial.println(
    "          EXASHIELD NODE"
  );
  Serial.println(
    "================================"
  );

  Serial.println(
    "ESP32 starting..."
  );

  startupLED();

  if (!connectWiFi()) {

    offlineLED();

    redBlink(
      5,
      150
    );

    delay(3000);

    ESP.restart();
  }

  onlineLED();

  uploadHeartbeat();

  delay(500);

  startWirelessMonitor();

  Serial.println();
  Serial.println(
    "================================"
  );
  Serial.println(
    "        EXASHIELD ONLINE"
  );
  Serial.println(
    "================================"
  );

  lastUpload =
    millis();

  lastHeartbeat =
    millis();
}

void loop() {

  if (
    WiFi.status() !=
    WL_CONNECTED
  ) {

    stopWirelessMonitor();

    offlineLED();

    delay(1000);

    if (connectWiFi()) {

      onlineLED();

      uploadHeartbeat();

      delay(500);

      startWirelessMonitor();

    } else {

      offlineLED();
    }
  }

  if (
    millis() -
    lastUpload >=
    UPLOAD_INTERVAL
  ) {

    lastUpload =
      millis();

    Serial.println();
    Serial.println(
      "------------------------------"
    );

    Serial.print(
      "Devices: "
    );

    int count = 0;

    unsigned long now =
      millis();

    portENTER_CRITICAL(&mux);

    for (
      int i = 0;
      i < MAX_DEVICES;
      i++
    ) {

      if (
        devices[i].id != 0 &&
        now -
        devices[i].lastSeen <=
        DEVICE_TIMEOUT
      ) {
        count++;
      }
    }

    portEXIT_CRITICAL(&mux);

    Serial.println(count);

    for (
      int i = 0;
      i < MAX_DEVICES;
      i++
    ) {

      uint32_t packets;
      uint32_t id;
      unsigned long lastSeen;

      portENTER_CRITICAL(&mux);

      id =
        devices[i].id;

      packets =
        devices[i].packets;

      lastSeen =
        devices[i].lastSeen;

      portEXIT_CRITICAL(&mux);

      if (id == 0) {
        continue;
      }

      if (
        millis() -
        lastSeen >
        DEVICE_TIMEOUT
      ) {
        continue;
      }

      Serial.print(
        "Device "
      );

      Serial.print(
        i + 1
      );

      Serial.print(
        " | "
      );

      Serial.print(
        packets
      );

      Serial.print(
        " packets | "
      );

      Serial.println(
        getLevel(packets)
      );
    }

    Serial.println(
      "------------------------------"
    );

    uploadDevices();

    blueBlink(
      1,
      60
    );

    blueOn();
  }

  if (
    millis() -
    lastHeartbeat >=
    HEARTBEAT_INTERVAL
  ) {

    lastHeartbeat =
      millis();

    uploadHeartbeat();

    blueOn();
  }

  delay(20);
}
