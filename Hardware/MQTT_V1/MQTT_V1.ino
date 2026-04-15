#include <WiFi.h>
#include <PubSubClient.h>

// ================= WIFI =================
const char* ssid = "Pommes";
const char* password = "12345678";

// ================= MQTT =================
const char* broker = "10.189.65.21";
const char* topicGPS = "exploratag/gpsinfo";
const char* topicBuzzer = "exploratag/buzzer";

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

// ================= STATE =================
bool buzzerState = false;
uint32_t lastSend = 0;

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
        }

        if (msg == "OFF") {
            buzzerState = false;
            Serial.println("OFF");
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
        Serial.print("Connecting MQTT...");

        if (mqtt.connect("ESP32_Client")) {
            Serial.println("OK");
            mqtt.subscribe(topicBuzzer);
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

    connectWiFi();

    mqtt.setServer(broker, 1883);
    mqtt.setCallback(callback);
}

// ================= LOOP =================
void loop()
{
    if (!mqtt.connected()) mqttConnect();
    mqtt.loop();

    if (millis() - lastSend > 15000)
    {
        lastSend = millis();

        // Dummy GPS (hier kannst du echten GPS Code einfügen)
        float lat = 52.123456;
        float lon = 10.123456;

        String payload =
            String("{\"lat\":") + lat +
            ",\"lon\":" + lon + "}";

        mqtt.publish(topicGPS, payload.c_str());

        Serial.println("GPS sent via WiFi");
        
    }
}