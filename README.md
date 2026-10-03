# 🛡️ ExaShield

## ESP32-Based Examination Hall Wireless Activity Detection and Alert System

> **“Detect Wireless Activity Before It Becomes a Problem.”**

ExaShield is an IoT-based examination monitoring and wireless activity awareness system designed to assist examination authorities in observing unusual wireless activity within examination environments.

The current prototype uses ESP32-based sensor nodes to monitor the surrounding Wi-Fi environment, process wireless activity information locally, and synchronize the collected data with Firebase Realtime Database.

The system provides a centralized web dashboard where administrators can monitor sensor nodes, wireless activity levels, events, system status, and alerts in real time.

As a future development, ExaShield can be extended with dedicated RF power detection hardware such as the **AD8318** to monitor RF energy in supported frequency ranges. This would provide an additional sensing layer for environments where cellular/mobile-network RF activity may need to be studied alongside Wi-Fi activity.

> **Important:** ExaShield is an assistance and monitoring system. Wireless activity measurements are indicators only; they do not identify individual students, determine who generated a signal, reveal communication content, or provide definitive proof of examination misconduct.

---

# 🏫 Project Information

| Information | Details |
|---|---|
| **University** | RTM Al-Kabir Technical University |
| **Location** | Sylhet, Bangladesh |
| **Project** | ExaShield |
| **Original Project Name** | ExamGuard |
| **Team** | H&S Tech |
| **Project Type** | Academic IoT / Embedded Systems Project |
| **Current Status** | Prototype / Academic Project |

---

# 👥 Team Members

- **MH Hridoy**
- **Umma Habiba Sumi**

---

# 💡 Project Idea

## Idea Title

**ExaShield — ESP32-Based Examination Hall Wireless Activity Detection and Alert System**

## Idea Category

- IoT
- Embedded Systems
- Wireless Monitoring
- RF Sensing
- Cloud Computing
- Web Application
- Real-Time Monitoring

---

# 🎯 Target Users

ExaShield is intended to assist:

- Schools
- Colleges
- Universities
- Examination controllers
- Invigilators
- Teachers
- Institution administrators

The architecture is particularly suitable for environments where a large number of students need to be monitored simultaneously.

---

# ❗ Problem Statement

Monitoring unauthorized electronic or wireless activity during examinations is largely dependent on direct human observation.

In large examination halls, it can be difficult for invigilators to continuously observe every student and recognize unusual wireless activity.

This creates a need for a low-cost, automated, and real-time monitoring system that can provide timely information to authorized examination personnel.

However, wireless activity alone cannot establish who generated a signal or whether the activity represents misconduct.

Therefore, ExaShield is designed as a **decision-support system** that provides wireless activity information to human invigilators rather than automatically identifying or accusing students.

---

# ✅ Proposed Solution

ExaShield uses distributed sensor nodes to observe the wireless environment of an examination area.

In the current prototype, ESP32 is used to monitor Wi-Fi-related wireless activity and process activity measurements locally.

The processed information is transmitted through Wi-Fi to Firebase Realtime Database, where it can be accessed by an administrator dashboard.

When activity reaches a configurable threshold, the system can generate an alert through:

- Dashboard visual alert
- Popup notification
- Audio notification
- ESP32 LED indication

The future architecture can additionally integrate an **AD8318 RF power detector** with an appropriate RF front-end to measure RF signal strength in supported frequency ranges.

This future RF sensing layer would complement the existing Wi-Fi monitoring system rather than replace it.

---

# 🔄 System Architecture

## Current Architecture

```text
             Wireless Environment
                     │
                     ▼
            ┌─────────────────┐
            │  ESP32 Sensor   │
            │      Node       │
            └────────┬────────┘
                     │
                     │ Wi-Fi
                     ▼
            ┌─────────────────┐
            │    Firebase     │
            │ Realtime DB     │
            └────────┬────────┘
                     │
                     │ Real-Time Sync
                     ▼
            ┌─────────────────┐
            │ Admin Dashboard │
            └────────┬────────┘
                     │
              ┌──────┴──────┐
              ▼             ▼
        Visual Alert    Audio Alert
```

---

# 🔮 Future Multi-Layer RF Monitoring Architecture

```text
                  Wireless Environment
                          │
             ┌────────────┴────────────┐
             │                         │
             ▼                         ▼
       Wi-Fi Activity             RF Energy
             │                         │
             ▼                         ▼
          ESP32                 RF Front-End
             │                         │
             │                         ▼
             │                     AD8318
             │                  RF Power Detector
             │                         │
             └────────────┬────────────┘
                          │
                          ▼
                   Sensor Processing
                          │
                          ▼
                    Firebase RTDB
                          │
                          ▼
                   Admin Dashboard
                          │
             ┌────────────┼────────────┐
             ▼            ▼            ▼
          Events        Alerts      Analytics
```

---

# ⚙️ Main Modules

## 1. ESP32 Monitoring Module

The ESP32 sensor node observes the surrounding Wi-Fi environment and processes wireless activity locally.

The node can record information such as:

- Activity count
- Packet/activity count
- Wi-Fi channel
- Wi-Fi RSSI
- Node ID
- Uptime
- Online/offline status
- Last-seen timestamp

Each ESP32 can operate as an independent sensor node.

---

## 2. Internet Connectivity Module

The ESP32 connects to a configured Wi-Fi network using **WiFiManager**.

This allows the device to be configured through a smartphone or computer without hard-coding Wi-Fi credentials into the firmware.

### Default Configuration

```text
Configuration Network:
ExaShield-Setup-1

Configuration Portal:
192.168.4.1
```

---

## 3. Firebase Cloud Module

Firebase Realtime Database is used as the cloud backend.

The system stores and synchronizes information through logical paths such as:

```text
/sensors
/nodes
/events
/alerts
```

This allows the administrator dashboard to receive updated sensor information in real time.

---

# 🖥️ 4. Admin Web Dashboard

The web dashboard provides centralized monitoring of the ExaShield system.

### Dashboard Functions

- Total sensor nodes
- Online sensor count
- Offline sensor count
- Current activity level
- Alert count
- System status
- Sensor details
- Event history
- Real-time alerts
- Audio notifications
- RF activity information in future versions

---

# 🔔 5. Alert Module

When measured activity reaches a predefined threshold, ExaShield can generate a high-activity warning.

### Current Prototype Threshold

```text
500 activity units / monitoring interval
```

The alert may contain:

- Node ID
- Activity level
- Packet/activity count
- Wireless channel
- Wi-Fi RSSI
- Verification status
- Timestamp

The dashboard can provide both visual and audio notifications.

> **Note:** A threshold-based alert is only an indicator. It should not be interpreted as proof of cheating or unauthorized device use.

---

# 📡 6. Multiple Sensor Node Module

The architecture supports multiple ESP32 sensor nodes.

For example:

```text
NODE-1
NODE-2
NODE-3
NODE-4
```

Each node can report independently to the same Firebase backend.

### Example Examination Hall

```text
                 Examination Hall
        ┌─────────────────────────────┐
        │                             │
        │   NODE-1        NODE-2      │
        │      ●             ●        │
        │                             │
        │                             │
        │   NODE-3        NODE-4      │
        │      ●             ●        │
        │                             │
        └─────────────────────────────┘
                       │
                       ▼
                  Firebase
                       │
                       ▼
                Admin Dashboard
```

---

# 📡 7. Future RF Monitoring Module — AD8318

One of the planned future improvements is the integration of the **AD8318 RF power detector**.

The AD8318 is an RF detector IC designed to measure RF signal power over a wide frequency range.

In the ExaShield architecture, it can be used as an additional RF sensing layer.

## Proposed Future Concept

```text
RF Environment
      │
      ▼
Antenna / RF Front-End
      │
      ▼
AD8318 RF Power Detector
      │
      ▼
Analog Output
      │
      ▼
ESP32 ADC
      │
      ▼
Signal Processing
      │
      ▼
Firebase
      │
      ▼
Admin Dashboard
```

The ESP32 would read the AD8318's analog output and convert the measurement into an RF activity indicator.

---

# 📊 Possible Measurements

The future system could record information such as:

- RF signal level
- Relative RF activity
- Activity trend
- Time of RF activity
- Sensor/node location
- Measurement history
- RF activity threshold events

---

# 📱 Future Mobile-Data / Cellular RF Monitoring

A future version of ExaShield may investigate the use of AD8318-based RF sensing for observing cellular-band RF energy, subject to:

- Frequency range
- Antenna characteristics
- RF front-end design
- Filtering
- Calibration
- Local regulatory requirements

The purpose would be to determine whether the RF environment shows increased activity in supported cellular frequency bands.

## Important Technical Limitation

The AD8318 does **not** directly identify:

- A particular mobile phone
- A particular student
- A phone number
- Mobile data content
- Websites visited
- Messages
- Calls
- SIM information
- The exact source device

Instead, it provides a measurement related to RF power/signal strength at its input.

Therefore, a future ExaShield RF module should be described as:

> **RF activity / RF power monitoring**

rather than claiming that it can directly detect exactly which phone is using mobile data.

---

# 🧠 Future Multi-Sensor Detection

The long-term architecture can combine multiple sensing methods.

```text
                 ExaShield Sensor Node
                         │
          ┌──────────────┴──────────────┐
          │                             │
          ▼                             ▼
     Wi-Fi Monitor                 RF Detector
        ESP32                       AD8318
          │                             │
          └──────────────┬──────────────┘
                         ▼
                  Sensor Processing
                         │
                         ▼
                  Activity Analysis
                         │
                         ▼
                    Firebase
                         │
                         ▼
                 Admin Dashboard
```

This approach can provide a broader picture of the wireless environment than relying on a single sensing method.

---

# 🚀 Key Features

## Current Prototype

- 📡 Wi-Fi wireless activity monitoring
- 🧠 Local activity processing
- 🌐 Wi-Fi connectivity
- ☁️ Firebase Realtime Database
- 📊 Real-time web dashboard
- 🔴 High-activity detection
- 🔔 Visual alerts
- 🔊 Audio alerts
- 💡 ESP32 LED indication
- 🛰️ Multiple sensor-node support
- 📜 Event history
- ❤️ Sensor heartbeat monitoring
- 📱 Smartphone-based Wi-Fi configuration
- 📈 Activity-level classification
- 🔄 Real-time Firebase synchronization
- 🏫 Examination-hall monitoring architecture
- 🔐 Activity-based and anonymous monitoring

## Future Features

- 📡 AD8318 RF power detection
- 📶 Cellular-band RF activity research
- 📊 RF signal-level analytics
- 🧠 Multi-sensor activity correlation
- 🤖 AI/ML anomaly detection
- 📈 Historical RF analytics
- 🗺️ Examination-hall sensor mapping
- 🔔 Hardware buzzer
- 📱 Mobile administrator application
- 🔒 Advanced Firebase Security Rules
- ⚡ Offline data buffering
- 🌐 Multi-hall centralized monitoring
- 🧠 Adaptive thresholds
- 📡 Improved RF sensing hardware

---

# 📊 Activity Classification

The current prototype uses predefined activity levels:

| Activity | Level |
|---:|:---|
| 0 – 199 | 🟢 LOW |
| 200 – 499 | 🟡 MEDIUM |
| 500+ | 🔴 HIGH |

When activity reaches **HIGH**, an alert can be generated.

> These thresholds are configurable and should be calibrated through controlled experiments before any real-world deployment.

---

# 🔔 Alert Workflow

```text
Wireless Activity Detected
            │
            ▼
      ESP32 Monitoring
            │
            ▼
     Activity Processing
            │
            ▼
     Threshold Evaluation
            │
        ┌───┴───┐
        │       │
       LOW     HIGH
        │       │
        │       ▼
        │   Firebase Alert
        │       │
        │   ┌───┼───────────┐
        │   ▼   ▼           ▼
        │ Popup Audio     ESP32 LED
        │
        ▼
 Continue Monitoring
```

---

# 🗄️ Firebase Data Structure

The prototype uses the following logical database structure:

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

# 🛠️ Technology Stack

## Hardware

### Current Prototype

- ESP32
- LED
- Power supply
- Optional buzzer

### Wireless / RF Expansion

- AD8318 RF power detector
- Appropriate RF antenna
- RF filtering / front-end
- RF connectors / cabling
- Additional RF sensing hardware where required

> **Note:** The final RF front-end must be selected according to the frequency band being studied. AD8318 integration alone does not automatically provide cellular-band identification.

---

# 💻 Embedded Software

- C++
- Arduino Framework
- Arduino IDE
- ESP32 Wi-Fi
- WiFiManager
- ESP32 Wi-Fi monitoring capabilities
- ESP32 ADC for future AD8318 integration
- Signal-processing algorithms

---

# ☁️ Cloud Technology

- Firebase Realtime Database
- Firebase Web SDK
- Firebase Authentication — planned
- Firebase Security Rules

---

# 🌐 Web Technology

- HTML5
- CSS3
- JavaScript
- Firebase Web SDK
- Real-time database listeners

---

# 📁 Project Structure

Recommended repository structure:

```text
ExaShield-Wireless-Activity-Tracker/
│
├── code/
│   └── firmware/
│       └── exashield_node/
│           └── exashield_node.ino
│
├── dashboard/
│   └── index.html
│
├── docs/
│   ├── architecture.md
│   ├── project-overview.md
│   ├── rf-monitoring.md
│   └── testing.md
│
├── assets/
│   └── screenshots/
│
├── hardware/
│   ├── schematics/
│   └── rf-module/
│
├── .gitignore
└── README.md
```

---

# 🔧 Configuration

Before uploading the firmware, configure the Firebase endpoint:

```cpp
const char* FIREBASE_URL =
  "https://YOUR-PROJECT-default-rtdb.REGION.firebasedatabase.app";
```

Each ESP32 sensor should have a unique node ID:

```cpp
#define NODE_ID 1
```

For another sensor:

```cpp
#define NODE_ID 2
```

The activity threshold can be adjusted:

```cpp
const uint16_t ALERT_THRESHOLD = 500;
```

---

# 📡 Wi-Fi Setup

On startup, the ESP32 can create a configuration access point:

```text
ExaShield-Setup-1
```

Connect a smartphone or computer to the network and open:

```text
192.168.4.1
```

The administrator can then select the examination network's Wi-Fi credentials and configure the ESP32.

---

# 🖥️ Admin Dashboard

The administrator dashboard contains four primary areas.

## Dashboard

Displays:

- Overall system statistics
- Online nodes
- Current activity
- Alerts
- System health

## Sensors

Displays:

- Node ID
- Online/offline status
- Activity
- Wi-Fi channel
- RSSI
- Last seen
- Uptime

## Alerts

Displays:

- High-activity events
- Node information
- Timestamp
- Activity level
- Verification status

## Events

Displays historical sensor activity.

---

# 🔒 Privacy & Responsible Use

ExaShield is intentionally designed around activity-based monitoring.

The system does **not** attempt to:

- Identify individual students
- Capture student identities
- Determine who is responsible for an activity
- Automatically declare cheating
- Inspect communication content
- Read messages
- Capture passwords
- Replace human invigilators

A detected wireless or RF activity event should be treated as an indicator requiring human verification.

Wireless signals can originate from legitimate devices, infrastructure, neighboring environments, or other sources.

Any future cellular/RF monitoring functionality should also be designed and deployed in accordance with applicable institutional policies, privacy requirements, spectrum regulations, and local laws.

---

# ⚠️ Limitations

The current prototype has several limitations:

- Wireless packet activity does not directly prove cheating.
- RF activity does not directly identify a specific person or device.
- Activity measurements depend on environmental conditions.
- Wi-Fi traffic can vary significantly between locations.
- RF measurements depend on antenna and front-end characteristics.
- Detection thresholds require controlled calibration.
- ESP32 monitoring capabilities depend on supported hardware and firmware.
- AD8318 measures RF power rather than communication content.
- Cellular RF monitoring requires appropriate frequency-specific RF hardware and filtering.
- Internet connectivity is required for real-time Firebase synchronization.
- Multiple sensor nodes may require calibration for comparable measurements.
- Security rules and authentication must be properly configured for production use.
- The prototype should be tested in controlled environments before deployment.

---

# 🔮 Future Development Roadmap

## Phase 1 — Current Prototype

- ESP32 sensor node
- Wi-Fi activity monitoring
- Firebase integration
- Real-time dashboard
- Activity classification
- Alert generation
- Multiple-node architecture

## Phase 2 — System Improvement

- Firebase Authentication
- Role-based administrator access
- Improved dashboard
- Historical analytics
- Activity graphs
- Sensor health monitoring
- Offline data buffering
- Hardware buzzer

## Phase 3 — RF Monitoring

- AD8318 integration
- RF power measurement
- Appropriate antenna and RF front-end
- Frequency-specific filtering
- ESP32 ADC integration
- RF activity logging
- RF threshold calibration

## Phase 4 — Multi-Sensor Intelligence

- Wi-Fi + RF activity correlation
- Multiple sensor comparison
- Sensor-location mapping
- Adaptive thresholds
- Environmental baseline analysis
- Anomaly detection

## Phase 5 — Advanced Platform

- AI/ML-based anomaly analysis
- Multi-hall monitoring
- Mobile administrator application
- Centralized institutional monitoring
- Advanced analytics
- Long-term activity trends

---

# 🧪 Testing Approach

The prototype should be evaluated in a controlled environment.

## Wi-Fi Monitoring

Testing should include:

- Normal examination environment
- Different numbers of wireless devices
- Different sensor locations
- Different Wi-Fi conditions
- Different activity thresholds
- Multiple ESP32 nodes

## System Reliability

Testing should include:

- Firebase connectivity loss
- Wi-Fi disconnection
- Wi-Fi reconnection
- Sensor restart
- Dashboard synchronization
- Heartbeat monitoring

## Alert System

Testing should include:

- LOW activity
- MEDIUM activity
- HIGH activity
- Repeated activity
- Multiple simultaneous sensor alerts

---

# 📡 Future RF Module Testing

The AD8318-based module should be tested using controlled RF sources and appropriate test equipment.

Testing should evaluate:

- RF measurement consistency
- Sensor response
- Frequency-dependent behavior
- Noise floor
- Environmental interference
- Antenna performance
- Threshold calibration

Performance should be evaluated using measurable indicators such as:

- Detection consistency
- False-positive rate
- Detection latency
- Sensor reliability
- Firebase synchronization latency
- Measurement stability

---

# 🎓 Feasibility

ExaShield is technically feasible as an academic prototype.

The current ESP32-based system uses affordable and widely available embedded hardware combined with Firebase and modern web technologies.

The modular architecture allows additional sensor nodes and future sensing hardware to be added without completely redesigning the cloud and dashboard architecture.

The planned AD8318 integration provides a possible path toward studying RF activity beyond Wi-Fi, although a practical cellular/RF monitoring implementation would require appropriate RF front-end design, calibration, and regulatory consideration.

---

# 🌟 Innovation

The main innovative aspects of ExaShield include:

1. Low-cost ESP32-based wireless activity monitoring
2. Real-time Firebase cloud synchronization
3. Centralized web-based monitoring
4. Automated activity-based alerts
5. Multiple sensor-node architecture
6. Anonymous activity-based monitoring
7. Modular RF sensing architecture
8. Future AD8318-based RF power monitoring
9. Wi-Fi and RF activity correlation
10. Scalable examination-hall architecture
11. Future AI/ML anomaly-detection capability

---

# 📈 Scalability

ExaShield is designed with scalability in mind.

## Small Deployment

```text
1 Examination Hall
      │
      ├── NODE-1
      └── NODE-2
```

## Larger Deployment

```text
University
│
├── Hall A
│   ├── NODE-1
│   ├── NODE-2
│   └── NODE-3
│
├── Hall B
│   ├── NODE-4
│   ├── NODE-5
│   └── NODE-6
│
└── Hall C
    ├── NODE-7
    └── NODE-8
```

All sensor nodes can report to a centralized backend and dashboard.

---

# 🔐 Production Security Considerations

Before production deployment, the following should be implemented:

- Firebase Authentication
- Strong Firebase Security Rules
- Role-based access control
- Secure API/database access
- Device authentication
- Unique node credentials
- Secure configuration storage
- Input validation
- Event logging
- Administrator audit logs
- Secure firmware update mechanism

> The prototype should **not** be deployed in a production environment using unrestricted database permissions.

---

# 📌 Project Status

**Current Status:** Prototype / Academic Project

The current system is intended for:

- Academic research
- Demonstration
- Controlled testing
- IoT experimentation
- Wireless activity research

The AD8318 RF monitoring component is currently planned as a future-development module and is not part of the current prototype unless separately implemented and validated.

---

# 📜 Disclaimer

ExaShield is an experimental academic IoT monitoring project.

Wireless activity measurements should not be interpreted as proof of examination misconduct.

Any alert generated by the system should be reviewed by authorized examination personnel before any action is taken.

The system should not be used to identify, track, intercept, inspect, or monitor the communications of individual students.

Any future RF/cellular monitoring implementation should be developed and deployed responsibly and in accordance with applicable institutional policies, privacy requirements, spectrum regulations, and local laws.

---

# 👨‍💻 Team

## H&S Tech

**MH Hridoy**  
**Umma Habiba Sumi**

**RTM Al-Kabir Technical University**  
**Sylhet, Bangladesh**

---

# ⭐ Project Vision

> **“Detect Wireless Activity Before It Becomes a Problem.”**

ExaShield aims to provide examination authorities with an affordable, scalable, and real-time wireless activity monitoring platform that supports human invigilators without replacing human judgment.

The long-term vision is to combine **ESP32 Wi-Fi monitoring, RF power sensing, cloud computing, real-time analytics, and intelligent anomaly detection** into a modular platform for responsible examination-environment monitoring.
