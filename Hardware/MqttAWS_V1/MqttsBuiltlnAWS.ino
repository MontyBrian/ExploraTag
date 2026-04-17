#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <time.h>

// ================= WIFI =================
const char* ssid = "Pommes";
const char* password = "12345678";

// ================= AWS IoT =================
const char* mqttServer = "a31cihr08e2bw-ats.iot.eu-central-1.amazonaws.com";
const int mqttPort = 8883;

// Root CA
static const char rootCA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

// Device Certificate
static const char clientCert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIURw2Cr/bJ7MRcYG8dEYT5PNFGi6owDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI2MDQxNjEyMzYw
MFoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAPG4X+nWxMp7sZlSP3yg
/TKJYOqFMqiSGolvi04CvQRbX+DXlxkWrJCP16XSf71r1/iIqCZFD7Ukgz9iDj1r
UgOiSwQ2/esqFBStXozFi5XLuOkTBYgEwSZVU03L6mKdbIhQCEND8mUimKJr9E3R
5mJeh452Eazz8FiNiomipOYC6MSbEYrBTV6ORYlb4u2i8JTv8uBE9eJk1zRrI15S
2OIE2p+ivkx8nWxKn9mP9Plzfu1X29RvzB8xx1uKYcO2jILa9Ix1Ywg1SIYzQGQS
VbVDVpGl43RMpf7+e9hKS92dqMoC/wlJTa4V0biYovMAdnNFK4x0udCu3khyznDY
zRkCAwEAAaNgMF4wHwYDVR0jBBgwFoAUQJnePRs7wrMUSzi30W4huxpU+J0wHQYD
VR0OBBYEFOSo4/Hj73FApGvsSS45HckYYdorMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCVqyOHeXY2x5CsSZw7lg1hjesG
Ip0zYOSYkvusJKUOQFZrRMeSbYIrjC93zkjpiw/OK2sQ5/drb7p7QAixcsKqooxF
Vtwaurtu+aG8qk+B7ju7GaVnmzeU6cbMOYJssLh7L4rmYAALOlWZid7UAzebpb1Z
KavBGHrxf+twstAxI5IrWd2VlCfdiWQr3TvnnA2UGb+Dr8qOn0EwoZ0ZsvXqIhGa
Lpqe2MZagN5U152aix7nhnYBiaGYOf3/hxwWy6JOspwWI4++1U245M8+Dfx9I2WX
Xe3e83/89xoIEer6n8VWhyRvOpSzhpPD/HIgdKEb/x2TddSJHA5IC5RsT9Z4
-----END CERTIFICATE-----
)EOF";

// Private Key
static const char privateKey[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEA8bhf6dbEynuxmVI/fKD9Molg6oUyqJIaiW+LTgK9BFtf4NeX
GRaskI/XpdJ/vWvX+IioJkUPtSSDP2IOPWtSA6JLBDb96yoUFK1ejMWLlcu46RMF
iATBJlVTTcvqYp1siFAIQ0PyZSKYomv0TdHmYl6HjnYRrPPwWI2KiaKk5gLoxJsR
isFNXo5FiVvi7aLwlO/y4ET14mTXNGsjXlLY4gTan6K+THydbEqf2Y/0+XN+7Vfb
1G/MHzHHW4phw7aMgtr0jHVjCDVIhjNAZBJVtUNWkaXjdEyl/v572EpL3Z2oygL/
CUlNrhXRuJii8wB2c0UrjHS50K7eSHLOcNjNGQIDAQABAoIBAQCfDedvA3dylXoD
nW9RHs39FxDCwOrYqWr0zIUpy/t8hnPMi5BNXfInHb+UybJsfi4ayDF9HSx80Y3b
gzZDi/7njT83gsgKmiGkiYARuVtkMi12kKGvIxHFm5e5NVkBkvz7pU4KlQdHLUCq
f8a9/kROrk0cnOx8lfetDVX/wxomjrQPAvwxMGtBnM8LxAY0ZdA26MMQ9E4YLqkV
uKAUV1QpSNIWDjrKPGI9YlopMbQicJrC7+qkrLDlHOjk3OrRhs4vcsTGF6s92sNi
cCCEvatYnZNEFaNUulor5sbM+Ul/Iz0Tt2eVr2QsXbqGTZigPJoHpOisQqTqad+L
EaaG++WRAoGBAP5blFUNLbutL4yWwDIZa7GVUKW9diOoEP4shrAnA23aw+VFkXPm
IKAmPPt1JgKWa87fRye0te1F7i56GOdx4FrihWhaLvYafk63dX70cZq2debNh8Fd
owoBozh8NfvIOhkFP6/9hEaX/rkgsIMcDZqIaWHRQ+xzHrw4Szx+z7nvAoGBAPNH
6DRPJoDkiodpRRRL2G1AG/QOdjVu/ZKrs8judm3Ni+EOXXYKkHC2ssVaiVqtFnlZ
G9rHLuD/soAahEMrDPn0GkD9RcbFvTy9LCePaPm9te3kZ/wtPAfN9HPxxTO0htia
KSIvLiNMH6apkpl6EqQK9/7qBVE3xHTbpW92l5F3AoGBAPlDBbnTkF26+j3r/a0a
/E6q5L2HTZ7jtMNhHxzBHrspB5BG/KN3gcFvt5M02mruvfedFz3x+dbsEN27CWzG
S/bExbfdqOfETsMVRi+DnVDgPepQwBjeikfkEu/nyX9UWJrsad0ts9L6YMVLI69Y
bnxh8ePKD6ZHqncJM7G1FBxBAoGAF3fOTI1+F1z0iA99nHGclR1kFOz4NjtYtrdd
hnqKbhkv7yvllCd8u4LESAltypN74YTZDQe92/KOk5D7RUd33vzPM4m7G2ujz6Io
CkDpTX4heKtZ5JewfMg/VpfyAATgJTDp2uRWPQ1w2DDvdWKzPaPZPHvyu1z31JYD
pXb5NA0CgYAmMwtDmUJ8PS9GXjxQ/h5oBYWiDNbLA3MGpM/+ipARfVU+UzCpV33m
h14QixG2bLt81QVbzseHG6kIXfuEUFPwFyD6c2WTAgNw2z5gUAn+GNAtNLqca4cq
yYuzr9jMlZZhJWv40JIDgp4kOduFDDheAOx1JKNx2d5LztZuSI2szg==
-----END RSA PRIVATE KEY-----
)EOF";

// ================= TOPICS =================
const char* topicGPS = "exploratag/gpsinfo";
const char* topicBuzzer = "exploratag/buzzer";
const char* topicUser = "exploratag/benutzer";

// ================= CLIENT =================
WiFiClientSecure secureClient;
PubSubClient mqtt(secureClient);

// ================= STATE =================
bool buzzerState = false;
bool userState = false;

uint32_t lastSend = 0;

// ================= PINS =================
const int buzzerPin = 27;

// ================= WIFI =================
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

// ================= TIME SYNC (IMPORTANT) =================
void syncTime()
{
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    Serial.print("Syncing time");
    time_t now = time(nullptr);

    while (now < 1700000000) {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }

    Serial.println("\nTime synced");
}

// ================= CALLBACK =================
void callback(char* topic, byte* payload, unsigned int length)
{
    char msg[length + 1];
    memcpy(msg, payload, length);
    msg[length] = '\0';

    Serial.print("Topic: ");
    Serial.println(topic);

    Serial.print("Msg: ");
    Serial.println(msg);

    if (strcmp(topic, topicBuzzer) == 0) {
        buzzerState = (strcmp(msg, "ON") == 0);
        Serial.println("Buzzer");
    }
    else if (strcmp(topic, topicUser) == 0) {
        userState = (strcmp(msg, "new_user") == 0);
    }
}

// ================= MQTT CONNECT =================
void connectMQTT()
{
    secureClient.setCACert(rootCA);
    secureClient.setCertificate(clientCert);
    secureClient.setPrivateKey(privateKey);

    secureClient.setHandshakeTimeout(30);

    mqtt.setServer(mqttServer, mqttPort);
    mqtt.setCallback(callback);
    mqtt.setBufferSize(512);

    String clientId = "ExploraTag001-" + String((uint32_t)ESP.getEfuseMac(), HEX);

    while (!mqtt.connected()) {
        Serial.print("Connecting MQTT...");

        if (mqtt.connect(clientId.c_str())) {
            Serial.println("connected");

            mqtt.subscribe(topicBuzzer);
            mqtt.subscribe(topicUser);

        } else {
            Serial.print("failed, rc=");
            Serial.println(mqtt.state());
            delay(2000);
        }
    }
}

// ================= SETUP =================
void setup()
{
    Serial.begin(115200);

    pinMode(buzzerPin, OUTPUT);

    connectWiFi();
    syncTime();
    connectMQTT();
}

// ================= LOOP =================
void loop()
{
    if (!mqtt.connected()) {
        connectMQTT();
    }

    mqtt.loop();

    digitalWrite(buzzerPin, buzzerState ? HIGH : LOW);

    // send GPS every 15s
    if (millis() - lastSend > 300000)
    {
        lastSend = millis();

        float lat = 51.82575607;
        float lon = 10.76302242;

const char* deviceId = "ExploraTag001";

    char payload[200];
    snprintf(payload, sizeof(payload),
             "{\"id\":\"%s\",\"lat\":%.8f,\"lon\":%.8f}",
             deviceId, lat, lon);

        mqtt.publish(topicGPS, payload);

        Serial.println(payload);
    }
}