#include <Arduino.h>
#include "powerInfo.h"
#include "timer.h"

void powerInfo::formatRenogyData() {
    // TODO: unsure if this is needed.
    // this->mppt.renogyData;
};

void powerInfo::formatVictronData() {
    for (size_t i = 0; i < this->bms.labels.size(); i++) {
        if (bms.fields[i][0] == 'H') {
            // mostly extra codes
        }
        else {
            switch (bms.labels[i][0]) {
            case 'V':
                this->bms.mvoltage = stoi(bms.fields[i]); break;
            case 'I':
                this->bms.mcurrent = stoi(bms.fields[i]); break;
            case 'P':
                this->bms.power = stoi(bms.fields[i]); break;
            case 'C':
                this->bms.consumedmAH = stoi(bms.fields[i]); break;
            case 'S':
                this->bms.stateOfCharge = stoi(bms.fields[i]); break;
            case 'T':
                this->bms.timeToGo = stoi(bms.fields[i]); break;
            case 'B':
                this->bms.model = bms.fields[i]; break;
            case 'A':
                if (bms.labels[i][1] == 'R') {
                    this->bms.alarmReason = stoi(bms.fields[i]);
                } else { this->bms.alarm = (bms.fields[i][1] == 'F') ? false : true;} break;
            case 'F':
               this->bms.firmware = stoi(bms.fields[i]); break;

               // WIP
            }
        }
    }
};

// collect power data from each source, format it as needed.
// handles all serial line switching, as needed.
int powerInfo::getData() {
    // this should not take more than 60 seconds, timeout if needed.
    // int minute = getMinutes();
    // int second = getSeconds();

    uartSwitch(BMS, VICTRON_BAUD, VICTRON_CONFIG);
    fetchVictronStats(this->bms);

    uartSwitch(MPPT, RENOGY_BAUD, RENOGY_CONFIG);
    this->mppt.renogy_read_data_registers();
    this->mppt.renogy_read_info_registers();

    return 0;
}