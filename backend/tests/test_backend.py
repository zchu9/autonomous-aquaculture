"""
author: Nathan Skirvin
created: 2/17/2025

test_backend.py: Test MQTT pub/sub methods and routes
"""

import pytest
from bson import ObjectId
from unittest.mock import patch, MagicMock
from backend import app, mqtt, handle_mqtt_message

@pytest.fixture
def client():
    """Creates a test client for the Flask app"""
    app.testing = True
    with app.test_client() as client:
        yield client

""" Tests for Farm Collection """
# Test for adding a new farm
@patch("backend.farm_collection.insert_one")
@patch("backend.get_eastern_time")
def test_add_farm(mock_get_time, mock_insert, client):
    # Test for adding a farm succesfully in collection
    new_farm_data = {
        "location": "Cheasepeak Bay",
        "created_at": "2025-02-22T10:31:15-05:00"
    }

    mock_insert.return_value.inserted_id = str(ObjectId())
    mock_get_time.return_value = new_farm_data["created_at"]

    response = client.post("/addFarm", json=new_farm_data)
    mock_insert.assert_called_once_with(new_farm_data)
    assert response.status_code == 201
    assert '"_id"' in response.data.decode()

    # Test for an unexpected error
    mock_insert.side_effect = Exception("Database error")
    response = client.post("/addFarm", json=new_farm_data)
    assert response.status_code == 500
    assert "Error adding farm" in response.data.decode()

# Test deleting a farm
@patch("backend.farm_collection.delete_one")
def test_delete_farm(mock_delete, client):
    # Test for finding a farm in the collection
    farm_ID = str(ObjectId())
    mock_delete.return_value.deleted_count = 1
    response = client.delete(f"/farm/{farm_ID}/delete")
    mock_delete.assert_called_once_with({"_id": ObjectId(farm_ID)})
    assert f"Farm {farm_ID} deleted successfully" in response.data.decode()

    # Test for no farm in the collection
    mock_delete.return_value.deleted_count = 0
    response = client.delete(f"/farm/{farm_ID}/delete")
    mock_delete.assert_called()
    assert response.status_code == 404
    assert f"Farm {farm_ID} is not in database" in response.data.decode()

    # Test for an unexpected error
    mock_delete.side_effect = Exception("Database error")
    response = client.delete(f"/farm/{farm_ID}/delete")
    mock_delete.assert_called()
    assert response.status_code == 500
    assert "Error deleting farm" in response.data.decode()

# Test update farm information
@patch("backend.farm_collection.update_one")
def test_update_farm(mock_update, client):
    # Test for updating an existing farm in the collection
    farm_ID = str(ObjectId())
    updated_data = {
        "location": "Washington D.C."
    }

    mock_update.return_value.matched_count = 1
    response = client.put(f"/farm/{farm_ID}/update", json=updated_data)
    mock_update.assert_called_once_with({"$set": updated_data})
    assert response.status_code == 200
    assert f"Farm {farm_ID} updated successfully" in response.data.decode()

    # Test for when no existing farm is in the collection
    mock_update.return_value.matched_count = 0
    response = client.put(f"/farm/{farm_ID}/update", json=updated_data)
    assert response.status_code == 404
    assert f"Farm {farm_ID} is not in database" in response.data.decode()

    # Test for an unexpected error
    mock_update.side_effect = Exception("Database error")
    response = client.put(f"/farm/{farm_ID}/update", json=updated_data)
    assert response.status_code == 500
    assert "Error updating farm" in response.data.decode()

# Test getting farm information
@patch("backend.farm_collection.find_one")
def test_get_farm_info(mock_find, client):
    # Test for getting an existing farm in the collection
    farm_ID = str(ObjectId())
    farm_data = {
        "_id": ObjectId(farm_ID),
        "location": "Cheasepeak Bay",
        "created_at": "2025-02-22T10:31:15-05:00"
    }

    mock_find.return_value = farm_data
    response = client.get(f"/farm/{farm_ID}/info")
    assert response.status_code == 200
    assert farm_ID in response.data.decode()
    assert farm_data["location"] in response.data.decode()
    assert farm_data["created_at"] in response.data.decode()

    # Test for non-existent farm in collection
    mock_find.return_value = None
    response = client.get(f"/farm/{farm_ID}/info")
    assert response.status_code == 404
    assert f"Farm {farm_ID} is not in database" in response.data.decode()

    # Test for an unexpected error
    mock_find.side_effect = Exception("Database error")
    response = client.get(f"/farm/{farm_ID}/info")
    assert response.status_code == 500
    assert "Error getting farm" in response.data.decode()


""" Tests for Sensor Collections """
# Test updating the sensor data for the active collection and storing previous data in archive
@patch("backend.sensor_active_data_collection.insert_one")
@patch("backend.sensor_archive_data_collection.insert_one")
@patch("backend.sensor_active_data_collection.find_one")
@patch("backend.sensor_active_data_collection.delete_one")
@patch("backend.get_eastern_time")
def test_update_active_sensor_data(mock_get_time, mock_delete, mock_find, mock_archive_insert, mock_insert, client):
    # Test for updating an existing sensor 
    farm_id = ObjectId()
    sensor_id = ObjectId()
    mock_get_time.return_value = "2025-02-20T10:00:00"
    
    # Previous sensor data
    mock_find.return_value = {
        "_id": str(sensor_id),
        "farm_id": str(farm_id),
        "temperature": [{"timestamp": "2025-02-20", "value": 22.5}],
        "height": [{"timestamp": "2025-02-20", "value": 5.5}],
        "camera": [{"timestamp": "2025-02-20", "image": "base64"}],
        "created_at": "2025-02-20T10:00:00"
    }
    
    new_sensor_data = {
        "farm_id": str(farm_id),
        "temperature": [{"timestamp": "2025-02-21", "value": 23.0}],
        "height": [{"timestamp": "2025-02-21", "value": 5.8}],
        "camera": [{"timestamp": "2025-02-21", "image": "base64"}],
        "created_at": "2025-02-21T10:00:00"
    }

    archived_data = {
        "farm_id": str(farm_id),
        "temperature": [{"timestamp": "2025-02-20", "value": 22.5}],
        "height": [{"timestamp": "2025-02-20", "value": 5.5}],
        "camera": [{"timestamp": "2025-02-20", "image": "base64"}],
        "archived_at": "2025-02-20T10:00:00"
    }

    response = client.post(f"/farm/{str(farm_id)}/updateActiveSensorData", json=new_sensor_data)
    
    mock_find.assert_called_once()
    mock_archive_insert.assert_called_once_with(archived_data)
    mock_insert.assert_called_once_with(new_sensor_data)
    mock_delete.assert_called_once_with({"_id": sensor_id})
    assert response.status_code == 200
    assert "Updated sensor data successfully" in response.data.decode()

    # Test for an unexpected error
    mock_insert.side_effect = Exception("Database error")
    response = client.post(f"/farm/{str(farm_id)}/updateActiveSensorData", json=new_sensor_data)
    mock_insert.assert_called_once_with(new_sensor_data)
    assert response.status_code == 500
    assert "Error updating sensor data" in response.data.decode()

# Test getting active sensor data
@patch("backend.sensor_active_data_collection.find_one")
def test_get_active_sensor_data(mock_find, client):
    # Test for getting an existing sensor
    farm_id = ObjectId()
    sensor_id = ObjectId()

    mock_find.return_value = {
        "_id": str(sensor_id),
        "farm_id": str(farm_id),
        "temperature": [{"timestamp": "2025-02-20", "value": 22.5}],
        "height": [{"timestamp": "2025-02-20", "value": 5.5}],
        "camera": [{"timestamp": "2025-02-20", "image": "base64"}],
        "created_at": "2025-02-20T10:00:00"
    }

    response = client.get(f"/farm/{str(farm_id)}/getActiveSensorData")
    assert response.status_code == 200
    assert "temperature" in response.data.decode()
    assert "height" in response.data.decode()

    # Test for an unexpected error
    mock_find.side_effect = Exception("Database error")
    response = client.get(f"/farm/{str(farm_id)}/getActiveSensorData")
    assert response.status_code == 500
    assert "Error getting active sensor data" in response.data.decode()

# Test for getting archived sensor data
@patch("backend.sensor_archive_data_collection.find")
def test_get_archived_sensor_data(mock_find, client):
    # Test for getting existing archive sensor
    farm_id = ObjectId()
    arc_sensor_id = ObjectId()
    archived_data = [
        {
            "_id": arc_sensor_id,
            "farm_id": farm_id,
            "temperature": [{"timestamp": "2025-02-20", "value": 22.5}],
            "height": [{"timestamp": "2025-02-20", "value": 5.5}],
            "camera": [{"timestamp": "2025-02-20", "image": "base64"}],
            "archived_at": "2025-02-20T10:00:00"
        }
    ]
    
    mock_find.return_value = iter(archived_data)
    
    response = client.get(f"/farm/{str(farm_id)}/getArchivedSensorData?start_date=2025-02-20&end_date=2025-02-21")
    assert response.status_code == 200
    assert len(response.json) == 1
    assert "archived_at" in response.data.decode()

    # Test for an unexpected error
    mock_find.side_effect = Exception("Database error")
    response = client.get(f"/farm/{str(farm_id)}/getArchivedSensorData?start_date=2025-02-20&end_date=2025-02-21")
    mock_find.assert_called_once()
    assert response.status_code == 500
    assert "Error getting archived sensor data" in response.data.decode()


""" Tests for System Level Collections """
# Test updating the system level for the active collection and storing previous data in archive
@patch("backend.get_eastern_time")
@patch("backend.system_active_levels_collection.insert_one")
@patch("backend.system_archive_levels_collection.insert_one")
@patch("backend.system_active_levels_collection.find_one")
@patch("backend.system_active_levels_collection.delete_one")
def test_update_active_system_levels(mock_get_eastern_time, mock_delete, mock_find, mock_archive_insert, mock_insert, client):
    # Test for updating existing system level
    farm_id = ObjectId()
    system_id = ObjectId()
    mock_get_eastern_time.return_value = "2025-02-21T10:00:00"

    # Previous system level
    mock_find.return_value = {
        "_id": str(system_id),
        "farm_id": str(farm_id),
        "battery_life": [{"timestamp": "2025-02-20", "percentage": 85}],
        "created_at": "2025-02-20T10:00:00"
    }
    
    new_system_data = {
        "farm_id": str(farm_id),
        "battery_life": [{"timestamp": "2025-02-21", "percentage": 90}],
        "created_at": "2025-02-21T10:00:00"
    }

    archived_data = {
        "farm_id": str(farm_id),
        "battery_life": [{"timestamp": "2025-02-20", "percentage": 85}],
        "archived_at": "2025-02-21T10:00:00"
    }

    response = client.post(f"/farm/{str(farm_id)}/updateActiveSystemLevels", json=new_system_data)

    mock_find.assert_called_once()
    mock_archive_insert.assert_called_once_with(archived_data)

    mock_insert.assert_called_once_with(new_system_data)
    mock_delete.assert_called_once_with({"_id": system_id})

    assert response.status_code == 200
    assert "Updated system levels data successfully" in response.data.decode()

    # Test for an unexpected error
    mock_insert.side_effect = Exception("Database error")
    response = client.post(f"/farm/{str(farm_id)}/updateActiveSystemLevels", json=new_system_data)
    mock_insert.assert_called_once_with(new_system_data)
    assert response.status_code == 500
    assert "Error updating system levels" in response.data.decode()

# Test getting active system level data
@patch("backend.system_active_levels_collection.find_one")
def test_get_active_system_levels(mock_find, client):
    # Test for getting exisitng system level
    farm_id = ObjectId()
    system_id = ObjectId()

    mock_find.return_value = {
        "_id": system_id, 
        "farm_id": farm_id,
        "battery_life": [{"timestamp": "2025-02-20", "percentage": 85}],
        "created_at": "2025-02-20T10:00:00"
    }

    response = client.get(f"/farm/{str(farm_id)}/getActiveSystemLevels")
    mock_find.assert_called_once()

    assert response.status_code == 200
    assert "battery_life" in response.json
    assert response.json["battery_life"][0]["percentage"] == 85
    assert str(farm_id) in response.json["farm_id"]

    # Test for an unexpected error
    mock_find.side_effect = Exception("Database error")
    response = client.get(f"/farm/{str(farm_id)}/getActiveSystemLevels")
    assert response.status_code == 500
    assert "Error getting active system levels" in response.data.decode()

# Test for getting archived system level
@patch("backend.system_archive_levels_collection.find")
def test_get_archived_system_levels(mock_find, client):
    # Test for existing archive system level
    farm_id = ObjectId()
    arc_system_level_id = ObjectId()

    archived_data = [
        {
            "_id": str(arc_system_level_id),
            "farm_id": str(farm_id),
            "battery_life": [{"timestamp": "2025-02-20", "percentage": 85}],
            "archived_at": "2025-02-21T10:00:00"
        },
        {
            "farm_id": str(farm_id),
            "battery_life": [{"timestamp": "2025-02-19", "percentage": 80}],
            "archived_at": "2025-02-20T10:00:00"
        }
    ]
    
    mock_find.return_value = iter(archived_data)
    response = client.get(f"/farm/{str(farm_id)}/getArchivedSystemLevels?start_date=2025-02-19&end_date=2025-02-21")
    mock_find.assert_called_once()

    assert response.status_code == 200
    assert len(response.json) == 2

    # Test for an unexpected error
    mock_find.side_effect = Exception("Database error")
    response = client.get(f"/farm/{str(farm_id)}/getArchivedSystemLevels?start_date=2025-02-19&end_date=2025-02-21")
    assert response.status_code == 500
    assert "Error getting archived system levels" in response.data.decode()

""" Tests for Lift Schedule Collections """
# Test lift command for specific farm
@patch("backend.mqtt.publish")
def test_lift_command(mock_publish, client):
    farm_ID = str(ObjectId())
    response = client.get(f"/farm/{farm_ID}/liftCages")
    mock_publish.assert_called_once_with(f'farm/{farm_ID}/liftCages', 'lift')
    assert response.status_code == 200
    assert f"Requested to lift farm {farm_ID} cages" in response.data.decode()

# Test lower command for specific farm
@patch("backend.mqtt.publish")
def test_lower_command(mock_publish, client):
    farm_ID = str(ObjectId())
    response = client.get(f"/farm/{farm_ID}/lowerCages")
    mock_publish.assert_called_once_with(f'farm/{farm_ID}/lowerCages', 'lower')
    assert response.status_code == 200
    assert f"Requested to lower farm {farm_ID} cages" in response.data.decode()


if __name__ == "__main__":
    pytest.main()