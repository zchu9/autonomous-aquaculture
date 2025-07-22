import pymongo
from pymongo.collection import ObjectId
import pytest
import datetime
# import db_access


@pytest.mark.parametrize(
    "input,expected",
    [("local", True), ("admin", True), ("oyster_db", True), ("config", True)],
)
def test_db_names(input, expected):
    client = pymongo.MongoClient(host="localhost", port=27017)
    db_names = client.list_database_names()
    client.close()
    assert db_names.__contains__(input) == expected


@pytest.mark.parametrize(
    "input,expected", [("sensor_data", True), ("system_levels", True)]
)
def test_collections_names(input, expected):
    client = pymongo.MongoClient(host="localhost", port=27017)
    names = client.get_database("oyster_db").list_collection_names()
    client.close()
    assert names.__contains__(input) == expected


def test_retrieval():
    client = pymongo.MongoClient(host="localhost", port=27017)
    id = ObjectId("687eb7fc08f1667d2c4ed74f")
    result: dict | None = (
        client.get_database("oyster_db")
        .get_collection("system_levels")
        .find_one({"_id": id})
    )
    client.close()

    # TODO: sample values made from test insertions. backend never ran locally on this machine, so this is limited to backend/test.py which manually inserts values.
    assert result is not None
    assert result["farm_id"] == ObjectId("6802d1c8c3e488d72cea23bb")
    assert result["smart_shunt"][0]["battery_voltage"] == 12.0
    assert result["renogy_mppt"][0]["controller_temp"] == 29.0
    assert result['created_at'] == datetime.datetime(2025, 7, 21, 21, 58, 20, 494000)
