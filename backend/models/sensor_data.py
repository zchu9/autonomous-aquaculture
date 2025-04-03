from mongoengine import (
    Document, EmbeddedDocument, EmbeddedDocumentField, ObjectIdField,
    FloatField, StringField, DateTimeField, ListField
    )
from clock import get_eastern_time


# Embedded document for listing time and value
class TimeValue(EmbeddedDocument):
    timestamp = DateTimeField(default=get_eastern_time)
    value = FloatField()


# Embedded document for camera
class CameraFrame(EmbeddedDocument):
    timestamp = DateTimeField(default=get_eastern_time)
    image = StringField()  # base64 or image path


class SensorActiveData(Document):
    farm_id = ObjectIdField(required=True)
    temperature = ListField(EmbeddedDocumentField(TimeValue))
    height = ListField(EmbeddedDocumentField(TimeValue))
    camera = ListField(EmbeddedDocumentField(CameraFrame))
    created_at = DateTimeField(default=get_eastern_time)

    meta = {'db_alias': 'default'}


# 📦 Archived sensor data
class SensorArchiveData(Document):
    farm_id = ObjectIdField(required=True)
    temperature = ListField(EmbeddedDocumentField(TimeValue))
    height = ListField(EmbeddedDocumentField(TimeValue))
    camera = ListField(EmbeddedDocumentField(CameraFrame))
    archived_at = DateTimeField(default=get_eastern_time)

    meta = {'db_alias': 'default'}
