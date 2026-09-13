//=====main working code======//


#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "esp_wifi.h"

#define NODE_ID 1
#define LED_BUILTIN 2


const char* FIREBASE_URL = "https://e***********************base.app";


const unsigned long WIFI_TIMEOUT = 30000;
const unsigned long UPLOAD_INTERVAL = 5000;
const unsigned long HEARTBEAT_INTERVAL = 30000;

const uint16_t ALERT_THRESHOLD = 500;

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


void IRAM_ATTR wifiSnifferCallback(
  void* buffer,
  wifi_promiscuous_pkt_type_t type
) {

  if (buffer == nullptr) {
    return;
  }

  if (
    type == WIFI_PKT_MGMT ||
    type == WIFI_PKT_DATA
  ) {

    wirelessPackets++;
  }
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

  Serial.println();
  Serial.println(
    "================================"
  );

  Serial.println(
    "      WIFI SETUP MODE"
  );

  Serial.println(
    "================================"
  );

  WiFi.mode(WIFI_STA);

  WiFi.setAutoReconnect(true);

  WiFi.persistent(false);


  WiFiManager wm;

  wm.resetSettings();


  const char* AP_NAME =
    "ExaShield-Setup-1";

  Serial.println(
    "Starting configuration portal..."
  );

  Serial.print(
    "Setup WiFi: "
  );

  Serial.println(
    AP_NAME
  );

  Serial.println();
  Serial.println(
    "Connect phone to:"
  );

  Serial.println(
    "ExaShield-Setup-1"
  );

  Serial.println();

  Serial.println(
    "Then open:"
  );

  Serial.println(
    "192.168.4.1"
  );

  Serial.println();


  wm.setConfigPortalTimeout(300);

  bool result =
    wm.startConfigPortal(
      AP_NAME
    );

  if (!result) {

    Serial.println();
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
    "       WIFI CONNECTED"
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
    "Gateway: "
  );

  Serial.println(
    WiFi.gatewayIP()
  );

  Serial.print(
    "RSSI: "
  );

  Serial.println(
    WiFi.RSSI()
  );

  Serial.print(
    "Channel: "
  );

  Serial.println(
    WiFi.channel()
  );

  Serial.print(
    "MAC: "
  );

  Serial.println(
    WiFi.macAddress()
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
    WiFi.status() != WL_CONNECTED
  ) {

    Serial.println(
      "Firebase PUT: WiFi offline"
    );

    return false;
  }

  WiFiClientSecure client;

  client.setInsecure();

  HTTPClient http;

  String url =
    firebaseURL(path);

  if (!http.begin(client, url)) {

    Serial.println(
      "Firebase HTTP begin failed."
    );

    return false;
  }

  http.setTimeout(10000);

  http.addHeader(
    "Content-Type",
    "application/json"
  );

  int response =
    http.PUT(json);

  Serial.print(
    "Firebase PUT: "
  );

  Serial.println(
    response
  );

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
    WiFi.status() != WL_CONNECTED
  ) {

    Serial.println(
      "Firebase POST: WiFi offline"
    );

    return false;
  }

  WiFiClientSecure client;

  client.setInsecure();

  HTTPClient http;

  String url =
    firebaseURL(path);

  if (!http.begin(client, url)) {

    Serial.println(
      "Firebase HTTP begin failed."
    );

    return false;
  }

  http.setTimeout(10000);

  http.addHeader(
    "Content-Type",
    "application/json"
  );

  int response =
    http.POST(json);

  Serial.print(
    "Firebase POST: "
  );

  Serial.println(
    response
  );

  http.end();

  return (
    response >= 200 &&
    response < 300
  );
}



uint32_t getActivity() {

  uint32_t packets;

  noInterrupts();

  packets =
    wirelessPackets;

  wirelessPackets = 0;

  interrupts();

  return packets;
}



String getActivityLevel(
  uint32_t activity
) {

  if (activity >= 500) {

    return "HIGH";
  }

  if (activity >= 200) {

    return "MEDIUM";
  }

  return "LOW";
}



void uploadSensorStatus(
  uint32_t activity
) {

  String node =
    "node" +
    String(NODE_ID);

  String level =
    getActivityLevel(activity);

  String json =
    "{"
    "\"nodeId\":" +
    String(NODE_ID) +
    ","
    "\"status\":\"ONLINE\","
    "\"activity\":" +
    String(activity) +
    ","
    "\"packets\":" +
    String(activity) +
    ","
    "\"level\":\"" +
    level +
    "\","
    "\"channel\":" +
    String(WiFi.channel()) +
    ","
    "\"wifiRssi\":" +
    String(WiFi.RSSI()) +
    ","
    "\"uptime\":" +
    String(millis() / 1000) +
    ","
    "\"lastSeen\":{\".sv\":\"timestamp\"}"
    "}";

  firebasePUT(
    "/sensors/" +
    node,
    json
  );
}



void uploadEvent(
  uint32_t activity
) {

  String level =
    getActivityLevel(activity);

  String json =
    "{"
    "\"nodeId\":" +
    String(NODE_ID) +
    ","
    "\"activity\":" +
    String(activity) +
    ","
    "\"packets\":" +
    String(activity) +
    ","
    "\"level\":\"" +
    level +
    "\","
    "\"channel\":" +
    String(WiFi.channel()) +
    ","
    "\"wifiRssi\":" +
    String(WiFi.RSSI()) +
    ","
    "\"timestamp\":{\".sv\":\"timestamp\"}"
    "}";

  firebasePOST(
    "/events",
    json
  );
}



void uploadAlert(
  uint32_t activity
) {

  String json =
    "{"
    "\"nodeId\":" +
    String(NODE_ID) +
    ","
    "\"activity\":" +
    String(activity) +
    ","
    "\"packets\":" +
    String(activity) +
    ","
    "\"level\":\"HIGH\","
    "\"verification\":\"PENDING\","
    "\"message\":\"High wireless activity detected. Human verification required.\","
    "\"channel\":" +
    String(WiFi.channel()) +
    ","
    "\"wifiRssi\":" +
    String(WiFi.RSSI()) +
    ","
    "\"timestamp\":{\".sv\":\"timestamp\"}"
    "}";

  firebasePOST(
    "/alerts",
    json
  );
}


void uploadHeartbeat() {

  String node =
    "node" +
    String(NODE_ID);

  String json =
    "{"
    "\"nodeId\":" +
    String(NODE_ID) +
    ","
    "\"status\":\"ONLINE\","
    "\"ip\":\"" +
    WiFi.localIP().toString() +
    "\","
    "\"channel\":" +
    String(WiFi.channel()) +
    ","
    "\"rssi\":" +
    String(WiFi.RSSI()) +
    ","
    "\"lastSeen\":{\".sv\":\"timestamp\"}"
    "}";

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
    LED_BUILTIN,
    OUTPUT
  );

  ledOff();

  Serial.println();
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


  ledBlink(
    3,
    200
  );

  if (!connectWiFi()) {

    Serial.println();

    Serial.println(
      "WiFi setup failed."
    );

    ledBlink(
      5,
      150
    );

    delay(3000);

    ESP.restart();
  }

  ledOn();

  Serial.println();

  Serial.println(
    "Sending Firebase heartbeat..."
  );

  uploadHeartbeat();

  delay(500);

  startWirelessMonitor();

  Serial.println();

  Serial.println(
    "================================"
  );

  Serial.println(
    "       EXASHIELD ONLINE"
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
    WiFi.status() != WL_CONNECTED
  ) {

    Serial.println();

    Serial.println(
      "WiFi disconnected!"
    );

    ledOff();

    stopWirelessMonitor();

    delay(1000);
    if (connectWiFi()) {

      ledOn();

      uploadHeartbeat();

      delay(500);

      startWirelessMonitor();

      Serial.println(
        "WiFi reconnected."
      );
    }
  }



  if (
    millis() -
    lastUpload >=
    UPLOAD_INTERVAL
  ) {

    lastUpload =
      millis();

    uint32_t activity =
      getActivity();

    String level =
      getActivityLevel(
        activity
      );

    Serial.println();

    Serial.println(
      "------------------------------"
    );

    Serial.print(
      "Activity: "
    );

    Serial.println(
      activity
    );

    Serial.print(
      "Level: "
    );

    Serial.println(
      level
    );

    Serial.print(
      "RSSI: "
    );

    Serial.println(
      WiFi.RSSI()
    );

    Serial.print(
      "Channel: "
    );

    Serial.println(
      WiFi.channel()
    );

    Serial.println(
      "------------------------------"
    );



    uploadSensorStatus(
      activity
    );


    uploadEvent(
      activity
    );



    if (
      activity >=
      ALERT_THRESHOLD
    ) {

      Serial.println();

      Serial.println(
        "HIGH wireless activity detected!"
      );

      uploadAlert(
        activity
      );

      ledBlink(
        3,
        120
      );

      ledOn();
    }
  }



  if (
    millis() -
    lastHeartbeat >=
    HEARTBEAT_INTERVAL
  ) {

    lastHeartbeat =
      millis();

    uploadHeartbeat();
  }

  delay(20);
}