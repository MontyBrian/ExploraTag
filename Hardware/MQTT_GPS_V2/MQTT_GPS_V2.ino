#include <WiFi.h>
#include <PubSubClient.h>
#include "utilities.h"

#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial


// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// See all AT commands, if wanted
// #define DUMP_AT_COMMANDS


#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS  // if enabled it requires the streamDebugger lib
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif


// ================= WIFI =================
const char* ssid = "Pommes";
const char* password = "12345678";

// ================= MQTT =================
const char* broker = "10.189.65.21";
const char* topicGPS = "exploratag/gpsinfo";
const char* topicBuzzer = "exploratag/buzzer";
const char* topicUser = "exploratag/benutzer";

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

// ================= STATE =================
bool buzzerState = false;
uint32_t lastSend = 0;
uint32_t lastErrorSend = 0;
bool user = false;
const int buzzerPin = 33;

// ================= CALLBACK =================
void callback(char* topic, byte* payload, unsigned int length)
{
    String msg;

    for (int i = 0; i < length; i++) {
        msg += (char)payload[i];
    }

    if (strcmp(topic, topicBuzzer) == 0) {

        if (msg == "ON") {
            buzzerState = true;
            Serial.println("ON");
            mqtt.publish(topicGPS, msg.c_str());
            tone(buzzerPin, 2000);
        }else if (msg == "OFF") {
            buzzerState = false;
            Serial.println("OFF");
            mqtt.publish(topicGPS, msg.c_str());
            noTone(buzzerPin);
        }
    }else if (strcmp(topic, topicUser) == 0) {

        if (msg == "new_user") {
            user = true;
            Serial.println("new User");
            mqtt.publish(topicGPS, msg.c_str());
        }else if (msg == "del_user") {
            user = false;
            Serial.println("del user");
            mqtt.publish(topicGPS, msg.c_str());
        }
    }
}

// ================= CONNECT WIFI =================
void connectWiFi()
{
    WiFi.begin(ssid, password);

    Serial.print("Connecting WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected");
}

// ================= MQTT CONNECT =================
void mqttConnect()
{
    while (!mqtt.connected()) {
        if (WiFi.status() != WL_CONNECTED){
            connectWiFi();
        }

        Serial.print("Connecting MQTT...");

        if (mqtt.connect("ESP32_Client")) {
            Serial.println("OK");
            mqtt.subscribe(topicBuzzer);
            mqtt.subscribe(topicUser);
        } else {
            Serial.println("fail, retry...");
            delay(2000);
        }
    }
}

// ================= SETUP =================
void setup()
{
    Serial.begin(115200);
    #ifdef BOARD_POWERON_PIN
    /* Set Power control pin output
    * * @note      Known issues, ESP32 (V1.2) version of T-A7670, T-A7608,
    *            when using battery power supply mode, BOARD_POWERON_PIN (IO12) must be set to high level after esp32 starts, otherwise a reset will occur.
    * */
    pinMode(BOARD_POWERON_PIN, OUTPUT);
    digitalWrite(BOARD_POWERON_PIN, HIGH);
#endif

    // Set modem reset pin ,reset modem
#ifdef MODEM_RESET_PIN
    pinMode(MODEM_RESET_PIN, OUTPUT);
    digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL); delay(100);
    digitalWrite(MODEM_RESET_PIN, MODEM_RESET_LEVEL); delay(2600);
    digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL);
#endif

#ifdef MODEM_DTR_PIN
    // Pull down DTR to ensure the modem is not in sleep state
    Serial.printf("Set DTR pin %d LOW\n", MODEM_DTR_PIN);
    pinMode(MODEM_DTR_PIN, OUTPUT);
    digitalWrite(MODEM_DTR_PIN, LOW);
#endif

    // Turn on modem
    pinMode(BOARD_PWRKEY_PIN, OUTPUT);
    digitalWrite(BOARD_PWRKEY_PIN, LOW);
    delay(100);
    digitalWrite(BOARD_PWRKEY_PIN, HIGH);
    delay(MODEM_POWERON_PULSE_WIDTH_MS);
    digitalWrite(BOARD_PWRKEY_PIN, LOW);

    // Set modem baud
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);

    Serial.println("Start modem...");
    delay(3000);

    int retry = 0;
    while (!modem.testAT(1000)) {
        Serial.println(".");
        if (retry++ > 30) {
            digitalWrite(BOARD_PWRKEY_PIN, LOW);
            delay(100);
            digitalWrite(BOARD_PWRKEY_PIN, HIGH);
            delay(MODEM_POWERON_PULSE_WIDTH_MS);
            digitalWrite(BOARD_PWRKEY_PIN, LOW);
            retry = 0;
        }
    }
    Serial.println();
    delay(200);
    String modemName = "UNKNOWN";
    int tries = 0;
    while (modemName == "UNKNOWN" && tries < 10) {
        modemName = modem.getModemName();
        if (modemName == "UNKNOWN") {
            Serial.println("Unable to obtain module information normally, try again");
            delay(1000);
        } else {
            Serial.print("Model Name:");
            Serial.println(modemName);
            break;
        }
        tries++;
        delay(5000);
    }

    // Print modem software version
    String res;
    modem.sendAT("+SIMCOMATI");
    modem.waitResponse(10000UL, res);
    Serial.println(res);

    Serial.println("Enabling GPS/GNSS/GLONASS");
    while (!modem.enableGPS(MODEM_GPS_ENABLE_GPIO, MODEM_GPS_ENABLE_LEVEL)) {
        Serial.print(".");
    }
    Serial.println();
    Serial.println("GPS Enabled");

    // Set GPS Baud to 115200
    modem.setGPSBaud(115200);


    connectWiFi();

    mqtt.setServer(broker, 1883);
    mqtt.setCallback(callback);
    pinMode(buzzerPin, OUTPUT);
}

// ================= LOOP =================
void loop()
{
    if (!mqtt.connected()){ 
        mqttConnect();
    }
    mqtt.loop();
if(user == true){
    float lat2      = 0;
    float lon2      = 0;
    float speed2    = 0;
    float alt2      = 0;
    int   vsat2     = 0;
    int   usat2     = 0;
    float accuracy2 = 0;
    int   year2     = 0;
    int   month2    = 0;
    int   day2      = 0;
    int   hour2     = 0;
    int   min2      = 0;
    int   sec2      = 0;
    uint8_t    fixMode   = 0;
            
        if (millis() - lastSend > 15000)
    {
        lastSend = millis();
        Serial.println("Requesting current GPS/GNSS/GLONASS location");
        if (modem.getGPS(&fixMode, &lat2, &lon2, &speed2, &alt2, &vsat2, &usat2, &accuracy2,
                         &year2, &month2, &day2, &hour2, &min2, &sec2)) {

        String payload =
  String("{\"lat\":") + String(lat2,8) +
  ",\"lon\":" + String(lon2,8) +
  ",\"Date\":\"" + String(year2) + " " + String(month2) + " " + String(day2) + "\"}";
  Serial.println(lat2,8);
  Serial.println(lon2,8);
  Serial.println(payload);

        mqtt.publish(topicGPS, payload.c_str());

        Serial.println("GPS sent via WiFi");
        } else {
          if (millis() - lastErrorSend > 15000)
        {
            lastErrorSend = millis();
            Serial.println("GPS konnte nicht gefunden werden, versucht erneut in 15 Sekunden.");
            mqtt.publish(topicGPS, "GPS konnte nicht gefunden werden, versucht erneut in 15 Sekunden.");
        }
        }
    }
}
}