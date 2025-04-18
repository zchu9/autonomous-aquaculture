from controllers import logging, request, jsonify, ObjectId, SensorData, convert_to_utc, convert_to_eastern_time


# Gets the most recent sensor data in a farm
def get_sensor_data(id):
    try:
        data = SensorData.objects(farm_id=ObjectId(id)).order_by("-created_at").first()

        if data:
            sensor_data = data.to_mongo()
            sensor_data["_id"] = str(data.id)
            sensor_data["farm_id"] = str(data.farm_id)
            eastern_time = convert_to_eastern_time(data.created_at)
            if eastern_time:
                sensor_data["created_at"] = eastern_time.strftime('%Y-%m-%d %H:%M:%S EST')
            return jsonify(sensor_data), 200
        else:
            return "No sensor data found", 404
    except Exception as e:
        logging.error("Failed to get sensor data: %s", e)
        return "Error getting sensor data", 500


# Gets all sensor data in a farm
def get_all_sensor_data(id):
    try:
        all_sensor_data = SensorData.objects(farm_id=ObjectId(id))
        sensor_data_list = [sensor.to_mongo() for sensor in all_sensor_data]

        for sensor_data in sensor_data_list:
            sensor_data["_id"] = str(sensor_data["_id"])
            sensor_data["farm_id"] = str(sensor_data["farm_id"])
            eastern_time = convert_to_eastern_time(sensor_data["created_at"])
            if eastern_time:
                sensor_data["created_at"] = eastern_time.strftime('%Y-%m-%d %H:%M:%S EST')

        return jsonify(sensor_data_list), 200

    except Exception as e:
        logging.error("Failed to get sensor data: %s", e)
        return "Error getting sensor data", 500


# Gets a specified date range of sensor data
def get_mult_sensor_data(id):
    try:
        farm_id = ObjectId(id)
        start_date = request.args.get("start_date")
        end_date = request.args.get("end_date")
        logging.info(f"User defined dates: start date = {start_date} and end date = {end_date}")

        query = {"farm_id": farm_id}

        if start_date and end_date:
            start_date_utc = convert_to_utc(start_date)
            end_date_utc = convert_to_utc(end_date)
            logging.info(f"UTC dates: start date = {start_date_utc} and end date = {end_date_utc}")

            if start_date_utc and end_date_utc:
                query["created_at__gte"] = start_date_utc.replace(hour=0, minute=0, second=0)
                query["created_at__lte"] = end_date_utc.replace(hour=23, minute=59, second=59)

        else:
            logging.info("User needs to add both start and end dates")
            return "You need to add both a start and end date", 400
        
        mult_sensor_data = SensorData.objects(**query)

        if mult_sensor_data:
            sensor_data_list = [sensor.to_mongo() for sensor in mult_sensor_data]
            for sensor_data in sensor_data_list:
                sensor_data["_id"] = str(sensor_data["_id"])
                sensor_data["farm_id"] = str(sensor_data["farm_id"])
                eastern_time = convert_to_eastern_time(sensor_data["created_at"])
                if eastern_time:
                    sensor_data["created_at"] = eastern_time.strftime('%Y-%m-%d %H:%M:%S EST')

            logging.info(f"Found all sensor data within the specfied date range")
            return jsonify(sensor_data_list), 200
        else:
            logging.info("Can not find the sensor data in specified dates")
            return "No sensor data found within the specified date range", 404
    except Exception as e:
        logging.error("Failed to get sensor data: %s", e)
        return "Error getting sensor data", 500


def add_sensor_data(id):
    sensor_data = request.json

    try:
        new_data = SensorData(
            farm_id = ObjectId(id),
            temperature = sensor_data.get("temperature"),
            height = sensor_data.get("height"),
            camera = sensor_data.get("camera")
        )
        new_data.save()
        logging.info(f"Sensor data created for farm {id}: {sensor_data}")
        
        sensor_response = new_data.to_mongo()
        sensor_response["_id"] = str(new_data.id)
        sensor_response["farm_id"] = str(new_data.farm_id)
        eastern_time = convert_to_eastern_time(new_data.created_at)
        if eastern_time:
            sensor_response["created_at"] = eastern_time.strftime("%Y-%m-%d %H:%M:%S EST")
        
        return jsonify(sensor_response), 201
    
    except Exception as e:
        logging.error("Failed to create sensor data: %s", e)
        return "Error creating sensor data", 500

# Deletes every sensor data document in specified farm
def delete_sensor_data(id):
    try:
        result = SensorData.objects(farm_id=ObjectId(id)).delete()

        if result > 0:
            return f"Sensor data from farm {id} deleted successfully", 200
        else:
            return f"No sensor data found for farm {id}", 404

    except Exception as e:
        logging.error("Failed to delete sensor data: %s", e)
        return "Error deleting sensor data", 500

# Gets most recent istance of an image in a farm
def get_sensor_image(id):
    try:
        farm_id = ObjectId(id)
        logging.info(f"This is the farm ID: {farm_id}")
        active_data = SensorData.objects(farm_id=farm_id).order_by('-created_at').first()

        if active_data and active_data.camera:
            return jsonify({"camera": f"data:image/jpeg;base64,{active_data.camera}"}), 200
        else:
            return "No image found", 404

    except Exception as e:
        logging.error("Failed to get image: %s", e)
        return "Error getting image", 500