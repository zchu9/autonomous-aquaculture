// 2.13.2025
// header for the BMV71X SmartShunt data
// VE.Direct protocol spec 3.33

// TODO:
// implement checksums.
// denote cases where value returned is '---'.
// include a timeout.

// messages are sent in blocks on 1 second intervals.

//  label   |   unit    |   description
// ------------------------------------------------------------------
//  V       |   mV      |   Main or channel 1 (battery) voltage
//  VS      |   mV      |   Auxiliary (starter) voltage
//  VM      |   mV      |   Mid-point voltage of the battery bank
//  DM      |   %       |   Mid-point deviation of the battery bank
//  I       |   mA      |   Main or channel 1 battery current
//  T       |   C       |   Battery temperature
//  P       |   W       |   Instantaneous power
//  CE      |   mAh     |   Consumed Amp Hours
//  SOC     |   %       |   State-of-charge
//  TTG     |   Minutes |   Time-to-go
//  Alarm   |           |   Alarm condition active
//  Relay   |           |   Relay state
//  AR      |           |   Alarm reason
//  H1      |   mAh     |   Depth of the deepest discharge
//  H2      |   mAh     |   Depth of the last discharge
//  H3      |   mAh     |   Depth of the average discharge
//  H4      |           |   Number of charge cycles
//  H5      |           |   Number of full discharges
//  H6      |   mAh     |   Cumulative Amp Hours drawn
//  H7      |   mV      |   Minimum main (battery) voltage
//  H8      |   mV      |   Maximum main (battery) voltage
//  H9      |   Seconds |   Number of seconds since last full charge
//  H10     |           |   Number of automatic synchronizations
//  H11     |           |   Number of low main voltage alarms
//  H12     |           |   Number of high main voltage alarms
//  H15     |   mV      |   Minimum auxiliary (battery) voltage
//  H16     |   mV      |   Maximum auxiliary (battery) voltage
//  H17     |   0.01kWh |   Amount of discharged energy (BMV) / Amount of produced energy (DC monitor)
//  H18     |   0.01kWh |   Amount of charged energy (BMV) / Amount of consumed energy (DC monitor)
//  BMV     |           |   Model description (deprecated)
//  FW      |           |   Firmware version (16 bit)
//  PID     |           |   Product ID
//  MON     |           |   DC monitor mode (v4.08+)

#define FIELDSTART '\r' // 0x0D, 0x0A
#define TAB '\t'        // 0x09
#define victronBaud 19200
#define victronConfig SERIAL_8N1

volatile bool powerReadInProgress;

typedef struct victronStats {
    static const size_t indices = 13;
    String labels[indices] = { "V", "P", "CE", "SOC", "TTG", "H1", "H2", "H3", "H4", "H5", "H6", "H7", "H8" };
    String data[indices];
};

void fetchVictronStats(victronStats& stats) {
    powerReadInProgress = true;
    int index = -1;
    while (Serial1.available()) {
        String label = Serial1.readStringUntil(TAB);       // take the label.
        String data = Serial1.readStringUntil(FIELDSTART); // read the next field.
        while (Serial1.peek() != '\n') {                   // newline is \r\n; this is to avoid truncated data.
            data += Serial1.readStringUntil(FIELDSTART);
        }

        if (index == -1) {
            for (int i = 0; i < stats.indices; i++) {
                if (!label.compareTo(stats.labels[i])) {   // wraps strcmp.
                    stats.data[i] = data;
                    index = i + 1;
                    break;
                }
            }
            continue;
        }

        if (!label.compareTo(stats.labels[index])) {
            stats.data[index] = data;
            index++;
        }

        if (index == stats.indices) {
            break;
        }
    }
    powerReadInProgress = false; // all done, bye bye
}