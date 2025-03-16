"""
author: Jayson De La Vega & Nathan Skirvin
created: 29/1/25

backend.py: The backend service for the oyster application
"""

import logging
import json
import time
from bson import ObjectId
from flask import Flask, request, jsonify
from flask_mqtt import Mqtt
from database import farm_collection, sensor_active_data_collection, sensor_archive_data_collection, system_active_levels_collection, system_archive_levels_collection, lift_active_schedule_collection, lift_archive_schedule_collection
from clock import get_eastern_time
from flask_cors import CORS
import time

logging.basicConfig(level=logging.DEBUG)

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

    # Setup for Last Will and Testament message for farm status
    farm_id = client._client_id
    client.will_set(f"farm/{farm_id}/status", payload="disconnected", qos=1, retain=True)

    mqtt.subscribe('test/topic/1')
    mqtt.subscribe('farm')
    mqtt.subscribe('farm/+/status')
    mqtt.subscribe('farm/+/getActiveSensorData')
    mqtt.subscribe('farm/+/getActiveSystemLevels')


# Recieves MQTT messages and stores them in the database
@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    print(f"Received message from MQTT broker on {MQTT_HOST_NAME}:{MQTT_PORT_NUM}")

    try:
        topic = message.topic
        payload = message.payload.decode("utf-8")
        data = json.loads(payload)
        print(f"Decoded topic: {topic}")
        print(f"Decoded message: {data}")

        if "status" in topic:
            farm_id = topic.split("/")[1]
            print(f"Farm ID: {farm_id}")

            if payload == "disconnected":
            # Update farm status to disconnected in MongoDB when LWT message is received
                farm_collection.update_one(
                    {"_id": ObjectId(farm_id)},
                    {"$set": {"status": False}}
                )

            elif payload == "connected":
                # Update farm status to connected in MongoDB
                farm_collection.update_one(
                    {"_id": ObjectId(farm_id)},
                    {"$set": {"status": True}}
                )

        elif "getActiveSensorData" in topic:
            farm_id = topic.split("/")[1]
            existing_data = sensor_active_data_collection.find_one({"farm_id": farm_id})

            if existing_data:
                archive_data = {
                    "farm_id": existing_data["farm_id"],
                    "temperature": existing_data["temperature"],
                    "height": existing_data["height"],
                    "camera": existing_data["camera"],
                    "archived_at": get_eastern_time()
                }
                sensor_archive_data_collection.insert_one(archive_data)
                sensor_active_data_collection.delete_one({"_id": existing_data["_id"]})

            data["created_at"] = get_eastern_time()
            sensor_active_data_collection.insert_one(data)

            farm_collection.update_one(
                    {"_id": ObjectId(farm_id)},
                    {"$set": {"status": True}}
                )

            print("Sensor data updated succesfully")

        elif "getActiveSystemLevels" in topic:
            farm_id = topic.split("/")[1]
            existing_data = system_active_levels_collection.find_one({"farm_id": farm_id})

            if existing_data:
                archive_data = {
                    "farm_id": existing_data["farm_id"],
                    "battery_voltage": existing_data["battery_voltage"],
                    "archived_at": get_eastern_time()
                }
                system_archive_levels_collection.insert_one(archive_data)
                system_active_levels_collection.delete_one({"_id": existing_data["_id"]})

            data["created_at"] = get_eastern_time()
            system_active_levels_collection.insert_one(data)

            farm_collection.update_one(
                    {"_id": ObjectId(farm_id)},
                    {"$set": {"status": True}}
                )

            print("System level data updated successfully")

    except Exception as e:
        logging.error(f"Failed to handle message for topic {message.topic}:", e)


@app.route("/")
def default():
    return f"HIII on port {PORT_NUM}"


@app.route("/test_pub", methods=['PUT', 'POST'])
def test_pub():
    mqtt.publish('test/topic', 'this is a test topic')
    return "Published to MQTT broker"

""" Rotues dedicated to just farm collection """
@app.route("/addFarm", methods=["POST"])
def add_farm():
    farm_data = request.json

    if 'location' not in farm_data:
        return jsonify({"error": "Location is required"}), 400
    
    if 'farm_name' not in farm_data:
        return jsonify({"error": "Farm name is required"}), 400

    farm_data["cage_position"] = True
    farm_data["status"] = False
    farm_data["created_at"] = get_eastern_time()

    try:
        result = farm_collection.insert_one(farm_data)
        new_farm_id = result.inserted_id

        # Create sensor and system level objects for the new farm
        try:
            sensor_active_data_collection.insert_one({"farm_id": farm_data["_id"]})
            system_active_levels_collection.insert_one({"farm_id": farm_data["_id"]})
        except Exception as e:
            logging.error("Failed to create sensor and system level objects: %s", e)
            return "Error creating sensor and system level objects", 500

        return jsonify({"_id": str(new_farm_id)}), 201
    except Exception as e:
        logging.error("Failed to add farm: %s", e)
        return "Error adding farm", 500


@app.route("/farm/<id>/delete", methods=["DELETE"])
def delete_farm(id):
    try:
        farm_id = ObjectId(id)
        result = farm_collection.delete_one({"_id": farm_id})

        if result.deleted_count > 0:
            return f"Farm {id} deleted successfully", 200
        else:
            return f"Farm {id} is not in database", 404

    except Exception as e:
        logging.error("Failed to delete farm: %s", e)
        return "Error deleting farm", 500


@app.route("/farm/<id>/update", methods=["PUT"])
def update_farm(id):
    updated_data = request.json

    try:
        farm_id = ObjectId(id)
        result = farm_collection.update_one({"_id": farm_id}, {"$set": updated_data})

        if result.matched_count == 1:
            return f"Farm {id} updated successfully", 200
        else:
            return f"Farm {id} is not in database", 404

    except Exception as e:
        logging.error("Failed to update farm: %s", e)
        return "Error updating farm", 500


@app.route("/farm/<id>/info", methods=["GET"])
def get_farm(id):
    try:
        farm_id = ObjectId(id)
        farm_data = farm_collection.find_one({"_id": farm_id})

        if farm_data:
            farm_data["_id"] = str(farm_data["_id"])
            return jsonify(farm_data), 200
        else:
            return f"Farm {id} is not in database", 404

    except Exception as e:
        logging.error("Failed to get farm: %s", e)
        return "Error getting farm", 500


@app.route("/farm", methods=["GET"])
def get_multiple_farms():
    try:
        farms_data = []

        for farm in farm_collection.find():
            if farm:
                farm["_id"] = str(farm["_id"])
                farms_data.append(farm)

        if not farms_data:
            return "No farms found for the given IDs", 400

        return jsonify(farms_data), 200

    except Exception as e:
        logging.error("Failed to get farms: %s", e)
        return "Error getting farms", 500


@app.route("/farm/<id>/check_status", methods=["GET"])
def get_farm_status(id):
    try:
        farm_id = ObjectId(id)
        farm_data = farm_collection.find_one({"_id": farm_id})

        if farm_data:
            farm_data["_id"] = str(farm_data["_id"])
            return jsonify({"status": farm_data["status"]}), 200
        else:
            return f"Farm {id} is not in database", 404

    except Exception as e:
        logging.error("Failed to get farm status: %s", e)
        return "Error getting farm status", 500


""" Rotues dedicated to just sensor collections """
@app.route("/farm/<id>/getActiveSensorData", methods=["GET"])
def get_active_sensor_data(id):

    # Get the most recent sensor data from active sensor collection
    try:
        farm_id = ObjectId(id)
        active_data = sensor_active_data_collection.find_one({"farm_id": farm_id})

        if active_data and str(active_data["farm_id"]) == id:
            # Converts ObjectId's to string for the return statement
            active_data["_id"] = str(active_data["_id"])
            active_data["farm_id"] = str(active_data["farm_id"])
            return jsonify(active_data), 200
        else:
            return "No active sensor data found", 404
    except Exception as e:
        logging.error("Failed to get active sensor data: %s", e)
        return "Error getting active sensor data", 500


@app.route("/farm/<id>/getArchivedSensorData", methods=["GET"])
def get_archived_sensor_data(id):
    # Get archived sensor data based on user's specified date range
    try:
        start_date = request.args.get("start_date")
        end_date = request.args.get("end_date")

        if start_date and end_date:
            query = {
                "farm_id": ObjectId(id),
                "archived_at": {
                    "$gte": start_date,
                    "$lte": end_date
                }
            }
        else:
            # If no date range provided, return all archived data for this farm
            query = {"farm_id": ObjectId(id)}

        archived_data = sensor_archive_data_collection.find(query)
        archived_list = list(archived_data)

        if archived_list:
            return jsonify(archived_list), 200
        else:
            return "No archived sensor data found for the given dates", 404

    except Exception as e:
        logging.error("Failed to get archived sensor data: %s", e)
        return "Error getting archived sensor data", 500


""" Rotues dedicated to just system level collections """
@app.route("/farm/<id>/getActiveSystemLevels", methods=["GET"])
def get_active_system_levels(id):

    # Get the most recent system level information from active system level collection
    try:
        farm_id = ObjectId(id)
        active_data = system_active_levels_collection.find_one({"farm_id": farm_id})

        if active_data:
            active_data["_id"] = str(active_data["_id"])
            active_data["farm_id"] = str(active_data["farm_id"])
            return jsonify(active_data), 200
        else:
            return "No active system levels data found", 404
    except Exception as e:
        logging.error("Failed to get active system levels: %s", e)
        return "Error getting active system levels data", 500


@app.route("/farm/<id>/getArchivedSystemLevels", methods=["GET"])
def get_archived_system_levels(id):
    # Get archived system levels based on user's specified date range
    try:
        start_date = request.args.get("start_date")
        end_date = request.args.get("end_date")

        if start_date and end_date:
            query = {
                "farm_id": ObjectId(id),
                "archived_at": {
                    "$gte": start_date,
                    "$lte": end_date
                }
            }
        else:
            # If no date range provided, return all archived data for this farm
            query = {"farm_id": ObjectId(id)}

        # Fetch the data from the archive collection
        archived_data = system_archive_levels_collection.find(query)

        archived_list = list(archived_data)
        if archived_list:
            return jsonify(archived_list), 200
        else:
            return "No archived system levels data found for the given dates", 404

    except Exception as e:
        logging.error("Failed to get archived system levels: %s", e)
        return "Error getting archived system levels", 500


""" Rotues dedicated to just lift schedule collections """
@app.route("/farm/cage", methods=['PUT', 'POST'])
def farm_lift_cages():
    # TODO: Error checking
    commands = request.json

    for id in commands['ids']:
        cmd = {"command": commands['command']}
        mqtt.publish(f'farm/{id}/cage', json.dumps(cmd))

    return f"Requested to thing"


if __name__ == "__main__":
    app.run(host='0.0.0.0', port=PORT_NUM)
