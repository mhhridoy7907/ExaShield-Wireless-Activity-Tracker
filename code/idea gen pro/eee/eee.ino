


#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>

#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "esp_wifi.h"


// ========================================================
// CONFIGURATION
// ========================================================

#define NODE_ID 1

#define LED_PIN 2

// Optional buzzer.
// Set to -1 if no buzzer is connected.
#define BUZZER_PIN 25


// Firebase database URL
const char* FIREBASE_URL =
"https://exashield-dabd4-default-rtdb.asia-southeast1.firebasedatabase.app";


// Activity threshold
const uint16_t ALERT_THRESHOLD = 500;


// Upload interval
const unsigned long UPLOAD_INTERVAL = 5000;


// Sensor offline timeout
const unsigned long HEARTBEAT_INTERVAL = 30000;


// ========================================================
// GLOBAL VARIABLES
// ========================================================

volatile uint32_t wirelessPackets = 0;

unsigned long lastUpload = 0;

unsigned long lastHeartbeat = 0;


// ========================================================
// WIFI PACKET CALLBACK
// ========================================================

void IRAM_ATTR wifiSnifferCallback(0
    void* buffer,
    wifi_promiscuous_pkt_type_t type
) {

    if (buffer == nullptr) {
        return;
    }

    /*
      Count Wi-Fi management/data activity.

      This is only an activity counter.
      We are not storing packet contents.
    */

    if (
        type == WIFI_PKT_MGMT ||
        type == WIFI_PKT_DATA
    ) {

        wirelessPackets++;
    }
}


// ========================================================
// START WIRELESS MONITOR
// ========================================================

void startWirelessMonitor() {

    WiFi.mode(WIFI_STA);

    delay(200);

    esp_wifi_set_promiscuous(false);

    esp_wifi_set_promiscuous_rx_cb(
        wifiSnifferCallback
    );

    esp_wifi_set_promiscuous(true);

    Serial.println(
        "Wireless activity monitor started."
    );
}


// ========================================================
// STOP WIRELESS MONITOR
// ========================================================

void stopWirelessMonitor() {

    esp_wifi_set_promiscuous(false);

    Serial.println(
        "Wireless activity monitor stopped."
    );
}


// ========================================================
// CONNECT WIFI USING PHONE
// ========================================================

bool connectWiFi() {

    WiFiManager wm;

    String apName =
        "ExaShield-Setup-" +
        String(NODE_ID);

    Serial.println();
    Serial.println(
        "Starting WiFi configuration..."
    );

    Serial.print(
        "Configuration AP: "
    );

    Serial.println(
        apName
    );


    bool result =
        wm.autoConnect(
            apName.c_str()
        );


    if (!result) {

        Serial.println(
            "WiFi configuration failed."
        );

        return false;
    }


    Serial.println();

    Serial.println(
        "WiFi connected!"
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

    Serial.println(
        WiFi.RSSI()
    );


    return true;
}


// ========================================================
// FIREBASE URL
// ========================================================

String firebaseURL(
    const String& path
) {

    return String(
        FIREBASE_URL
    )
    + path
    + ".json";
}


// ========================================================
// FIREBASE PUT
// ========================================================

bool firebasePUT(
    const String& path,
    const String& json
) {

    if (
        WiFi.status() != WL_CONNECTED
    ) {

        Serial.println(
            "WiFi not connected."
        );

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

        Serial.println(
            "HTTP begin failed."
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
        "Firebase PUT response: "
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


// ========================================================
// FIREBASE POST
// ========================================================

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
        "Firebase POST response: "
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


// ========================================================
// GET ACTIVITY
// ========================================================

uint16_t getActivity() {

    uint32_t packets;


    noInterrupts();

    packets =
        wirelessPackets;

    wirelessPackets =
        0;

    interrupts();


    /*
      Convert packet count to a simple 0-1000 score.
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


// ========================================================
// BUZZER
// ========================================================

void localAlert() {

    digitalWrite(
        LED_PIN,
        HIGH
    );


    if (
        BUZZER_PIN >= 0
    ) {

        tone(
            BUZZER_PIN,
            1800,
            300
        );
    }


    delay(350);


    digitalWrite(
        LED_PIN,
        LOW
    );
}


// ========================================================
// UPLOAD SENSOR STATUS
// ========================================================

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


// ========================================================
// UPLOAD EVENT
// ========================================================

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


// ========================================================
// UPLOAD ALERT
// ========================================================

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


// ========================================================
// UPLOAD GATEWAY/NODE HEARTBEAT
// ========================================================

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
        String(WiFi.RSSI()) +
        ","
        "\"lastSeen\":{\".sv\":\"timestamp\"}"
        "}";


    firebasePUT(
        "/nodes/" + node,
        json
    );
}


// ========================================================
// SETUP
// ========================================================

void setup() {

    Serial.begin(
        115200
    );


    delay(1000);


    pinMode(
        LED_PIN,
        OUTPUT
    );


    digitalWrite(
        LED_PIN,
        LOW
    );


    if (
        BUZZER_PIN >= 0
    ) {

        pinMode(
            BUZZER_PIN,
            OUTPUT
        );
    }


    Serial.println();
    Serial.println(
        "======================================"
    );

    Serial.println(
        "       ExaShield ESP32"
    );

    Serial.println(
        " Wireless Activity Monitor"
    );

    Serial.println(
        "======================================"
    );


    Serial.print(
        "Node ID: "
    );

    Serial.println(
        NODE_ID
    );


    // ----------------------------------------------------
    // WiFi
    // ----------------------------------------------------

    if (
        !connectWiFi()
    ) {

        Serial.println(
            "Restarting..."
        );

        delay(3000);

        ESP.restart();
    }


    // ----------------------------------------------------
    // Firebase heartbeat
    // ----------------------------------------------------

    uploadHeartbeat();


    // ----------------------------------------------------
    // Start wireless monitoring
    // ----------------------------------------------------

    startWirelessMonitor();


    lastUpload =
        millis();

    lastHeartbeat =
        millis();


    Serial.println();
    Serial.println(
        "ExaShield READY."
    );
}


// ========================================================
// LOOP
// ========================================================

void loop() {

    // ----------------------------------------------------
    // Reconnect WiFi if necessary
    // ----------------------------------------------------

    if (
        WiFi.status() != WL_CONNECTED
    ) {

        Serial.println(
            "WiFi disconnected."
        );

        stopWirelessMonitor();

        delay(1000);

        connectWiFi();

        startWirelessMonitor();
    }


    // ----------------------------------------------------
    // Upload activity
    // ----------------------------------------------------

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
            "--------------------------------"
        );


        Serial.print(
            "Activity: "
        );

        Serial.println(
            activity
        );


        Serial.print(
            "WiFi RSSI: "
        );

        Serial.println(
            WiFi.RSSI()
        );


        uploadSensorStatus(
            activity,
            packets
        );


        uploadEvent(
            activity,
            packets
        );


        if (
            activity >=
            ALERT_THRESHOLD
        ) {

            Serial.println(
                "HIGH ACTIVITY!"
            );


            uploadAlert(
                activity,
                packets
            );


            localAlert();
        }
    }


    // ----------------------------------------------------
    // Heartbeat
    // ----------------------------------------------------

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
