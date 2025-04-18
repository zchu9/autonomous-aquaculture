from mongoengine import (
    Document, EmbeddedDocument, EmbeddedDocumentField, BooleanField,
    ObjectIdField, StringField, DateTimeField, IntField, ListField
)
from clock import get_utc_timestamp


# A single lift/lower operation
class LiftCommand(EmbeddedDocument):
    dates = ListField(DateTimeField())
    command = BooleanField()    # Up is 1, Down is 0
    duration = IntField(defualt=None)
    status = StringField(choices=["pending", "in progress", "completed", "failed"])
    lift_end_time = DateTimeField(defualt=None)


class LiftActiveSchedule(Document):
    farm_id = ObjectIdField(required=True)
    schedule = ListField(EmbeddedDocumentField(LiftCommand))
    created_at = DateTimeField(required=True, default=get_utc_timestamp)

    meta = {'db_alias': 'default'}


class LiftArchiveSchedule(Document):
    farm_id = ObjectIdField(required=True)
    archived_schedules = ListField(EmbeddedDocumentField(LiftCommand))
    archived_at = DateTimeField(required=True, default=get_utc_timestamp)

    meta = {'db_alias': 'default'}
