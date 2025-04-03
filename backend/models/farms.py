from mongoengine import Document, StringField, BooleanField, DateTimeField
from clock import get_eastern_time


class Farm(Document):
    farm_name = StringField(required=True)
    location = StringField(required=True)
    cage_position = BooleanField(default=True)
    status = BooleanField(default=False)
    created_at = DateTimeField(default=get_eastern_time)

    meta = {'db_alias': 'default'}
