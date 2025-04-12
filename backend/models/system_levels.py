from mongoengine import Document, ObjectIdField, FloatField, DateTimeField

from clock import get_utc_timestamp


class SystemLevels(Document):
    farm_id = ObjectIdField(required=True)
    solar_panel_power = FloatField()
    battery_voltage = FloatField()
    battery_temp = FloatField()
    battery_time = DateTimeField()
    created_at = DateTimeField(required=True, default=get_utc_timestamp)

    meta = {'db_alias': 'default'}

