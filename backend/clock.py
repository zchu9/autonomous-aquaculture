import pytz
from datetime import datetime

def get_eastern_time():
    eastern_time = datetime.now(pytz.timezone('US/Eastern'))
    return eastern_time.isoformat()