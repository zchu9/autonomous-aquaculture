"""
author: Nathan Skirvin
created: 2/10/2025

database.py: The initailization of the database connection for the oyster application
"""
from mongoengine import connect

connect('oyster_db', host='mongo', port=27017, alias='default')

# The URI for the mongo database
MONGO_URI = 'mongodb://mongo:27017'
