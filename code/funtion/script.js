import {
    initializeApp
}
from
"https://***********************ase-app.js";

import {
    getDatabase,
    ref,
    onValue,
    query,
    orderByChild,
    limitToLast
}
from
"https://www.g***********************firebase-database.js";

const firebaseConfig = {

    apiKey:  "A************************co",

    authDomain: "ex***********************com",

    databaseURL: "https://exa***********************e.app",

    projectId: "e***********************4",

    storageBucket: "ex***********************app",

    messagingSenderId:  "104***********************960",

    appId:  "1:1***********************4c94d5",

    measurementId:  "***********************HF"
};

const app =
    initializeApp(
        firebaseConfig
    );

const db =
    getDatabase(
        app
    );

const connection =
    document.getElementById(
        "connection"
    );

const soundButton =
    document.getElementById(
        "soundButton"
    );

const totalSensors =
    document.getElementById(
        "totalSensors"
    );

const onlineSensors =
    document.getElementById(
        "onlineSensors"
    );

const alertCount =
    document.getElementById(
        "alertCount"
    );

const systemStatus =
    document.getElementById(
        "systemStatus"
    );

const message =
    document.getElementById(
        "message"
    );

const sensorGrid =
    document.getElementById(
        "sensorGrid"
    );

const alerts =
    document.getElementById(
        "alerts"
    );

const events =
    document.getElementById(
        "events"
    );

const popupBG =
    document.getElementById(
        "popupBG"
    );

const popupText =
    document.getElementById(
        "popupText"
    );

const closePopup =
    document.getElementById(
        "closePopup"
    );

let audioContext = null;

let soundEnabled = false;

let lastAlertID = null;

soundButton.addEventListener(
    "click",
    async () => {

        try {

            if (!audioContext) {

                audioContext =
                    new (
                        window.AudioContext ||
                        window.webkitAudioContext
                    )();
            }

            if (
                audioContext.state ===
                "suspended"
            ) {

                await audioContext.resume();
            }

            soundEnabled = true;

            soundButton.textContent =
                "🔊 Sound ON";

            beep(
                700,
                150
            );

        } catch (error) {

            console.error(
                error
            );
        }
    }
);

function beep(
    frequency,
    duration
) {

    if (
        !soundEnabled ||
        !audioContext
    ) {

        return;
    }

    const oscillator =
        audioContext.createOscillator();

    const gain =
        audioContext.createGain();

    oscillator.type =
        "square";

    oscillator.frequency.value =
        frequency;

    gain.gain.setValueAtTime(
        0.0001,
        audioContext.currentTime
    );

    gain.gain.exponentialRampToValueAtTime(
        0.25,
        audioContext.currentTime + 0.02
    );

    gain.gain.exponentialRampToValueAtTime(
        0.0001,
        audioContext.currentTime +
        duration / 1000
    );

    oscillator.connect(
        gain
    );

    gain.connect(
        audioContext.destination
    );

    oscillator.start();

    oscillator.stop(
        audioContext.currentTime +
        duration / 1000
    );
}

function alarm() {

    beep(
        1000,
        250
    );

    setTimeout(
        () => {

            beep(
                700,
                250
            );

        },
        300
    );

    setTimeout(
        () => {

            beep(
                1000,
                350
            );

        },
        600
    );
}

const connectedRef =
    ref(
        db,
        ".info/connected"
    );

onValue(
    connectedRef,
    snapshot => {

        const connected =
            snapshot.val() === true;

        if (connected) {

            connection.textContent =
                "● Firebase ONLINE";

            connection.className =
                "connection online";

            systemStatus.textContent =
                "ONLINE";

            systemStatus.className =
                "green";

            message.textContent =
                "Firebase connection established.";

        } else {

            connection.textContent =
                "● Firebase OFFLINE";

            connection.className =
                "connection offline";

            systemStatus.textContent =
                "OFFLINE";

            systemStatus.className =
                "red";

            message.textContent =
                "Firebase connection lost.";
        }
    }
);

onValue(
    ref(
        db,
        "sensors"
    ),
    snapshot => {

        renderSensors(
            snapshot.val() || {}
        );
    }
);

function renderSensors(
    data
) {

    sensorGrid.innerHTML = "";

    const list =
        Object.entries(
            data
        );

    totalSensors.textContent =
        list.length;

    let online = 0;

    if (
        list.length === 0
    ) {

        sensorGrid.innerHTML =
            `
            <div class="empty">
                No sensor connected yet.
            </div>
            `;

        onlineSensors.textContent =
            "0";

        return;
    }

    list.forEach(
        ([id, sensor]) => {

            const activity =
                Number(
                    sensor.activity || 0
                );

            const packets =
                Number(
                    sensor.packets || 0
                );

            const rssi =
                Number(
                    sensor.wifiRssi ||
                    sensor.rssi ||
                    0
                );

            const channel =
                Number(
                    sensor.channel || 0
                );

            const level =
                sensor.level ||
                (
                    activity >= 500
                    ? "HIGH"
                    : activity >= 200
                    ? "MEDIUM"
                    : "LOW"
                );

            const isOnline =
                sensor.status ===
                "ONLINE";

            if (isOnline) {
                online++;
            }

            const percentage =
                Math.min(
                    100,
                    activity / 10
                );

            const card =
                document.createElement(
                    "div"
                );

            card.className =
                "sensor";

            card.innerHTML =
                `

                <div class="sensorTop">

                    <div class="node">
                        ${id.toUpperCase()}
                    </div>

                    <div class="status ${
                        isOnline
                        ? "online"
                        : "offline"
                    }">

                        ${
                            isOnline
                            ? "● ONLINE"
                            : "● OFFLINE"
                        }

                    </div>

                </div>

                <div class="label">
                    WIRELESS ACTIVITY
                </div>

                <div class="activity">
                    ${activity}
                </div>

                <div class="level ${
                    level.toLowerCase()
                }">
                    ${level}
                </div>

                <div class="bar">

                    <div
                        class="barFill"
                        style="width:${percentage}%"
                    ></div>

                </div>

                <div class="details">

                    <span>
                        Packets: ${packets}
                    </span>

                    <span>
                        Channel: ${channel}
                    </span>

                    <span>
                        Wi-Fi RSSI: ${rssi} dBm
                    </span>

                    <span>
                        Node ID: ${sensor.nodeId || "-"}
                    </span>

                </div>

                `;

            sensorGrid.appendChild(
                card
            );
        }
    );

    onlineSensors.textContent =
        online;

    message.textContent =
        `${online} of ${list.length} sensor node(s) online.`;
}

const alertsQuery =
    query(
        ref(
            db,
            "alerts"
        ),
        orderByChild(
            "timestamp"
        ),
        limitToLast(
            30
        )
    );

onValue(
    alertsQuery,
    snapshot => {

        renderAlerts(
            snapshot.val() || {}
        );
    }
);

function renderAlerts(
    data
) {

    alerts.innerHTML = "";

    const list =
        Object.entries(
            data
        ).reverse();

    alertCount.textContent =
        list.length;

    if (
        list.length === 0
    ) {

        alerts.innerHTML =
            `
            <div class="empty">
                No alerts.
            </div>
            `;

        return;
    }

    list.forEach(
        ([id, alert]) => {

            const item =
                document.createElement(
                    "div"
                );

            item.className =
                "alert";

            item.innerHTML =
                `

                <div class="alertTitle">
                    ⚠ HIGH WIRELESS ACTIVITY
                </div>

                <div class="alertText">

                    Sensor:
                    NODE-${alert.nodeId || "-"}

                    <br>

                    Activity:
                    ${alert.activity || 0}

                    <br>

                    Packets:
                    ${alert.packets || 0}

                    <br>

                    Channel:
                    ${alert.channel || "-"}

                    <br>

                    Signal:
                    ${alert.wifiRssi || "-"} dBm

                    <br>

                    Verification:
                    ${alert.verification || "PENDING"}

                    <br>

                    ${
                        alert.message ||
                        "Human verification required."
                    }

                </div>

                `;

            alerts.appendChild(
                item
            );
        }
    );

    const newest =
        list[0];

    if (
        newest &&
        lastAlertID !== null &&
        newest[0] !== lastAlertID
    ) {

        alarm();

        popupText.textContent =
            `Node ${newest[1].nodeId || "-"} ` +
            `reported HIGH wireless activity. ` +
            `Please verify the situation.`;

        popupBG.classList.remove(
            "hidden"
        );
    }

    if (newest) {
        lastAlertID =
            newest[0];
    }
}

closePopup.addEventListener(
    "click",
    () => {

        popupBG.classList.add(
            "hidden"
        );
    }
);

const eventsQuery =
    query(
        ref(
            db,
            "events"
        ),
        orderByChild(
            "timestamp"
        ),
        limitToLast(
            50
        )
    );

onValue(
    eventsQuery,
    snapshot => {

        renderEvents(
            snapshot.val() || {}
        );
    }
);

function renderEvents(
    data
) {

    events.innerHTML = "";

    const list =
        Object.entries(
            data
        ).reverse();

    if (
        list.length === 0
    ) {

        events.innerHTML =
            `
            <div class="empty">
                No events.
            </div>
            `;

        return;
    }

    list.forEach(
        ([id, event]) => {

            const item =
                document.createElement(
                    "div"
                );

            item.className =
                "event";

            item.innerHTML =
                `

                <div class="eventNode">
                    NODE-${event.nodeId || "-"}
                </div>

                <div class="eventData">
                    Activity:
                    ${event.activity || 0}
                </div>

                <div class="eventData">
                    Packets:
                    ${event.packets || 0}
                </div>

                <div class="eventData">
                    Level:
                    ${event.level || "LOW"}
                </div>

                `;

            events.appendChild(
                item
            );
        }
    );
}


/* ===== SECTION BUTTON CONTROLS ===== */
const navButtons = document.querySelectorAll(".navBtn");
const targetButtons = document.querySelectorAll("[data-target]");

function goToSection(id){
    const target = document.getElementById(id);
    if(!target) return;
    target.scrollIntoView({behavior:"smooth", block:"start"});

    navButtons.forEach(btn=>{
        btn.classList.toggle("active", btn.dataset.target === id);
    });
}

targetButtons.forEach(btn=>{
    btn.addEventListener("click", ()=>{
        if(btn.dataset.target) goToSection(btn.dataset.target);
    });
});

window.addEventListener("scroll", ()=>{
    const sections = [
        document.getElementById("dashboardSection"),
        document.getElementById("sensorSection"),
        document.getElementById("alertSection"),
        document.getElementById("eventSection")
    ];

    let current = "dashboardSection";
    for(const section of sections){
        if(!section) continue;
        if(window.scrollY + 190 >= section.offsetTop){
            current = section.id;
        }
    }

    navButtons.forEach(btn=>{
        btn.classList.toggle("active", btn.dataset.target === current);
    });
});

document.getElementById("refreshSensors")?.addEventListener("click", ()=>{
    message.textContent = "Sensor data is live. Refreshing the dashboard view...";
    setTimeout(()=>{
        message.textContent = "Sensor data is connected to Firebase real-time updates.";
    }, 900);
});

document.getElementById("refreshEvents")?.addEventListener("click", ()=>{
    message.textContent = "Event history is already synchronized with Firebase.";
});

document.getElementById("clearAlertView")?.addEventListener("click", ()=>{
    alerts.innerHTML = `<div class="empty">Alert view cleared. New Firebase alerts will appear automatically.</div>`;
    alertCount.textContent = "0";
    message.textContent = "Alert view cleared locally. Firebase data was not deleted.";
});
