from flask import Blueprint
from .farm_routes import farm_routes
from .lift_schedule_routes import lift_schedule_routes
from .sensor_routes import sensor_routes
from .system_level_routes import system_level_routes

main_blueprint = Blueprint('main', __name__)

main_blueprint.register_blueprint(farm_routes)
main_blueprint.register_blueprint(lift_schedule_routes)
main_blueprint.register_blueprint(sensor_routes)
main_blueprint.register_blueprint(system_level_routes)