"""
author: Jayson De La Vega & Nathan Skirvin
created: 29/1/25

backend.py: The backend service for the oyster application
"""

import logging
import json
from bson import ObjectId
from datetime import datetime
from flask import Flask, request
from flask_mqtt import Mqtt
from clock import get_utc_timestamp, convert_to_eastern_time
from datetime import timedelta
from models.lift_schedules import LiftActiveSchedule, LiftArchiveSchedule
from models.farms import Farm
from models.sensor_data import SensorData
from models.system_levels import SystemLevels, RenogyMppt, SmartShunt
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
    mqtt.subscribe('farm/+/sensorData')
    mqtt.subscribe('farm/+/systemLevels')

@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
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

            camera_data = "data:image/jpeg;base64,"
            image = data.get("camera", {})
            image = str(image)
            camera_data += image

            try:
                new_data = SensorData(
                    farm_id=ObjectId(farm_id),
                    temperature=data.get("temperature"),
                    height=data.get("height"),
                    camera=camera_data
                )
                new_data.save()
                logging.info(f"Sensor data added to collection succesfully for farm {farm_id}: {new_data}")

                Farm.objects(id=ObjectId(farm_id)).update_one(set__status=True)
                logging.info(f"Farm {farm_id} status updated to connected")
            except Exception as e:
                logging.error("Failed to create sensor data: %s", e)
                return "Error creating sensor data", 500

        elif "systemLevels" in topic:
            logging.info("Writing new system levels")
            farm_id = topic.split("/")[1]
            logging.info(f"Farm ID: {farm_id}")
            logging.info(f"System Levels: {data}")

            try:
                smart_shunt = SmartShunt(**data.get("smart_shunt", {}))
                renogy_mppt = RenogyMppt(**data.get("renogy_mppt", {}))

                new_data = SystemLevels(
                    farm_id=ObjectId(farm_id),
                    smart_shunt=[smart_shunt],
                    renogy_mppt=[renogy_mppt]
                )
                new_data.save()
                logging.info(f"System levels added to collection succesfully for farm {farm_id}: {new_data}")

                Farm.objects(id=ObjectId(farm_id)).update_one(set__status=True)
                logging.info(f"Farm {farm_id} status updated to connected")
            except Exception as e:
                logging.error("Failed to create sensor data: %s", e)
                return "Error creating sensor data", 500

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
        current_time = get_utc_timestamp()
        logging.info(f"Current time: {current_time}")
        logging.info(f"Current time in EST: {convert_to_eastern_time(current_time)}")

        for farm in farms:
            documents = LiftActiveSchedule.objects(farm_id=farm.id)

            for schedule in documents:
                logging.info(f"Schedule: {schedule.schedule}")
                for lift_command in schedule.schedule:
                    for date in lift_command.dates:
                        schedule_date = datetime.strptime(str(date), '%Y-%m-%d %H:%M:%S')
                        logging.info(f"Lift command date: {schedule_date}")
                        logging.info(f"Lift command date in EST: {convert_to_eastern_time(schedule_date)}")
                        
                        # Stage 1: Start Lift
                        if lift_command.status == "pending" and current_time.replace(tzinfo=None) >= schedule_date:
                            logging.info(f"It is time to perform a lift for farm {farm.id} and here is the command {lift_command.command}")
                            mqtt.publish(f"farm/{farm.id}/cage", json.dumps({"command": lift_command.command}))
                            
                            if lift_command.duration:
                                lift_command.status = "in progress"
                                logging.info(f"Lift command status: {lift_command.status}")
                                lift_command.lift_end_time = current_time + timedelta(minutes=lift_command.duration)
                                logging.info(f"Lift command end time: {lift_command.lift_end_time}")
                                logging.info(f"Lift command end time in EST: {convert_to_eastern_time(lift_command.lift_end_time)}")
                                lift_command.command = not lift_command.command
                                logging.info(f"Lift command: {lift_command.command}")
                                schedule.save()
                            else:
                                lift_command.status = "completed"
                                logging.info(f"Lift is completed without duration")
                                LiftArchiveSchedule(
                                    farm_id=farm.id,
                                    archived_schedules=[lift_command],
                                    archived_at=current_time
                                ).save()
                                schedule.schedule.remove(lift_command)

                                if len(schedule.schedule) == 0:
                                    schedule.delete()
                                    logging.info(f"Deleted empty LiftActiveSchedule for farm {farm.id}")
                                else:
                                    schedule.save()
                                    logging.info(f"Lift command removed from schedule for farm {farm.id}")

                        # Stage 2: Check if lift duration has expired
                        elif lift_command.status == "in progress" and hasattr(lift_command, 'lift_end_time'):
                            if current_time.replace(tzinfo=None) >= lift_command.lift_end_time.replace(tzinfo=None):
                                logging.info(f"Duration has expired")
                                logging.info(f"Current time in UTC: {current_time}")
                                logging.info(f"Lift command end time in UTC: {lift_command.lift_end_time}")
                                logging.info(f"Lift command end time in EST: {convert_to_eastern_time(lift_command.lift_end_time)}")
                                mqtt.publish(f"farm/{farm.id}/cage", json.dumps({"command": lift_command.command}))
                                lift_command.status = "completed"

                                LiftArchiveSchedule(
                                    farm_id=farm.id,
                                    archived_schedules=[lift_command],
                                    archived_at=current_time
                                ).save()

                                schedule.schedule.remove(lift_command)

                                if len(schedule.schedule) == 0:
                                    schedule.delete()
                                    logging.info(f"Deleted empty LiftActiveSchedule for farm {farm.id}")
                                else:
                                    schedule.save()
                                    logging.info(f"Lift command removed from schedule for farm {farm.id}")
    except Exception as e:
        logging.error(f"Error while checking and publishing lift: {e}")


scheduler.add_job(
    func=check_and_publish_lift,
    trigger=IntervalTrigger(minutes=1),
    id='lift_check',
    replace_existing=True,
    misfire_grace_time=3600    # Handles missed lifts within an hour if the computer crashes
)

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=PORT_NUM)
