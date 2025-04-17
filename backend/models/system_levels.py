from mongoengine import (
    Document, EmbeddedDocument, EmbeddedDocumentField, ObjectIdField,
    FloatField, StringField, BooleanField, IntField, DateTimeField, ListField
)
from clock import get_utc_timestamp

class RenogyMppt(EmbeddedDocument):
    controller_temp = FloatField()
    load_voltage = FloatField()
    load_current = FloatField()
    load_power = FloatField()
    solar_panel_voltage = FloatField()
    solar_panel_current = FloatField()
    solar_panel_power = FloatField()
    min_battery_volt_today = FloatField()
    max_battery_volt_today = FloatField()
    max_charge_current_today = FloatField()
    max_discharge_current_today = FloatField()
    max_charge_power_today = FloatField()
    max_discharge_power_today = FloatField()
    charge_amphours_today = IntField()
    discharge_amphours_today = IntField()
    charge_watthours_today = IntField()
    discharge_watthours_today = IntField()
    controller_uptime = IntField()
    total_battery_overcharges = IntField()
    total_battery_fullcharges = IntField()
    total_charge_amphours = FloatField()
    total_discharge_amphours = FloatField()
    total_power_gen = FloatField()
    total_power_consumed = FloatField()


class SmartShunt(EmbeddedDocument):
    battery_voltage = FloatField()      # Battery voltage in mV
    battery_current = FloatField()      # Battery current in mA
    battery_temp = FloatField()         # Battery temperature in Celsius
    power = FloatField()           # Power in W
    state_of_charge = FloatField()     # State of charge in percentage
    time_to_go = FloatField()       # Time to go in minutes
    alarm_status = BooleanField()
    alarm_reason = StringField()
    deepest_discharged = FloatField()   # Deepest discharged in mAh
    last_discharge = FloatField()      # Last discharge in mAh
    avg_discharge = FloatField()    # Average discharge in mAh
    charge_cycles = IntField()
    full_discharge_cycles = IntField()
    cum_amp_hrs_drawn = FloatField()    # Cumulative amp hours drawn in mAh
    min_volt_battery = FloatField()     # Minimum battery voltage in mV
    max_volt_battery = FloatField()     # Maximum battery voltage in mV
    last_full_charge = FloatField()
    auto_sync_count = IntField()
    low_volt_alarm_count = IntField()
    high_volt_alarm_count = IntField()
    energy_discharged = FloatField()    # Energy discharged in 0.01 kWh
    energy_charged = FloatField()     # Energy charged in 0.01 kWh
    model_description = StringField()
    firmware_version = StringField()
    product_id = StringField()
    dc_monitor_mode = StringField()


class SystemLevels(Document):
    farm_id = ObjectIdField(required=True)
    smart_shunt = ListField(EmbeddedDocumentField(SmartShunt))
    renogy_mppt = ListField(EmbeddedDocumentField(RenogyMppt))
    created_at = DateTimeField(required=True, default=get_utc_timestamp)

    meta = {'db_alias': 'default'}
