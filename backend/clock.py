import ntplib
import pytz
from datetime import datetime, timezone
import logging

logging.basicConfig(level=logging.DEBUG)

def get_utc_timestamp():
    client = ntplib.NTPClient()

    try:
        response = client.request('pool.ntp.org')
        logging.info(f"Got NTP response: {response}")

        utc_time = datetime.fromtimestamp(response.tx_time, timezone.utc)
        logging.info(f"UTC time fetched: {utc_time}")
        return utc_time        

    except Exception as e:
        logging.error(f"Error fetching time from NTP server: {e}")
        return None

def convert_to_eastern_time(utc_time):
    try:
        if utc_time is None:
            logging.error("Need utc_time from database")
            return None
        
        if isinstance(utc_time, str):
            try:
                utc_time = datetime.strptime(utc_time, '%Y-%m-%d %H:%M:%S')
                logging.info(f"Parsed time from string: {utc_time}")
            except ValueError as e:
                logging.error(f"Error parsing string as datetime: {e}")
                return None
            
        if isinstance(utc_time, datetime):
            eastern_time = pytz.timezone('US/Eastern')
            eastern_time = utc_time.astimezone(eastern_time)
            logging.info(f"Eastern Time: {eastern_time}")
            return eastern_time
        else:
            logging.error(f"Expected utc_time to be a datetime object, its {utc_time}")
    
    except Exception as e:
        logging.error(f"Error converting UTC to Eastern Time: {e}")
        return None

def convert_to_utc(est_time):
    try:
        if est_time is None:
            logging.error("Need EST from API route")
            return None
        
        if isinstance(est_time, str):
            eastern_time = pytz.timezone('US/Eastern')

            try:
                est_time_obj = datetime.strptime(est_time, '%Y-%m-%d %H:%M:%S')
                logging.info(f"Parsed time with (Y-M-D H:M:S) format : {est_time_obj}")
            except ValueError:
                try:
                    est_time_obj = datetime.strptime(est_time, '%Y-%m-%d')
                    est_time_obj = est_time_obj.replace(hour = 0, minute = 0, second = 0)
                    logging.info(f"Parsed time with (Y-M-D) format: {est_time_obj}")

                except ValueError:
                    logging.error(f"Invalid date format: {est_time}")
                    return None

            est_time_obj = eastern_time.localize(est_time_obj)
            utc_time = est_time_obj.astimezone(pytz.utc)
            logging.info(f"Converted EST time to UTC: {utc_time}")
            return utc_time

        else:
            logging.error(f"Expected utc_time to be a string, got {type(utc_time)}")
            return None
    
    except Exception as e:
        logging.error(f"Error converting EST to UTC: {e}")
        return None