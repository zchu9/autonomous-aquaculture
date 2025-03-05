"""
author: Nathan Skirvin
created: 2/17/2025

test_database.py: Test initalization of collections in oyster database
"""

import pytest
from database import db

# Test if MongoDB connection is established
def test_mongo_connection():
    assert db is not None, "Database connection failed"

# Test is collections exisit in databse
def test_collections_exist():
    collections = db.list_collection_names()
    assert "farms" in collections, "farms colelction is missing"
    assert "sensor_active_data" in collections, "sensor active data colelction is missing"
    assert "sensor_archive_data" in collections, "sensor archive data colelction is missing"
    assert "system_active_levels" in collections, "system active levels colelction is missing"
    assert "system_archive_levels" in collections, "system archive levels colelction is missing"
    assert "lift_active_schedule" in collections, "lift active schedule colelction is missing"
    assert "lift_archive_schedule" in collections, "lift archive schedule colelction is missing"

if __name__ == "__main__":
    pytest.main()