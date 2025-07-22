from flask import Blueprint
from controllers.sensor_controller import (
    get_sensor_data,
    get_all_sensor_data,
    get_mult_sensor_data,
    add_sensor_data,
    delete_sensor_data,
    get_sensor_image
)

sensor_routes = Blueprint('sensor_routes', __name__)

sensor_routes.route("/farm/<id>/getSensorData", methods=['GET'])(get_sensor_data)
sensor_routes.route("/farm/<id>/getAllSensorData", methods=['GET'])(get_all_sensor_data)
sensor_routes.route("/farm/<id>/getMultSensorData", methods=['GET'])(get_mult_sensor_data)
sensor_routes.route("/farm/<id>/addSensorData", methods=['POST'])(add_sensor_data)
sensor_routes.route("/farm/<id>/deleteSensorData", methods=['DELETE'])(delete_sensor_data)
sensor_routes.route("/farm/<id>/sensorImage", methods=['GET'])(get_sensor_image)
