from mongoengine import (
    Document, EmbeddedDocument, EmbeddedDocumentField, BooleanField,
    ObjectIdField, StringField, DateTimeField, IntField, ListField
)
from clock import get_eastern_time


# A single lift/lower operation
class LiftCommand(EmbeddedDocument):
    dates = ListField(DateTimeField())
    command = BooleanField()
    duration = IntField()
    status = StringField(choices=["pending", "completed", "failed"])


class LiftActiveSchedule(Document):
    farm_id = ObjectIdField(required=True)
    schedule = ListField(EmbeddedDocumentField(LiftCommand))
    created_at = DateTimeField(default=get_eastern_time)

    meta = {'db_alias': 'default'}


class LiftArchiveSchedule(Document):
    farm_id = ObjectIdField(required=True)
    archived_schedules = ListField(EmbeddedDocumentField(LiftCommand))
    archived_at = DateTimeField(default=get_eastern_time)

    meta = {'db_alias': 'default'}
