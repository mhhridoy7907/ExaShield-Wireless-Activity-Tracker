/*
  ============================================================
  ExaShield
  ESP32 Wireless Activity Monitoring System
  ============================================================

  Board:
  Arduino Nano ESP32

  Features:
  - Phone-based Wi-Fi configuration
  - Firebase Realtime Database
  - Wireless activity counter
  - Built-in LED status
  - No external LED
  - No buzzer
  - Anonymous activity monitoring

  Firebase:
  Only Realtime Database URL is required.

  IMPORTANT:
  This system reports wireless activity.
  It does NOT identify a student/device and does NOT prove cheating.
*/


#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>

#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "esp_wifi.h"


// ============================================================
// CONFIGURATION
// ============================================================

// Change this for additional ESP32 nodes.
// Node 1 = node1
// Node 2 = node2
// etc.
#define NODE_ID 1


// Arduino Nano ESP32 built-in LED
#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif


// Firebase Realtime Database URL
const char* FIREBASE_URL =
  "https://exashield-dabd4-default-rtdb.asia-southeast1.firebasedatabase.app";


// Activity threshold
// 0-1000
const uint16_t ALERT_THRESHOLD = 500;


// Send data every 5 seconds
const unsigned long UPLOAD_INTERVAL = 5000;


// Heartbeat every 30 seconds
const unsigned long HEARTBEAT_INTERVAL = 30000;


// ============================================================
// GLOBAL VARIABLES
// ============================================================

volatile uint32_t wirelessPackets = 0;

unsigned long lastUpload = 0;

unsigned long lastHeartbeat = 0;


// ============================================================
// LED FUNCTIONS
// ============================================================

void ledOn() {
  digitalWrite(
    LED_BUILTIN,
    HIGH
  );
}


void ledOff() {
  digitalWrite(
    LED_BUILTIN,
    LOW
  );
}


void ledBlink(
  int count,
  int delayTime
) {

  for (
    int i = 0;
    i < count;
    i++
  ) {

    ledOn();

    delay(delayTime);

    ledOff();

    delay(delayTime);
  }
}


// ============================================================
// WIFI PACKET CALLBACK
// ============================================================

void IRAM_ATTR wifiSnifferCallback(
  void* buffer,
  wifi_promiscuous_pkt_type_t type
) {

  if (
    buffer == nullptr
  ) {
    return;
  }


  /*
    Count wireless packet activity only.

    We do NOT store:
    - MAC addresses
    - packet contents
    - SSIDs
    - personal information
  */

  if (
    type == WIFI_PKT_MGMT ||
    type == WIFI_PKT_DATA
  ) {

    wirelessPackets++;
  }
}


// ============================================================
// START WIRELESS MONITOR
// ============================================================

void startWirelessMonitor() {

  /*
    Promiscuous mode observes packet activity
    on the current Wi-Fi channel.
  */

  esp_wifi_set_promiscuous(false);

  esp_wifi_set_promiscuous_rx_cb(
    wifiSnifferCallback
  );

  esp_wifi_set_promiscuous(true);


  Serial.println(
    "Wireless activity monitor started."
  );
}


// ============================================================
// STOP WIRELESS MONITOR
// ============================================================

void stopWirelessMonitor() {

  esp_wifi_set_promiscuous(false);

  Serial.println(
    "Wireless activity monitor stopped."
  );
}


// ============================================================
// CONNECT WIFI THROUGH PHONE
// ============================================================

bool connectWiFi() {

  WiFiManager wm;


  String apName =
    "ExaShield-Setup-" +
    String(NODE_ID);


  Serial.println();
  Serial.println(
    "================================"
  );

  Serial.println(
    "Wi-Fi Configuration"
  );

  Serial.println(
    "================================"
  );


  Serial.print(
    "Setup AP: "
  );

  Serial.println(
    apName
  );


  /*
    If ESP32 has no saved Wi-Fi,
    WiFiManager creates:

    ExaShield-Setup-1
  */

  bool result =
    wm.autoConnect(
      apName.c_str()
    );


  if (!result) {

    Serial.println(
      "Wi-Fi configuration failed."
    );

    return false;
  }


  Serial.println();
  Serial.println(
    "Wi-Fi connected!"
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
    "RSSI: "
  );

  Serial.print(
    WiFi.RSSI()
  );

  Serial.println(
    " dBm"
  );


  return true;
}


// ============================================================
// FIREBASE URL BUILDER
// ============================================================

String firebaseURL(
  const String& path
) {

  return String(
    FIREBASE_URL
  )
  + path
  + ".json";
}


// ============================================================
// FIREBASE PUT
// ============================================================

bool firebasePUT(
  const String& path,
  const String& json
) {

  if (
    WiFi.status() != WL_CONNECTED
  ) {

    Serial.println(
      "Firebase PUT: Wi-Fi offline."
    );

    return false;
  }


  WiFiClientSecure client;

  /*
    Test/demo mode.
    For production, certificate validation should be used.
  */

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

    Serial.println(
      "Firebase HTTP begin failed."
    );

    return false;
  }


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


// ============================================================
// FIREBASE POST
// ============================================================

bool firebasePOST(
  const String& path,
  const String& json
) {

  if (
    WiFi.status() != WL_CONNECTED
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


// ============================================================
// GET ACTIVITY
// ============================================================

uint16_t getActivity() {

  uint32_t packets;


  noInterrupts();

  packets =
    wirelessPackets;

  wirelessPackets =
    0;

  interrupts();


  /*
    Convert packet count to 0-1000 activity score.
  */

  uint16_t activity =
    packets;


  if (
    activity > 1000
  ) {

    activity = 1000;
  }


  return activity;
}


// ============================================================
// UPLOAD SENSOR STATUS
// ============================================================

void uploadSensorStatus(
  uint16_t activity,
  uint32_t packets
) {

  String node =
    "node" +
    String(NODE_ID);


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
      String(packets) +
      ","
      "\"uptime\":" +
      String(
        millis() / 1000
      ) +
      ","
      "\"rssi\":" +
      String(
        WiFi.RSSI()
      ) +
      ","
      "\"lastSeen\":{\".sv\":\"timestamp\"}"
    "}";


  firebasePUT(
    "/sensors/" + node,
    json
  );
}


// ============================================================
// UPLOAD EVENT
// ============================================================

void uploadEvent(
  uint16_t activity,
  uint32_t packets
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
      String(packets) +
      ","
      "\"timestamp\":{\".sv\":\"timestamp\"}"
    "}";


  firebasePOST(
    "/events",
    json
  );
}


// ============================================================
// UPLOAD ALERT
// ============================================================

void uploadAlert(
  uint16_t activity,
  uint32_t packets
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
      String(packets) +
      ","
      "\"level\":\"HIGH\","
      "\"message\":\"High wireless activity detected - verification required\","
      "\"timestamp\":{\".sv\":\"timestamp\"}"
    "}";


  firebasePOST(
    "/alerts",
    json
  );
}


// ============================================================
// HEARTBEAT
// ============================================================

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
      "\"rssi\":" +
      String(
        WiFi.RSSI()
      ) +
      ","
      "\"lastSeen\":{\".sv\":\"timestamp\"}"
    "}";


  firebasePUT(
    "/nodes/" + node,
    json
  );
}


// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(
    115200
  );


  delay(1000);


  // Built-in LED
  pinMode(
    LED_BUILTIN,
    OUTPUT
  );


  ledOff();


  Serial.println();
  Serial.println(
    "=========================================="
  );

  Serial.println(
    "             ExaShield"
  );

  Serial.println(
    " ESP32 Wireless Activity Monitor"
  );

  Serial.println(
    "=========================================="
  );


  Serial.print(
    "Node ID: "
  );

  Serial.println(
    NODE_ID
  );


  // ----------------------------------------------------------
  // Wi-Fi
  // ----------------------------------------------------------

  if (
    !connectWiFi()
  ) {

    Serial.println(
      "Restarting ESP32..."
    );

    delay(3000);

    ESP.restart();
  }


  // Wi-Fi connected
  ledOn();


  // ----------------------------------------------------------
  // Firebase
  // ----------------------------------------------------------

  Serial.println(
    "Sending Firebase heartbeat..."
  );


  uploadHeartbeat();


  // ----------------------------------------------------------
  // Wireless monitoring
  // ----------------------------------------------------------

  startWirelessMonitor();


  lastUpload =
    millis();


  lastHeartbeat =
    millis();


  Serial.println();
  Serial.println(
    "ExaShield READY."
  );

  Serial.println(
    "System is monitoring wireless activity."
  );
}


// ============================================================
// LOOP
// ============================================================

void loop() {


  // ----------------------------------------------------------
  // Wi-Fi reconnect
  // ----------------------------------------------------------

  if (
    WiFi.status() != WL_CONNECTED
  ) {

    Serial.println(
      "Wi-Fi disconnected."
    );


    ledOff();


    stopWirelessMonitor();


    delay(1000);


    if (
      connectWiFi()
    ) {

      ledOn();

      startWirelessMonitor();

    } else {

      ledOff();
    }
  }


  // ----------------------------------------------------------
  // Activity upload
  // ----------------------------------------------------------

  if (
    millis() - lastUpload
    >= UPLOAD_INTERVAL
  ) {

    lastUpload =
      millis();


    uint16_t activity =
      getActivity();


    uint32_t packets =
      activity;


    Serial.println();
    Serial.println(
      "------------------------------------------"
    );


    Serial.print(
      "Activity: "
    );

    Serial.println(
      activity
    );


    Serial.print(
      "Packets: "
    );

    Serial.println(
      packets
    );


    Serial.print(
      "Wi-Fi RSSI: "
    );

    Serial.print(
      WiFi.RSSI()
    );

    Serial.println(
      " dBm"
    );


    // Firebase
    uploadSensorStatus(
      activity,
      packets
    );


    uploadEvent(
      activity,
      packets
    );


    // --------------------------------------------------------
    // High activity
    // --------------------------------------------------------

    if (
      activity >=
      ALERT_THRESHOLD
    ) {

      Serial.println(
        "HIGH ACTIVITY DETECTED!"
      );


      uploadAlert(
        activity,
        packets
      );


      /*
        Built-in LED warning blink.
      */

      ledBlink(
        3,
        120
      );


      // Return to normal online state
      ledOn();
    }
  }


  // ----------------------------------------------------------
  // Heartbeat
  // ----------------------------------------------------------

  if (
    millis() - lastHeartbeat
    >= HEARTBEAT_INTERVAL
  ) {

    lastHeartbeat =
      millis();


    uploadHeartbeat();
  }


  delay(20);
}
