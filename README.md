# 🛡️ ExaShield

### ESP32-Based Examination Hall Wireless Activity Detection and Alert System

> **“Detect Wireless Activity Before It Becomes a Problem.”**

ExaShield is an IoT-based examination monitoring system designed to observe wireless activity within examination environments. The system uses ESP32-based sensor nodes to monitor the surrounding wireless environment, process activity information locally, and send the collected data to Firebase Realtime Database.

An administrator can monitor connected sensor nodes, wireless activity levels, events, and alerts through a real-time web dashboard.

> **Important:** ExaShield is an assistance and monitoring system. It reports potentially abnormal wireless activity; it does **not** identify individual students or provide definitive proof of cheating or examination misconduct.

---

## 🏫 Project Information

**University:** RTM Al-Kabir Technical University
**Location:** Sylhet, Bangladesh

**Project:** ExaShield
**Original Project Name:** ExamGuard
**Team:** H&S Tech

### Team Members

* **MH Hridoy**
* **Umma Habiba Sumi**

---

## 💡 Idea

### Idea Title

**ExamGuard — ESP32-Based Examination Hall Wireless Activity Detection and Alert System**

### Idea Category

* IoT
* Embedded Systems
* Wireless Monitoring
* Cloud Computing
* Web Application

---

## 🎯 Target Users

ExaShield is intended for:

* Schools
* Colleges
* Universities
* Examination controllers
* Invigilators
* Teachers
* Institution administrators

The system is particularly useful for environments where a large number of students need to be monitored simultaneously.

---

## ❗ Problem Statement

Monitoring unauthorized electronic or wireless activity during examinations is largely dependent on direct human observation.

In large examination halls, it can be difficult for invigilators to continuously monitor every student and identify unusual wireless activity.

This creates a need for a low-cost, automated, and real-time monitoring system that can provide timely information to examination authorities.

---

## ✅ Proposed Solution

ExaShield uses ESP32 sensor nodes to observe the surrounding wireless environment and calculate an activity level based on detected wireless packets.

The processed activity information is transmitted through Wi-Fi to **Firebase Realtime Database**.

An administrator can then monitor the examination environment through a web-based dashboard.

When activity reaches a predefined threshold, the system generates a warning through:

* Dashboard visual alerts
* Popup notifications
* Audio alerts
* ESP32 LED indication

The system is designed as a **decision-support and monitoring tool**, not as a system for automatically accusing or identifying students.

---

## 🔄 System Architecture

```text
        Wireless Environment
                │
                ▼
       ┌──────────────────┐
       │  ESP32 Sensor    │
       │      Node        │
       └────────┬─────────┘
                │
                │ Wi-Fi
                ▼
       ┌──────────────────┐
       │ Firebase         │
       │ Realtime         │
       │ Database         │
       └────────┬─────────┘
                │
                │ Real-time Sync
                ▼
       ┌──────────────────┐
       │ Admin Web        │
       │ Dashboard        │
       └────────┬─────────┘
                │
        ┌───────┴────────┐
        ▼                ▼
   Visual Alert      Audio Alert
```

### Hardware Flow

```text
Wireless Environment
        ↓
nRF24L01 / Wireless Monitoring
        ↓
ESP32 Sensor Node
        ↓
Wi-Fi Internet
        ↓
Firebase
        ↓
Admin Dashboard
        ↓
Visual + Audio Alert
```

---

## ⚙️ Main Modules

### 1. ESP32 Monitoring Module

The ESP32 sensor node observes the wireless environment and processes wireless packet activity locally.

The node records information such as:

* Activity count
* Packet count
* Wi-Fi channel
* Wi-Fi RSSI
* Node ID
* Uptime
* Online status

---

### 2. Internet Connectivity Module

The ESP32 connects to a configured Wi-Fi network using **WiFiManager**.

The configuration portal allows the device to be configured through a smartphone.

Default setup network:

```text
ExaShield-Setup-1
```

Configuration portal:

```text
192.168.4.1
```

---

### 3. Firebase Cloud Module

Firebase Realtime Database is used as the cloud backend.

The system stores and synchronizes:

```text
/sensors
/nodes
/events
/alerts
```

This allows the administrator dashboard to receive real-time updates.

---

### 4. Admin Web Dashboard

The web dashboard provides:

* Total sensor nodes
* Online sensor count
* Alert count
* System status
* Wireless activity levels
* Sensor details
* Event history
* Real-time alerts
* Audio notifications

---

### 5. Alert Module

When wireless activity reaches the configured threshold, the system generates a high-activity alert.

Default threshold:

```text
500 packets / activity interval
```

Alert information includes:

* Node ID
* Activity
* Packet count
* Wireless channel
* Wi-Fi RSSI
* Verification status
* Timestamp

The dashboard also provides an audio alarm and popup notification.

---

### 6. Multiple Sensor Module

The architecture supports multiple ESP32 sensor nodes.

Each node can have a unique ID, for example:

```text
NODE-1
NODE-2
NODE-3
NODE-4
```

All nodes can report to the same Firebase backend and can be monitored from a centralized dashboard.

---

## 🚀 Key Features

* 📡 Wireless activity monitoring
* 🧠 Local activity processing
* 🌐 Wi-Fi connectivity
* ☁️ Firebase Realtime Database integration
* 📊 Real-time web dashboard
* 🔴 High-activity detection
* 🔔 Visual and audio alerts
* 💡 ESP32 LED indication
* 🛰️ Multiple sensor node support
* 📜 Event history
* ❤️ Sensor heartbeat monitoring
* 📱 Smartphone-based Wi-Fi configuration
* 📈 Activity level classification
* 🔄 Real-time Firebase synchronization
* 🏫 Suitable for examination-hall environments
* 🔐 Designed for anonymous activity-based monitoring

---

## 📊 Activity Classification

The current prototype uses predefined activity levels:

|    Activity | Level  |
| ----------: | ------ |
|   `0 – 199` | LOW    |
| `200 – 499` | MEDIUM |
|      `500+` | HIGH   |

When the activity level reaches **HIGH**, an alert is generated.

These thresholds are configurable and should be calibrated through controlled testing before real-world deployment.

---

## 🔔 Alert Workflow

```text
Wireless Activity Detected
          ↓
ESP32 Counts Activity
          ↓
Activity Threshold Check
          ↓
       HIGH?
       /   \
     NO     YES
     │       │
     │       ▼
     │   Firebase Alert
     │       │
     │       ├── Dashboard Popup
     │       ├── Audio Alarm
     │       └── ESP32 LED
     │
     ▼
Continue Monitoring
```

---

## 🗄️ Firebase Data Structure

The prototype uses the following main database paths:

```text
exashield-dabd4
│
├── sensors
│   └── node1
│       ├── nodeId
│       ├── status
│       ├── activity
│       ├── packets
│       ├── level
│       ├── channel
│       ├── wifiRssi
│       ├── uptime
│       └── lastSeen
│
├── nodes
│   └── node1
│       ├── nodeId
│       ├── status
│       ├── ip
│       ├── channel
│       ├── rssi
│       └── lastSeen
│
├── events
│   └── generated-event-id
│       ├── nodeId
│       ├── activity
│       ├── packets
│       ├── level
│       ├── channel
│       ├── wifiRssi
│       └── timestamp
│
└── alerts
    └── generated-alert-id
        ├── nodeId
        ├── activity
        ├── packets
        ├── level
        ├── verification
        ├── message
        ├── channel
        ├── wifiRssi
        └── timestamp
```

---

## 🛠️ Technology Stack

### Hardware

* ESP32
* nRF24L01 antenna/module
* LED
* Optional buzzer
* Power supply

### Embedded Software

* C++
* Arduino Framework
* Arduino IDE
* ESP32 Wi-Fi
* WiFiManager
* ESP32 Wi-Fi promiscuous monitoring

### Cloud

* Firebase Realtime Database

### Web

* HTML5
* CSS3
* JavaScript
* Firebase Web SDK

---

## 📁 Project Structure

A recommended repository structure is:

```text
ExaShield-Wireless-Activity-Tracker/
code/
│
├── firmware/
│   └── exashield_node/
│       └── exashield_node.ino
│
├── dashboard/
│   └── index.html
│
├── docs/
│   ├── architecture.md
│   └── project-overview.md
│
├── assets/
│   └── screenshots/
│
├── .gitignore
└── README.md
```

---

## 🔧 Configuration

Before uploading the firmware, configure the Firebase endpoint:

```cpp
const char* FIREBASE_URL =
  "https://YOUR-PROJECT-default-rtdb.REGION.firebasedatabase.app";
```

Each ESP32 node should have a unique node ID:

```cpp
#define NODE_ID 1
```

For another sensor:

```cpp
#define NODE_ID 2
```

and so on.

The activity threshold can also be adjusted:

```cpp
const uint16_t ALERT_THRESHOLD = 500;
```

---

## 📡 Wi-Fi Setup

On startup, the ESP32 creates a configuration access point:

```text
ExaShield-Setup-1
```

Connect a phone or computer to this network and open:

```text
192.168.4.1
```

Select the examination network's Wi-Fi credentials and allow the ESP32 to connect.

---

## 🖥️ Dashboard

The administrator dashboard provides four main areas:

### Dashboard

Displays overall system statistics.

### Sensors

Displays connected ESP32 nodes and their current activity.

### Alerts

Displays high wireless activity events requiring human verification.

### Events

Displays historical activity records received from sensor nodes.

---

## 🔒 Privacy & Responsible Use

ExaShield is intentionally designed around **activity-based monitoring**.

The system does not attempt to:

* Identify individual students
* Capture student identities
* Determine who is responsible for an activity
* Automatically declare cheating
* Replace human invigilators

A detected wireless activity event should be treated as an **indicator that requires human verification**.

False positives are possible because wireless traffic can originate from legitimate devices and surrounding infrastructure.

---

## ⚠️ Limitations

The current prototype has several limitations:

* Wireless packet activity does not directly prove cheating.
* Activity levels depend on environmental conditions.
* Wi-Fi traffic can vary significantly between locations.
* Thresholds require real-world calibration.
* ESP32 monitoring capabilities depend on supported hardware and firmware.
* Internet connectivity is required for Firebase synchronization.
* The prototype should be tested in controlled environments before deployment.
* Security rules and authentication must be properly configured for production use.

---

## 🔮 Future Improvements

Possible future development includes:

* 🔐 Firebase Authentication
* 👥 Role-based administrator access
* 🤖 AI/ML-based anomaly detection
* 📊 Historical analytics
* 📈 Activity graphs
* 🗺️ Examination-hall sensor mapping
* 🔔 Hardware buzzer integration
* 📱 Mobile administrator application
* 🔒 Improved Firebase Security Rules
* ⚡ Offline data buffering
* 🌐 Multi-hall centralized monitoring
* 🧠 Adaptive activity thresholds
* 📡 Improved wireless sensing hardware
* 📝 Automated examination session management

---

## 🎓 Feasibility

The project is technically and economically feasible as a prototype.

ESP32 hardware, Firebase, and modern web technologies are widely available and relatively affordable. The architecture also allows additional sensor nodes to be added without redesigning the entire system.

A functional prototype can be developed and tested in a controlled examination-like environment.

---

## 🌟 Innovation

The main innovative aspects of ExaShield include:

1. Low-cost ESP32-based wireless monitoring
2. Real-time Firebase cloud synchronization
3. Web-based centralized monitoring
4. Automated activity-based alerts
5. Multiple sensor-node architecture
6. Anonymous wireless activity monitoring
7. Scalable examination-hall architecture
8. Future integration potential for AI/ML anomaly detection

---

## 🧪 Testing Approach

The prototype should be evaluated in a controlled environment.

Testing should include:

* Normal examination environment
* Different numbers of wireless devices
* Different sensor locations
* Different Wi-Fi conditions
* Different activity thresholds
* Multiple ESP32 nodes
* Firebase connectivity loss
* Wi-Fi disconnection and reconnection
* Alert generation
* Dashboard synchronization

Performance should be evaluated using measurable indicators such as detection consistency, false-positive rate, latency, and sensor reliability.

---

## 👨‍💻 Team

### H&S Tech

**MH Hridoy**
**Umma Habiba Sumi**

RTM Al-Kabir Technical University
Sylhet, Bangladesh

---

## 📌 Project Status

**Current Status:** Prototype / Academic Project

The system is currently intended for academic research, demonstration, and controlled testing.

---

## 📜 Disclaimer

ExaShield is an experimental academic IoT monitoring project.

Wireless activity detection should not be interpreted as proof of examination misconduct. Any alert generated by the system should be reviewed by authorized examination personnel before taking any action.

The project should be deployed responsibly and in accordance with applicable institutional policies, privacy requirements, and local regulations.

---

## ⭐ Project Vision

> **Detect Wireless Activity Before It Becomes a Problem.**

ExaShield aims to provide examination authorities with an affordable, scalable, and real-time wireless activity monitoring platform that supports human invigilators without replacing human judgment.
