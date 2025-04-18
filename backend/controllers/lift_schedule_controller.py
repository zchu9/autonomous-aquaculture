from controllers import (
    logging, request, jsonify, ObjectId, datetime, LiftActiveSchedule,
    LiftArchiveSchedule, LiftCommand, convert_to_eastern_time, convert_to_utc
)
from pytz import utc


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
                dates_est = [datetime.strptime(date, '%Y-%m-%d %H:%M:%S') for date in schedule_item.get("dates", [])]
                logging.info(f"Dates in EST: {dates_est}")
                dates_utc = [convert_to_utc(date) for date in dates_est]
                logging.info(f"Dates in UTC: {dates_utc}")

                new_lift_command = [
                    LiftCommand(
                        dates=dates_utc,
                        command=schedule_item.get("command"),
                        duration=schedule_item.get("duration"),
                        status=schedule_item.get("status")
                    )
                ] 
                logging.info(f"Lift command for farm {farm_id}: {new_lift_command}")

                new_schedule = LiftActiveSchedule(
                    farm_id=ObjectId(farm_id),
                    schedule=new_lift_command
                )
                new_schedule.save()
                logging.info(f"Active lift schedule created for farm {farm_id}")

                lift_response = new_schedule.to_mongo()
                lift_response["_id"] = str(new_schedule.id)
                lift_response["farm_id"] = str(new_schedule.farm_id)
                eastern_time = convert_to_eastern_time(new_schedule.created_at)
                if eastern_time:
                    lift_response["created_at"] = eastern_time.strftime("%Y-%m-%d %H:%M:%S EST")

                for command in lift_response.get("schedule", []):
                    if "dates" in command:
                        converted_dates = []
                        for dt in command["dates"]:
                            eastern_date = convert_to_eastern_time(dt)
                            if eastern_date:
                                converted_dates.append(eastern_date.strftime("%Y-%m-%d %H:%M:%S EST"))
                        command["dates"] = converted_dates
                
                lift_requests.append(lift_response)

        return jsonify(lift_requests), 201

    except Exception as e:
        logging.error("Failed to create active lift schedule: %s", e)
        return "Error creating active lift schedule data", 500


def add_archive_lift_schdeule():
    archive_lift = request.json

    try:
        lift_requests = []

        for farm_id in archive_lift["farm_ids"]:
            schedule = archive_lift.get("schedule", [])

            logging.info(f"Farm ids: {archive_lift['farm_ids']}")
            logging.info(f"Farm's schedule: {schedule}")

            if not schedule:
                return f"No schedule provided for farm {farm_id}", 400

            for schedule_item in schedule:
                dates_est = [datetime.strptime(date, '%Y-%m-%d %H:%M:%S') for date in schedule_item.get("dates", [])]
                logging.info(f"Dates in EST: {dates_est}")
                dates_utc = [convert_to_utc(date) for date in dates_est]
                logging.info(f"Dates in UTC: {dates_utc}")
                lift_end_est = schedule_item.get("lift_end_time")
                lift_end_utc = convert_to_utc(lift_end_est) if lift_end_est else None

                new_lift_command = [
                    LiftCommand(
                        dates=dates_utc,
                        command=schedule_item.get("command"),
                        duration=schedule_item.get("duration"),
                        status=schedule_item.get("status"),
                        lift_end_time=lift_end_utc
                    )
                ] 
                logging.info(f"Lift command for farm {farm_id}: {new_lift_command}")

                new_schedule = LiftArchiveSchedule(
                    farm_id=ObjectId(farm_id),
                    archived_schedules=new_lift_command
                )
                new_schedule.save()
                logging.info(f"Active lift schedule created for farm {id}")

                lift_response = new_schedule.to_mongo()
                lift_response["_id"] = str(new_schedule.id)
                lift_response["farm_id"] = str(new_schedule.farm_id)
                eastern_time = convert_to_eastern_time(new_schedule.archived_at)
                if eastern_time:
                    lift_response["archived_at"] = eastern_time.strftime("%Y-%m-%d %H:%M:%S EST")

                for command in lift_response.get("archived_schedules", []):
                    if "dates" in command:
                        converted_dates = []
                        for dt in command["dates"]:
                            eastern_date = convert_to_eastern_time(dt)
                            if eastern_date:
                                converted_dates.append(eastern_date.strftime("%Y-%m-%d %H:%M:%S EST"))
                        command["dates"] = converted_dates
                
                if "lift_end_time" in command and command["lift_end_time"]:
                    command["lift_end_time"] = convert_to_eastern_time(command["lift_end_time"]).strftime("%Y-%m-%d %H:%M:%S EST")

                lift_requests.append(lift_response)
        return jsonify(lift_requests), 201

    except Exception as e:
        logging.error("Failed to create active lift schedule: %s", e)
        return "Error creating active lift schedule data", 500


# Gets most recent active lift in a farm
def get_active_lift_schedule(id):
    try:
        data = LiftActiveSchedule.objects(farm_id=id).order_by("-created_at").first()

        if data:
            active_data = data.to_mongo()
            active_data["_id"] = str(data.id)
            active_data["farm_id"] = str(data.id)
            eastern_time = convert_to_eastern_time(data.created_at)
            if eastern_time:
                active_data["created_at"] = eastern_time.strftime('%Y-%m-%d %H:%M:%S EST')

            for schedule in active_data.get("schedule", []):
                if "dates" in schedule:
                    converted_dates = []
                    for dt in schedule["dates"]:
                        eastern_date = convert_to_eastern_time(dt)
                        if eastern_date:
                            converted_dates.append(eastern_date.strftime('%Y-%m-%d %H:%M:%S EST'))
                    schedule["dates"] = converted_dates

                if "lift_end_time" in schedule:
                    eastern_end_time = convert_to_eastern_time(schedule["lift_end_time"])
                    if eastern_end_time:
                        schedule["lift_end_time"] = eastern_end_time.strftime('%Y-%m-%d %H:%M:%S EST')

            return jsonify(active_data), 200
        else:
            return "No active lift schedule data found", 404
    except Exception as e:
        logging.error("Failed to get active lift schedule: %s", e)
        return "Error getting active lift schedule data", 500


# Get all active lifts in a farm
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

            for schedule in lift.get("schedule", []):
                    if "dates" in schedule:
                        converted_dates = []
                        for dt in schedule["dates"]:
                            eastern_date = convert_to_eastern_time(dt)
                            if eastern_date:
                                converted_dates.append(eastern_date.strftime('%Y-%m-%d %H:%M:%S EST'))
                        schedule["dates"] = converted_dates

                    if "lift_end_time" in schedule:
                        eastern_end_time = convert_to_eastern_time(schedule["lift_end_time"])
                        if eastern_end_time:
                            schedule["lift_end_time"] = eastern_end_time.strftime('%Y-%m-%d %H:%M:%S EST')

        return jsonify(lift_list), 200

    except Exception as e:
        logging.error("Failed to get active lifts: %s", e)
        return "Error getting active lifts", 500


# Gets a specified date range of active lifts
def get_mult_active_lifts(id):
    try:
        farm_id = ObjectId(id)
        start_date = request.args.get("start_date")
        end_date = request.args.get("end_date")
        logging.info(f"Farm id: {farm_id}")

        if not (start_date and end_date):
            logging.error("Start date and end date are required")
            return "Start date and end date are required", 400
        
        start_date_utc = convert_to_utc(start_date)
        end_date_utc = convert_to_utc(end_date)
        logging.info(f"UTC dates: start date = {start_date_utc} and end date = {end_date_utc}")

        start_date_utc = start_date_utc.replace(hour=0, minute=0, second=0)
        end_date_utc = end_date_utc.replace(hour=23, minute=59, second=59)

        all_lifts = LiftActiveSchedule.objects(farm_id=farm_id)
        matched_lifts = []

        for lift in all_lifts:
            logging.info(f"Lift schedule: {lift.schedule}")
            match_found = False
            for command in lift.schedule:
                logging.info(f"Lift command: {command}")
                for date in command.dates:
                    logging.info(f"Lift command date: {date}")
                    if date.tzinfo is None:
                        date = utc.localize(date)
                    if start_date_utc <= date <= end_date_utc:
                        match_found = True
                        break
                if match_found:
                    break

            if match_found:
                lift_data = lift.to_mongo()
                lift_data["_id"] = str(lift.id)
                lift_data["farm_id"] = str(lift.farm_id)
                eastern_time = convert_to_eastern_time(lift.created_at)
                if eastern_time:
                    lift_data["created_at"] = eastern_time.strftime("%Y-%m-%d %H:%M:%S EST")
                logging.info(f"Lift data in EST: {lift_data["created_at"]}")

                for cmd in lift_data.get("schedule", []):
                    if "dates" in cmd:
                        converted_dates = []
                        for dt in cmd["dates"]:
                            eastern_date = convert_to_eastern_time(dt)
                            if eastern_date:
                                converted_dates.append(eastern_date.strftime("%a, %d %b %Y %H:%M:%S EST"))
                                logging.info(f"Converted date: {converted_dates}")
                        cmd["dates"] = converted_dates
                matched_lifts.append(lift_data)

        if matched_lifts:
            return jsonify(matched_lifts), 200
        else: 
            return "No active lifts found within the specified date range", 404

    except Exception as e:
        logging.error("Failed to get active lifts: %s", e)
        return "Error getting active lifts", 500


# Gets most recent archive lift in a farm
def get_archive_lift_schedule(id):
    try:
        data = LiftArchiveSchedule.objects(farm_id=id).order_by("-archived_at").first()

        if data:
            archive_data = data.to_mongo()
            archive_data["_id"] = str(data.id)
            archive_data["farm_id"] = str(data.id)
            eastern_time = convert_to_eastern_time(data.archived_at)
            if eastern_time:
                archive_data["archived_at"] = eastern_time.strftime('%Y-%m-%d %H:%M:%S EST')

            for schedule in archive_data.get("archived_schedules", []):
                if "lift_end_time" in schedule:
                    eastern_end = convert_to_eastern_time(schedule["lift_end_time"])
                    if eastern_end:
                        schedule["lift_end_time"] = eastern_end.strftime('%Y-%m-%d %H:%M:%S EST')

                if "dates" in schedule:
                    converted_dates = []
                    for dt in schedule["dates"]:
                        eastern_date = convert_to_eastern_time(dt)
                        if eastern_date:
                            converted_dates.append(eastern_date.strftime('%Y-%m-%d %H:%M:%S EST'))
                    schedule["dates"] = converted_dates

            return jsonify(archive_data), 200
        else:
            return "No archive lift schedule data found", 404
    except Exception as e:
        logging.error("Failed to get archive lift schedule: %s", e)
        return "Error getting archive lift schedule data", 500


# Get all archive lifts in a farm
def get_all_archive_lifts(id):
    try:
        all_lifts = LiftArchiveSchedule.objects(farm_id=ObjectId(id))
        lift_list = [lift.to_mongo() for lift in all_lifts]

        for lift in lift_list:
            lift["_id"] = str(lift["_id"])
            lift["farm_id"] = str(lift["farm_id"])
            eastern_time = convert_to_eastern_time(lift["archived_at"])
            if eastern_time:
                lift["archived_at"] = eastern_time.strftime('%Y-%m-%d %H:%M:%S EST')

            for schedule in lift.get("archived_schedules", []):
                if "lift_end_time" in schedule:
                    eastern_lift_end = convert_to_eastern_time(schedule["lift_end_time"])
                    if eastern_lift_end:
                        schedule["lift_end_time"] = eastern_lift_end.strftime('%Y-%m-%d %H:%M:%S EST')

                if "dates" in schedule:
                    converted_dates = []
                    for date in schedule["dates"]:
                        eastern_date = convert_to_eastern_time(date)
                        if eastern_date:
                            converted_dates.append(eastern_date.strftime('%Y-%m-%d %H:%M:%S EST'))
                    schedule["dates"] = converted_dates

        return jsonify(lift_list), 200

    except Exception as e:
        logging.error("Failed to get archive lifts: %s", e)
        return "Error getting archive lifts", 500


# Gets a specified date range of archive lifts
def get_mult_archive_lifts(id):
    try:
        farm_id = ObjectId(id)
        start_date = request.args.get("start_date")
        end_date = request.args.get("end_date")
        logging.info(f"Farm id: {farm_id}")

        if not (start_date and end_date):
            logging.error("Start date and end date are required")
            return "Start date and end date are required", 400
        
        start_date_utc = convert_to_utc(start_date)
        end_date_utc = convert_to_utc(end_date)
        logging.info(f"UTC dates: start date = {start_date_utc} and end date = {end_date_utc}")

        start_date_utc = start_date_utc.replace(hour=0, minute=0, second=0)
        end_date_utc = end_date_utc.replace(hour=23, minute=59, second=59)

        all_lifts = LiftArchiveSchedule.objects(farm_id=farm_id)
        matched_lifts = []

        for lift in all_lifts:
            logging.info(f"Lift archive schedules: {lift.archived_schedules}")
            match_found = False
            for command in lift.archived_schedules:
                logging.info(f"Lift command: {command}")
                for date in command.dates:
                    logging.info(f"Lift command date: {date}")
                    if date.tzinfo is None:
                        date = utc.localize(date)
                    if start_date_utc <= date <= end_date_utc:
                        match_found = True
                        break
                if match_found:
                    break

            if match_found:
                lift_data = lift.to_mongo()
                lift_data["_id"] = str(lift.id)
                lift_data["farm_id"] = str(lift.farm_id)
                eastern_time = convert_to_eastern_time(lift.archived_at)
                if eastern_time:
                    lift_data["archived_at"] = eastern_time.strftime("%Y-%m-%d %H:%M:%S EST")
                logging.info(f"Lift data in EST: {lift_data['archived_at']}")

                for cmd in lift_data.get("archived_schedules", []):
                    if "dates" in cmd:
                        converted_dates = []
                        for dt in cmd["dates"]:
                            eastern_date = convert_to_eastern_time(dt)
                            if eastern_date:
                                converted_dates.append(eastern_date.strftime("%a, %d %b %Y %H:%M:%S EST"))
                                logging.info(f"Converted date: {converted_dates}")
                        cmd["dates"] = converted_dates
                    
                    if "lift_end_time" in cmd:
                        eastern_end_time = convert_to_eastern_time(cmd["lift_end_time"])
                        if eastern_end_time:
                            cmd["lift_end_time"] = eastern_end_time.strftime("%Y-%m-%d %H:%M:%S EST")
                            logging.info(f"Lift end time in EST: {cmd["lift_end_time"]}")
                
                matched_lifts.append(lift_data)
        if matched_lifts:
            return jsonify(matched_lifts), 200
        else: 
            return "No archive lifts found within the specified date range", 404

    except Exception as e:
        logging.error("Failed to get archive lifts: %s", e)
        return "Error getting archive lifts", 500


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
