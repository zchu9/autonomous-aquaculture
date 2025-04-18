import paho.mqtt.client as mqtt
import json
import time

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, reason_code, properties):
    print(f"Connected with result code {reason_code}")
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("$SYS/#")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    print(msg.topic+" "+str(msg.payload))

mqttc = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
mqttc.on_connect = on_connect
mqttc.on_message = on_message

mqttc.connect("localhost", 1883, 60)

# Test Sensor Data Payload
sensor_data = {
    "height": 1.0,
    "temperature": 1.0,
}

# Test System Level Data Payload
system_level_data = {
    "smart_shunt": {
        "battery_voltage": 12.0,
        "battery_current": 1.0,
        "power": 12.0,
        "alarm_reason": "LOW VOLTAGE",
        "alarm_status": True,
        "low_volt_alarm_count": 1
    },
    "renogy_mppt": {
        "solar_panel_voltage": 17.8,
        "solar_panel_current": 2.5,
        "solar_panel_power": 44.5,
        "controller_temp": 29.0
    }
}

# Publish Sensor Data
print("Publishing Sensor Data...")
mqttc.publish("farm/67f966b2b9c6c0d234547e30/sensorData", json.dumps(sensor_data))
time.sleep(60)

# Publish System Level Data
print("Publishing System Level Data...")
mqttc.publish("farm/67f966b2b9c6c0d234547e30/systemLevels", json.dumps(system_level_data))
