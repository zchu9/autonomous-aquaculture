#ifndef CONFIG_H
#define CONFIG_H

// --- WiFi Credentials ---
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// --- MQTT Configuration ---
const char* MQTT_SERVER = "YOUR_MQTT_BROKER_IP";
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "sensor/data";

#endif // CONFIG_H
