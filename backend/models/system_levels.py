from mongoengine import (
    Document, EmbeddedDocument, EmbeddedDocumentField,
    ObjectIdField, FloatField, DateTimeField, ListField
)
from clock import get_eastern_time


# Embedded document for battery voltage readings
class VoltageReading(EmbeddedDocument):
    timestamp = DateTimeField(default=get_eastern_time)
    voltage = FloatField()


class SystemActiveLevels(Document):
    farm_id = ObjectIdField(required=True)
    battery_voltage = ListField(EmbeddedDocumentField(VoltageReading))
    created_at = DateTimeField(default=get_eastern_time)

    meta = {'db_alias': 'default'}


class SystemArchiveLevels(Document):
    farm_id = ObjectIdField(required=True)
    battery_voltage = ListField(EmbeddedDocumentField(VoltageReading))
    archived_at = DateTimeField(default=get_eastern_time)

    meta = {'db_alias': 'default'}
