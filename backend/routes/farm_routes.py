from flask import Blueprint
from controllers.farm_controller import (
    add_farm,
    delete_farm,
    update_farm,
    get_farm,
    get_multiple_farms,
    get_farm_status
)

farm_routes = Blueprint('farm_routes', __name__)

farm_routes.route("/addFarm", methods=["POST"])(add_farm)
farm_routes.route("/farm/<id>/delete", methods=["DELETE"])(delete_farm)
farm_routes.route("/farm/<id>/update", methods=["PUT"])(update_farm)
farm_routes.route("/farm/<id>/info", methods=["GET"])(get_farm)
farm_routes.route("/farm", methods=["GET"])(get_multiple_farms)
farm_routes.route("/farm/<id>/check_status", methods=["GET"])(get_farm_status)