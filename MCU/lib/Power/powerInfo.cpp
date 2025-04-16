#include <Arduino.h>
#include "powerInfo.h"
#include "timer.h"
#include "uartSwitch.h"

void powerInfo::formatRenogyData() {
    // TODO
    // this->mppt.renogyData;
};

// collect power data from each source, format it as needed.
// handles all serial line switching, as needed.
int powerInfo::getData() {
    // this should not take more than 60 seconds, timeout if needed.
    // int minute = getMinutes();
    // int second = getSeconds();

    bool complete = false;
    while (!complete) {

        uartSwitch(BMS, VICTRON_BAUD, VICTRON_CONFIG);
        fetchVictronStats(this->bms);
    }


    return 0;
}