"""
author: Jayson De La Vega & Nathan Skirvin
created: 29/1/25

backend.py: The backend service for the oyster application
"""

import logging
import json
from flask import Flask, request
from flask_mqtt import Mqtt
from clock import get_utc_timestamp
from datetime import timedelta
from controllers.sensor_controller import add_sensor_data
from controllers.system_level_controller import add_system_levels
from models.lift_schedules import LiftActiveSchedule, LiftArchiveSchedule
from models.farms import Farm
from flask_cors import CORS
from apscheduler.schedulers.background import BackgroundScheduler
from apscheduler.triggers.interval import IntervalTrigger
from mongoengine import connect
from routes import main_blueprint
import os

connect(db='oyster_db', host='mongo', port=27017, alias='default')
logging.basicConfig(level=logging.DEBUG)

MQTT_HOST_NAME = 'mqtt'
MQTT_PORT_NUM = 1883
PORT_NUM = 3000

app = Flask(__name__)
app.register_blueprint(main_blueprint)
CORS(app, origin=["http://localhost:5173"])
app.config['MQTT_BROKER_URL'] = MQTT_HOST_NAME
app.config['MQTT_BROKER_PORT'] = MQTT_PORT_NUM
app.config['MQTT_REFRESH_TIME'] = 60.0  # refresh time in seconds
mqtt = Mqtt(app)
scheduler = BackgroundScheduler()
scheduler.start()

@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
    logging.info(f"Successfully connected to MQTT broker on {MQTT_HOST_NAME}:{MQTT_PORT_NUM}")

    # Setup for Last Will and Testament message for farm status
    farm_id = client._client_id
    client.will_set(f"farm/{farm_id}/status", payload="disconnected", qos=1, retain=True)

    mqtt.subscribe('test/topic/1')
    mqtt.subscribe('farm')
    mqtt.subscribe('farm/+/status')
    mqtt.subscribe('farm/+/sensorData')
    mqtt.subscribe('farm/+/systemLevels')

    commands = request.json

    for id in commands["ids"]:
        cmd = {"command": commands['command']}
        mqtt.publish(f'farm/{id}/cage', json.dumps(cmd))


@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    logging.info(f"Received message from MQTT broker on {MQTT_HOST_NAME}:{MQTT_PORT_NUM}")
    logging.info(f"Received message from MQTT broker on {MQTT_HOST_NAME}:{MQTT_PORT_NUM}")

    try:
        topic = message.topic
        payload = message.payload.decode("utf-8")
        data = json.loads(payload)
        logging.info(f"Decoded topic: {topic}")
        logging.info(f"Decoded message: {data}")

        if "sensorData" in topic:
            logging.info("Writing new sensor data")
            farm_id = topic.split("/")[1]
            logging.info(f"Farm ID: {farm_id}")
            logging.info(f"Sensor Data: {data}")

            # result = add_sensor_data(farm_id, data)
            # logging.info(f"Sensor data added to collection succesfully: {result}")

        elif "systemLevels" in topic:
            logging.info("Writing new system levels")
            farm_id = topic.split("/")[1]
            logging.info(f"Farm ID: {farm_id}")
            logging.info(f"System Levels: {data}")

            # result = add_system_levels(farm_id, data)
            # logging.info(f"System levels added to collection succesfully: {result}")

    except Exception as e:
        logging.error(f"Failed to handle message for topic {message.topic}:", e)


""" Rotues dedicated to just lift schedule collections """
@app.route("/farm/cage", methods=['PUT', 'POST'])
def farm_lift_cages():
    # TODO: Error checking
    commands = request.json
    max_height = commands.get('max_height', None)
    min_height = commands.get('min_height', None)

    for id in commands["ids"]:
        cmd = {
            "command": commands['command'],
            "max_height": max_height,
            "min_height": min_height
            }
        mqtt.publish(f'farm/{id}/cage', json.dumps(cmd))

    return f"Requested to thing"


def check_and_publish_lift():
    try:
        farms = Farm.objects.all()

        for farm in farms:
            documents = LiftActiveSchedule.objects(farm_id=farm.id)

            if documents:
                logging.info(f"Documents in active lift schedule collection for farm {farm.id}")

                for schedule in documents:
                    for lift_command in schedule.schedule:
                        dates = lift_command.dates
                        command = lift_command.command
                        duration = lift_command.duration
                        status = lift_command.status
                    
                        for date in dates:
                            current_time = get_utc_timestamp()
                            if current_time >= date and status != "completed":
                                logging.info(f"It is time to perform a lift for farm {farm.id}")
                                logging.info(f"This the lift command: {lift_command}")

                                mqtt.publish(f"farm/{farm.id}/cage", json.dumps({"command": lift_command.command}))
                                lift_command.status = "in progress"
                                lift_command.command = not lift_command.command
                                schedule.save()

                                lift_end_time = current_time + timedelta(minutes=duration)
                                logging.info(f"Lift started, it will be raised after {duration} minutes")
                                schedule_lift_back_up(farm, lift_end_time, schedule, lift_command)

    except Exception as e:
        logging.error(f"Error while checking and publishing lift: {e}")

def schedule_lift_back_up(farm, lift_end_time, schedule, lift_command):
    try:
        current_time = get_utc_timestamp()
        if current_time >= lift_end_time and lift_command.status == "in progress":
            logging.info(f"Lift duration has expired for farm {farm.id}, raising the lift back up")
            mqtt.publish(f"farm/{farm.id}/cage", json.dumps({"command": lift_command.command}))
            lift_command.status = "completed"

            archived_schedule = LiftArchiveSchedule(
                farm_id = farm.id,
                archived_schedules = [lift_command],
                archived_at = get_utc_timestamp()
            )
            archived_schedule.save()

            schedule.schedule.remove(lift_command)
            schedule.save()

            logging.info(f"Lift for farm {farm.id} completed and archived")

    except Exception as e:
        logging.error(f"Error while scheduling lift back up: {e}")
        
scheduler.add_job(
    func=check_and_publish_lift,
    trigger=IntervalTrigger(minutes=1),
    id='lift_check',
    replace_existing=True,
    misfire_grace_time=3600    # Handles missed lifts within an hour if the computer crashes
)

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=PORT_NUM)
