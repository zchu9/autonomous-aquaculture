"""
author: Nathan Skirvin
created: 2/10/2025

database.py: The initailization of the database connection for the oyster application
"""

""" Different implementation
from mongoengine import connect

connect('oyster_db', host='mongo', port=27017, alias='default')
"""

from pymongo import MongoClient

# The URI for the mongo database
MONGO_URI = 'mongodb://mongo:27017'
client = MongoClient(MONGO_URI)
db = client.oyster_db

# Collections in the database
farm_collection = db.farms
sensor_active_data_collection = db.sensor_active_data
sensor_archive_data_collection = db.sensor_archive_data
system_active_levels_collection = db.system_active_levels
system_archive_levels_collection = db.system_archive_levels
lift_active_schedule_collection = db.lift_active_schedule
lift_archive_schedule_collection = db.lift_archive_schedule

