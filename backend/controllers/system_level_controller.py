from controllers import logging, jsonify, request, ObjectId, SystemLevels, RenogyMppt, SmartShunt, convert_to_utc, convert_to_eastern_time


# Gets the most recent system level from a farm
def get_system_levels(id):
    try:
        data = SystemLevels.objects(farm_id=id).order_by("-created_at").first()

        if data:
            sys_data = data.to_mongo()
            sys_data['_id'] = str(data.id)
            sys_data['farm_id'] = str(data.farm_id)
            eastern_time = convert_to_eastern_time(data.created_at)
            if eastern_time:
                sys_data['created_at'] = eastern_time.strftime('%Y-%m-%d %H:%M:%S EST')
            return jsonify(sys_data), 200
        else:
            return "No system levels data found", 404

    except Exception as e:
        logging.error("Failed to get system levels: %s", e)
        return "Error getting system levels data", 500


# Get all system levels in a farm
def get_all_system_levels(id):
    try:
        all_sys_levs = SystemLevels.objects(farm_id=ObjectId(id))
        sys_lev_list = [sys.to_mongo() for sys in all_sys_levs]

        for system_level in sys_lev_list:
            system_level['_id'] = str(system_level['_id'])
            system_level['farm_id'] = str(system_level['farm_id'])
            eastern_time = convert_to_eastern_time(system_level['created_at'])
            if eastern_time:
                system_level['created_at'] = eastern_time.strftime('%Y-%m-%d %H:%M:%S EST')

        return jsonify(sys_lev_list), 200

    except Exception as e:
        logging.error("Failed to get sensor data: %s", e)
        return "Error getting sensor data", 500


# Gets a specified date range of system levels
def get_mult_system_levels(id):
    try:
        farm_id = ObjectId(id)
        logging.info(f"Farm ID for system levels: {farm_id}")
        start_date = request.args.get("start_date")
        end_date = request.args.get("end_date")

        query = {"farm_id": farm_id}

        if start_date and end_date:
            start_date_utc = convert_to_utc(start_date)
            end_date_utc = convert_to_utc(end_date)
            logging.info(f"UTC dates: start date = {start_date_utc} and end date = {end_date_utc}")

            if start_date_utc and end_date_utc:
                query['created_at__gte'] = start_date_utc.replace(hour=0, minute=0, second=0)
                query['created_at__lte'] = end_date_utc.replace(hour=23, minute=59, second=59)

        else:
            logging.info("User needs to add both start and end dates")
            return "You need to add both a start and end date", 400

        logging.info(f"Query for system levels: {query}")
        mult_sys_levels = SystemLevels.objects(**query)
        logging.info(f"Query successful: {mult_sys_levels}")

        if mult_sys_levels:
            sys_lev_list = []
            for system_level in mult_sys_levels:
                sys_lev_data = system_level.to_mongo()
                sys_lev_data['_id'] = str(system_level.id)
                sys_lev_data['farm_id'] = str(system_level.farm_id)

                eastern_time = convert_to_eastern_time(system_level.created_at)
                if eastern_time:
                    sys_lev_data['created_at'] = eastern_time.strftime('%Y-%m-%d %H:%M:%S EST')

                sys_lev_list.append(sys_lev_data)

            logging.info("Found all system levels within the specfied date range")
            return jsonify(sys_lev_list), 200
        else:
            logging.info("Can not find the system levels in specified dates")
            return "No system levels found within the specified date range", 404
    except Exception as e:
        logging.error("Failed to get system levels: %s", e)
        return "Error getting system levels", 500


def add_system_levels(id):
    system_level = request.json

    try:
        shunt = SmartShunt(**system_level.get("smart_shunt", {}))
        mppt = RenogyMppt(**system_level.get("renogy_mppt", {}))
        
        new_data = SystemLevels(
            farm_id=ObjectId(id),
            smart_shunt=[shunt],
            renogy_mppt=[mppt]
        )
        new_data.save()
        logging.info(f"System levels created for farm {id}: {system_level}")

        sys_response = new_data.to_mongo()
        sys_response['_id'] = str(new_data.id)
        sys_response['farm_id'] = str(new_data.farm_id)
        eastern_time = convert_to_eastern_time(new_data.created_at)
        if eastern_time:
            sys_response['created_at'] = eastern_time.strftime("%Y-%m-%d %H:%M:%S EST")

        return jsonify(sys_response), 201

    except Exception as e:
        logging.error("Failed to create sensor data: %s", e)
        return "Error creating sensor data", 500


# Deletes every system level document in specified farm
def delete_system_levels(id):
    try:
        result = SystemLevels.objects(farm_id=ObjectId(id)).delete()

        if result > 0:
            return f"System levels from farm {id} deleted successfully", 200
        else:
            return f"No system levels found for farm {id}", 404

    except Exception as e:
        logging.error("Failed to delete system levels: %s", e)
        return "Error deleting system levels", 500
