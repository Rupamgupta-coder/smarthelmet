// 🔹 Define Blynk Template Info
#define BLYNK_TEMPLATE_ID "TMPL3XA2P-Wi3"
#define BLYNK_TEMPLATE_NAME "Fall Detection Helmet"
#define BLYNK_AUTH_TOKEN "FC3mmjQs-V4EgEn8jMzjVgk-gfMe_shd"

// 🔹 Include Libraries
#define BLYNK_PRINT Serial  // Enable Serial Debugging
#include <Wire.h>
#include <MPU6050_tockn.h>  // MPU6050 library (Ensure it's installed correctly)
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// 🔹 Define Pin Connections
#define BUZZER 19
#define BUTTON 18
#define SDA_PIN 21
#define SCL_PIN 22

// 🔹 WiFi Credentials
char ssid[] = "Rupam";        // Change to your WiFi SSID
char pass[] = "00000000";     // Change to your WiFi Password

// 🔹 Create Instances
MPU6050 mpu6050(Wire);  // MPU6050 Object
BlynkTimer timer;

// 🔹 Fall Detection Variables
bool fallDetected = false;
unsigned long fallTime = 0;
const float FALL_THRESHOLD = -0.5;  // Adjust based on testing (Negative for downward acceleration)

// 🔹 Setup Function
void setup() {
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN);

    pinMode(BUZZER, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);

    // 🔹 Initialize MPU6050
    mpu6050.begin();
    mpu6050.calcGyroOffsets(true);
    Serial.println("✅ MPU6050 Initialized");

    // 🔹 Connect to Blynk
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    Serial.println("✅ Blynk Connected!");

    // 🔹 Setup timers
    timer.setInterval(500, checkFall);   // Check fall every 500ms
    timer.setInterval(1000, checkButton); // Check button every 1 sec
    timer.setInterval(1000, checkAlert);  // Check alert every 1 sec
}

// 🔹 Function to Detect Fall
void checkFall() {
    mpu6050.update();
    float accZ = mpu6050.getAccZ();  // Read Z-axis acceleration

    if (accZ < FALL_THRESHOLD) {  // Fall detected (Threshold should be negative)
        fallDetected = true;
        fallTime = millis();
        digitalWrite(BUZZER, HIGH); // Start buzzer
        Serial.println("⚠ Fall detected! Press button within 10 sec to cancel alert.");
    }
}

// 🔹 Function to Send Alert if Not Canceled
void checkAlert() {
    if (fallDetected && (millis() - fallTime >= 10000)) { // If 10 sec passed
        Serial.println("⏳ No response! Sending alert via Blynk...");
        Blynk.logEvent("fall_alert", "🚨 Fall Detected! Check immediately!");
        digitalWrite(BUZZER, LOW);
        fallDetected = false;
    }
}

// 🔹 Function to Check Button Press for Canceling Alert
void checkButton() {
    if (fallDetected && digitalRead(BUTTON) == LOW) {
        Serial.println("✅ Alert canceled by user.");
        digitalWrite(BUZZER, LOW);
        fallDetected = false;
    }
}

// 🔹 Loop Function
void loop() {
    Blynk.run();  // Run Blynk
    timer.run();  // Run Timers (NO SPECIAL CHARACTERS HERE)
}
