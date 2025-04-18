import logging
import json
from flask import jsonify, request
from datetime import datetime
from models.farms import Farm
from models.lift_schedules import LiftActiveSchedule, LiftArchiveSchedule, LiftCommand
from models.sensor_data import SensorData
from models.system_levels import SystemLevels, RenogyMppt, SmartShunt
from bson import ObjectId
from clock import get_utc_timestamp, convert_to_eastern_time, convert_to_utc
