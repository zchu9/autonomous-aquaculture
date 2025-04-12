from controllers import logging, jsonify, request, Farm, convert_to_eastern_time

def add_farm():
    farm_data = request.json

    if "location" not in farm_data:
        return "Error, location is required", 400

    if "farm_name" not in farm_data:
        return "Eroor, farm name is required", 400

    try:
        new_farm = Farm(
            farm_name=farm_data["farm_name"],
            location=farm_data["location"],
            )
        logging.info(f"Farm data before saving to DB: {new_farm.to_json()}")
        new_farm.save()

        farm_response = new_farm.to_mongo()
        farm_response["_id"] = str(new_farm.id)
        logging.info(f"This is the passed back time: {new_farm.created_at}")

        eastern_time = convert_to_eastern_time(new_farm.created_at)
        if eastern_time:
            farm_response["created_at"] = eastern_time.strftime("%Y-%m-%d %H:%M:%S EST")
        logging.info(f"farm response is {farm_response}")

        return jsonify(farm_response), 201
    
    except Exception as e:
        logging.error("Failed to add farm: %s", e)
        return "Error adding farm", 500

def delete_farm(id):
    try:
        result = Farm.objects(id=id).delete()

        if result > 0:
            return f"Farm {id} deleted successfully", 200
        else:
            return f"Farm {id} is not in database", 404

    except Exception as e:
        logging.error("Failed to delete farm: %s", e)
        return "Error deleting farm", 500

def update_farm(id):
    updated_data = request.json

    try:
        result = Farm.objects(id=id).update(**updated_data)

        if result > 0:
            return f"Farm {id} updated successfully", 200
        else:
            return f"Farm {id} is not in database", 404

    except Exception as e:
        logging.error("Failed to update farm: %s", e)
        return "Error updating farm", 500

   
def get_farm(id):
    try:
        farm = Farm.objects(id=id).first()

        if farm:
            farm_data = farm.to_mongo()
            farm_data["_id"] = str(farm.id)
            eastern_time = convert_to_eastern_time(farm.created_at)
            if eastern_time:
                farm_data["created_at"] = eastern_time.strftime('%Y-%m-%d %H:%M:%S EST')
            return farm_data, 200
        else:
            return f"Farm {id} is not in database", 404

    except Exception as e:
        logging.error("Failed to get farm: %s", e)
        return "Error getting farm", 500


def get_multiple_farms():
    try:
        farms = Farm.objects()
        farms_data = [farm.to_mongo() for farm in farms]

        for farm_data in farms_data:
            farm_data["_id"] = str(farm_data["_id"])
            eastern_time = convert_to_eastern_time(farm_data["created_at"])
            if eastern_time:
                farm_data["created_at"] = eastern_time.strftime('%Y-%m-%d %H:%M:%S EST')

        return farms_data, 200

    except Exception as e:
        logging.error("Failed to get farms: %s", e)
        return "Error getting farms", 500


def get_farm_status(id):
    try:
        farm = Farm.objects(id=id).first()

        if farm:
            return jsonify({"status": farm.status}), 200
        else:
            return f"Farm {id} is not in database", 404

    except Exception as e:
        logging.error("Failed to get farm status: %s", e)
        return "Error getting farm status", 500