from controllers import logging, request, jsonify, ObjectId, LiftActiveSchedule, LiftArchiveSchedule, LiftCommand, convert_to_eastern_time, convert_to_utc
from datetime import datetime


def add_active_lift_schdeule():
    active_lift = request.json

    try:
        lift_requests = []

        for farm_id in active_lift["farm_ids"]:
            schedule = active_lift.get("schedule", [])
            
            logging.info(f"Farm ids: {active_lift['farm_ids']}")
            logging.info(f"Farm's schedule: {schedule}")

            if not schedule:
                return f"No schedule provided for farm {farm_id}", 400

            for schedule_item in schedule:
                dates = [datetime.strptime(date, '%Y-%m-%d %H:%M:%S') for date in schedule_item.get("dates", [])]
                # utc_dates = [convert_to_utc(date) for date in dates] NEED TO LOOK ATTTT

                new_lift_command = [
                    LiftCommand(
                        dates = dates,
                        command = schedule[0].get("command"),
                        duration = schedule[0].get("duration"),
                        status = schedule[0].get("status")
                    )
                ] 
                logging.info(f"Lift command for farm {farm_id}: {new_lift_command}")

                new_schedule = LiftActiveSchedule(
                    farm_id = ObjectId(farm_id),
                    schedule = new_lift_command
                )
                new_schedule.save()
                logging.info(f"Active lift schedule created for farm {id}")
            
                lift_response = new_schedule.to_mongo()
                lift_response["_id"] = str(new_schedule.id)
                lift_response["farm_id"] = str(new_schedule.farm_id)
                eastern_time = convert_to_eastern_time(new_schedule.created_at)
                if eastern_time:
                    lift_response["created_at"] = eastern_time.strftime("%Y-%m-%d %H:%M:%S EST")
                
                ##### NEED TO CONVERT BACK TO EST FOR USER #####

            lift_requests.append(lift_response)
        return jsonify(lift_requests), 201
    
    except Exception as e:
        logging.error("Failed to create active lift schedule: %s", e)
        return "Error creating active lift schedule data", 500


def add_archive_lift_schdeule(id):
    archive_lift = request.json

    try:
        new_lift_command = [
            LiftCommand(
                dates=[datetime.strptime(date, '%Y-%m-%d %H:%M:%S') for date in archive_lift.get("dates", [])],
                command=archive_lift.get("command"),
                duration=archive_lift.get("duration"),
                status=archive_lift.get("status")
            )
        ] 
        logging.info(f"Lift command for farm {id}: {new_lift_command}")

        new_schedule = LiftActiveSchedule(
            farm_id = ObjectId(id),
            schedule = new_lift_command
        )
        new_schedule.save()
        logging.info(f"Archive lift schedule created for farm {id}")
        
        lift_response = new_schedule.to_mongo()
        lift_response["_id"] = str(new_schedule.id)
        lift_response["farm_id"] = str(new_schedule.farm_id)
        eastern_time = convert_to_eastern_time(new_schedule.created_at)
        if eastern_time:
            lift_response["created_at"] = eastern_time.strftime("%Y-%m-%d %H:%M:%S EST")
        
        return jsonify(lift_response), 201
    
    except Exception as e:
        logging.error("Failed to create archive lift schedule: %s", e)
        return "Error creating archive lift schedule data", 500

# Gets most recent active lift in a farm
def get_active_lift_schedule(id):
    try:
        data = LiftActiveSchedule.objects(farm_id=id).first()

        if data:
            active_data = data.to_mongo()
            active_data["_id"] = str(data.id)
            active_data["farm_id"] = str(data.id)
            eastern_time = convert_to_eastern_time(data.created_at)
            if eastern_time:
                active_data["created_at"] = eastern_time.strftime('%Y-%m-%d %H:%M:%S EST')
            
            return jsonify(active_data), 200
        else:
            return "No active lift schedule data found", 404
    except Exception as e:
        logging.error("Failed to get active lift schedule: %s", e)
        return "Error getting active lift schedule data", 500


# Get all system levels in a farm
def get_all_active_lifts(id):
    try:
        all_lifts = LiftActiveSchedule.objects(farm_id=ObjectId(id))
        lift_list = [lift.to_mongo() for lift in all_lifts]

        for lift in lift_list:
            lift["_id"] = str(lift["_id"])
            lift["farm_id"] = str(lift["farm_id"])
            eastern_time = convert_to_eastern_time(lift["created_at"])
            if eastern_time:
                lift["created_at"] = eastern_time.strftime('%Y-%m-%d %H:%M:%S EST')

        return jsonify(lift_list), 200

    except Exception as e:
        logging.error("Failed to get sensor data: %s", e)
        return "Error getting sensor data", 500


# Gets a specified date range of system levels
def get_mult_active_lifts(id):
    try:
        farm_id = ObjectId(id)
        start_date = request.args.get("start_date")
        end_date = request.args.get("end_date")

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
        
        mult_active_lifts = LiftActiveSchedule.objects(**query)

        if mult_active_lifts:
            lift_list = []
            for lift in mult_active_lifts:
                lift_data = lift.to_mongo()
                lift_data["_id"] = str(lift.id)
                lift_data["farm_id"] = str(lift.farm_id)
                
                eastern_time = convert_to_eastern_time(lift.created_at)
                if eastern_time:
                    lift_data["created_at"] = eastern_time.strftime('%Y-%m-%d %H:%M:%S EST')
                
                lift_list.append(lift_data)
                
            logging.info(f"Found all active lifts within the specfied date range")
            return jsonify(lift_list), 200
        else:
            logging.info("Can not find active lifts in specified dates")
            return "No active lifts found within the specified date range", 404
    except Exception as e:
        logging.error("Failed to get active lifts: %s", e)
        return "Error getting active lifts", 500


# Gets most recent archive lift in a farm
def get_archive_lift_schedule(id):
    try:
        archive_data = LiftArchiveSchedule.objects(farm_id=id).first()

        if archive_data:
            return jsonify(archive_data), 200
        else:
            return "No archive lift schedule data found", 404
    except Exception as e:
        logging.error("Failed to get archive lift schedule: %s", e)
        return "Error getting archive lift schedule data", 500


# Deletes all active lifts in a farm
def delete_active_lift_schedule(id):
    try:
        result = LiftActiveSchedule.objects(farm_id=id).delete()

        if result > 0:
            return f"Active lift schedule for farm {id} deleted successfully", 200
        else:
            return f"Active lift schedule for farm {id} not found", 404
    except Exception as e:
        logging.error("Failed to delete active lift schedule: %s", e)
        return "Error deleting active lift schedule", 500


# Deletes all archive lifts in a farm
def delete_archive_lift_schedule(id):
    try:
        result = LiftArchiveSchedule.objects(farm_id=id).delete()

        if result > 0:
            return f"Archived lift schedule for farm {id} deleted successfully", 200
        else:
            return f"Archived lift schedule for farm {id} not found", 404
    except Exception as e:
        logging.error("Failed to delete archived lift schedule: %s", e)
        return "Error deleting archived lift schedule", 500