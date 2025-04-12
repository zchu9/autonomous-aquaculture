from mongoengine import Document, ObjectIdField, FloatField, StringField, DateTimeField
from clock import get_utc_timestamp

class SensorData(Document):
    farm_id = ObjectIdField(required=True)
    temperature = FloatField()
    height = FloatField()
    camera = StringField()
    created_at = DateTimeField(required=True, default=get_utc_timestamp)

    meta = {'db_alias': 'default'}
