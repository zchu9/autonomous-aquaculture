"""
author: Jayson De La Vega & Nathan Skirvin
created: 29/1/25

backend.py: The backend service for the oyster application
"""

import logging
from bson import ObjectId
from flask import Flask, request, jsonify
from flask_mqtt import Mqtt
from database import farm_collection, sensor_active_data_collection, sensor_archive_data_collection, system_active_levels_collection, system_archive_levels_collection, lift_active_schedule_collection, lift_archive_schedule_collection
from clock import get_eastern_time

logging.basicConfig(level=logging.DEBUG)

MQTT_HOST_NAME = 'mqtt'
MQTT_PORT_NUM = 1883
PORT_NUM = 3000

app = Flask(__name__)
app.config['MQTT_BROKER_URL'] = MQTT_HOST_NAME
app.config['MQTT_BROKER_PORT'] = MQTT_PORT_NUM
app.config['MQTT_REFRESH_TIME'] = 60.0  # refresh time in seconds
mqtt = Mqtt(app)

@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
    print(f"Successfully connected to MQTT broker on {MQTT_HOST_NAME}:{MQTT_PORT_NUM}")

    mqtt.subscribe('test/topic/1')
    mqtt.subscribe('farm')
    mqtt.subscribe('sensor_active_data')
    mqtt.subscribe('sensor_archieve_data')
    mqtt.subscribe('lift_active_schedule')
    mqtt.subscribe('lift_archieve_schedule')
    mqtt.subscribe('system_active_levels')
    mqtt.subscribe('system_archieve_levels')

# Recieves MQTT messages and stores them in the database
@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    print(f"Received message from MQTT broker on {MQTT_HOST_NAME}:{MQTT_PORT_NUM}")
    

@app.route("/")
def default():
    return f"HIII on port {PORT_NUM}"

@app.route("/test_pub")
def test_pub():
    mqtt.publish('test/topic', 'this is a test topic')
    return f"Published to MQTT broker"

""" Rotues dedicated to just farm collection """
@app.route("/addFarm", methods=["POST"])
def add_farm():
    farm_data = request.json
    farm_data["created_at"] = get_eastern_time()

    try:
        result = farm_collection.insert_one(farm_data)
        new_farm_id = result.inserted_id
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
        result = farm_collection.update_one({"$set": updated_data})

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
        farm_data = farm_collection.find_one({"_id": id})

        if farm_data:
            farm_data["_id"] = str(farm_data["_id"])
            return jsonify(farm_data), 200
        else:
            return f"Farm {id} is not in database", 404

    except Exception as e:
        logging.error("Failed to get farm: %s", e)
        return "Error getting farm", 500


""" Rotues dedicated to just sensor collections """
@app.route("/farm/<id>/updateActiveSensorData", methods=["POST"])
def update_active_sensor_data(id):
    # Adds the most recent sensor data to the active collection
    sensor_data = request.json
    
    try:
        # Gets existing sensor data
        latest_data = sensor_active_data_collection.find_one(sort=[("created_at", -1)])
        
        if latest_data:
            # Formats sensor data for archive collection
            archived_data = {
                "farm_id": latest_data["farm_id"],
                "temperature": latest_data["temperature"],
                "height": latest_data["height"],
                "camera": latest_data["camera"],
                "archived_at": get_eastern_time()
            }
            
            sensor_archive_data_collection.insert_one(archived_data)
            sensor_active_data_collection.delete_one({"_id": latest_data["_id"]})

        sensor_data["created_at"] = get_eastern_time()
        sensor_active_data_collection.insert_one(sensor_data)
        
        return "Updated sensor data succesfully", 200

    except Exception as e:
        logging.error("Failed to update active sensor data: %s", e)
        return "Error updating sensor data", 500

@app.route("/farm/<id>/getActiveSensorData", methods=["GET"])
def get_active_sensor_data(id):
    # Get the most recent sensor data from active sensor collection
    try:
        active_data = sensor_active_data_collection.find_one({"farm_id": ObjectId(id)})

        if active_data:
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
@app.route("/farm/<id>/updateActiveSystemLevels", methods=["POST"])
def update_active_system_levels(id):
    # Adds the most recent system levels data to the active collection
    system_data = request.json

    try:
        # Gets existing system levels data from active collection
        latest_data = system_active_levels_collection.find_one(sort=[("created_at", -1)])
        
        if latest_data:
            # Formats system level information for archive collection
            archived_data = {
                "farm_id": latest_data["farm_id"],
                "battery_life": latest_data["battery_life"],
                "archived_at": get_eastern_time()
            }
            
            # Move the data to archive collection and remove from active collection
            system_archive_levels_collection.insert_one(archived_data)
            system_active_levels_collection.delete_one({"_id": latest_data["_id"]})

        system_data["created_at"] = get_eastern_time()
        system_active_levels_collection.insert_one(system_data)
        
        return "Updated system levels data successfully", 200

    except Exception as e:
        logging.error("Failed to update system levels: %s", e)
        return "Error updating system levels", 500


@app.route("/farm/<id>/getActiveSystemLevels", methods=["GET"])
def get_active_system_levels(id):
    # Get the most recent system level information from active system level collection
    try:
        active_data = system_active_levels_collection.find_one({"farm_id": ObjectId(id)})

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
@app.route("/farm/<id>/liftCages")
def farm_lift_cages(id):
    mqtt.publish(f'farm/{id}/liftCages', 'lift')
    return f"Requested to lift farm {id} cages"

@app.route("/farm/<id>/lowerCages")
def farm_lower_cages(id):
    mqtt.publish(f'farm/{id}/lowerCages', 'lower')
    return f"Requested to lower farm {id} cages"

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=PORT_NUM)