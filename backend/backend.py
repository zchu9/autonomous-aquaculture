"""
author: Jayson De La Vega
created: 29/1/25

backend.py: The backend service for the oyster application
"""

from flask import Flask, request, jsonify
from flask_mqtt import Mqtt
from flask_cors import CORS
import time

MQTT_HOST_NAME = 'mqtt'
MQTT_PORT_NUM = 1883
PORT_NUM = 3000

app = Flask(__name__)
CORS(app, origin=["http://localhost:5173"])
app.config['MQTT_BROKER_URL'] = MQTT_HOST_NAME
app.config['MQTT_BROKER_PORT'] = MQTT_PORT_NUM
app.config['MQTT_REFRESH_TIME'] = 60.0  # refresh time in seconds
mqtt = Mqtt(app)

@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
    print(f"Successfully connected to MQTT broker on {MQTT_HOST_NAME}:{MQTT_PORT_NUM}")

    mqtt.subscribe('test/topic/1')

@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    print(f"Received message from MQTT broker on {MQTT_HOST_NAME}:{MQTT_PORT_NUM}")

@app.route("/")
def default():
    return f"HIII on port {PORT_NUM}"

@app.route("/test_pub", methods=['PUT', 'POST'])
def test_pub():
    mqtt.publish('test/topic', 'this is a test topic')
    return f"Published to MQTT broker"

@app.route("/farm/<id>/liftCages", methods=['PUT', 'POST'])
def farm_lift_cages(id):
    mqtt.publish(f'farm/{id}/liftCages', 'lift')
    return f"Requested to lift farm {id} cages"

@app.route("/farm/<id>/lowerCages", methods=['PUT', 'POST'])
def farm_lower_cages(id):
    mqtt.publish(f'farm/{id}/lowerCages', 'lift')
    return f"Requested to lower farm {id} cages"

@app.route("/sensor_data/<id>", methods=['GET'])
def get_sensor_data(id):
    return f"Here's your data"

@app.route('/device_data')
@app.route("/device_data/<id>", methods=['GET'])
def get_device_data(id=None):
    return f"Here's your data"

if __name__ == "__main__":
    
    app.run(host='0.0.0.0', port=PORT_NUM)