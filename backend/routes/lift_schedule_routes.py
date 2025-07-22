from flask import Blueprint
from controllers.lift_schedule_controller import (
    add_active_lift_schdeule,
    get_active_lift_schedule,
    get_all_active_lifts,
    get_mult_active_lifts,
    add_archive_lift_schdeule,
    get_archive_lift_schedule,
    get_all_archive_lifts,
    get_mult_archive_lifts,
    delete_active_lift_schedule,
    delete_archive_lift_schedule
)

lift_schedule_routes = Blueprint('lift_schedule_routes', __name__)

lift_schedule_routes.route("/farm/cage/addActiveSchedule", methods=['POST'])(add_active_lift_schdeule)
lift_schedule_routes.route("/farm/<id>/getActiveLiftSchedule", methods=['GET'])(get_active_lift_schedule)
lift_schedule_routes.route("/farm/<id>/getAllActiveLiftSchedule", methods=['GET'])(get_all_active_lifts)
lift_schedule_routes.route("/farm/<id>/getMultActiveLiftSchedule", methods=['GET'])(get_mult_active_lifts)
lift_schedule_routes.route("/farm/cage/addArchiveSchedule", methods=['POST'])(add_archive_lift_schdeule)
lift_schedule_routes.route("/farm/<id>/getArchiveLiftSchedule", methods=['GET'])(get_archive_lift_schedule)
lift_schedule_routes.route("/farm/<id>/getAllArchiveLiftSchedule", methods=['GET'])(get_all_archive_lifts)
lift_schedule_routes.route("/farm/<id>/getMultArchiveLiftSchedule", methods=['GET'])(get_mult_archive_lifts)
lift_schedule_routes.route("/farm/<id>/deleteActiveLiftSchedule", methods=['DELETE'])(delete_active_lift_schedule)
lift_schedule_routes.route("/farm/<id>/deleteArchiveLiftSchedule", methods=['DELETE'])(delete_archive_lift_schedule)