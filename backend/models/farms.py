from mongoengine import Document, StringField, BooleanField, DateTimeField
from clock import get_utc_timestamp


class Farm(Document):
    farm_name = StringField(required=True)
    location = StringField(required=True)
    cage_position = BooleanField(required=True, default=True)   # 1 = up, 0 = down
    status = BooleanField(required=True, default=False)         # 1 = connected, 0 = disconnected
    created_at = DateTimeField(required=True, default=get_utc_timestamp)

    meta = {'db_alias': 'default'}
