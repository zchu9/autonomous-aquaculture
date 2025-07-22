from flask import Blueprint
from controllers.system_level_controller import (
    get_system_levels,
    get_all_system_levels,
    get_mult_system_levels,
    add_system_levels,
    delete_system_levels
)

system_level_routes = Blueprint('system_level_routes', __name__)

system_level_routes.route("/farm/<id>/getSystemLevels", methods=['GET'])(get_system_levels)
system_level_routes.route("/farm/<id>/getAllSystemLevels", methods=['GET'])(get_all_system_levels)
system_level_routes.route("/farm/<id>/getMultSystemLevels", methods=['GET'])(get_mult_system_levels)
system_level_routes.route("/farm/<id>/addSystemLevels", methods=['POST'])(add_system_levels)
system_level_routes.route("/farm/<id>/deleteSystemLevels", methods=['DELETE'])(delete_system_levels)