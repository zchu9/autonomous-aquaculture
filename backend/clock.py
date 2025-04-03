import ntplib
import pytz
from datetime import datetime, timezone
import logging

logging.basicConfig(level=logging.DEBUG)

""" Old implementation for getting_eastern_time

def get_eastern_time():
    eastern_time = pytz.timezone('US/Eastern')
    return datetime.now(eastern_time)

    """


def get_eastern_time():
    client = ntplib.NTPClient()

    try:
        response = client.request('pool.ntp.org')
        logging.info(f"Got NTP response: {response}")

        utc_time = datetime.fromtimestamp(response.tx_time, timezone.utc)
        logging.info(f"UTC time fetched: {utc_time}")

        eastern_time = pytz.timezone('US/Eastern')
        eastern_time = utc_time.astimezone(eastern_time)
        logging.info(f"Eastern Time: {eastern_time}")

        formatted_eastern_time = eastern_time.isoformat()
        logging.info(f"ISO Eastern Time: {formatted_eastern_time}")

        return formatted_eastern_time

    except Exception as e:
        logging.error(f"Error fetching time from NTP server: {e}")
        return None
