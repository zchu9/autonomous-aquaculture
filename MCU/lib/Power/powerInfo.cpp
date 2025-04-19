#include <Arduino.h>
#include "powerInfo.h"
#include "timer.h"

void powerInfo::formatRenogyData() {
    // TODO: unsure if this is needed.
    // this->mppt.renogyData;
};

//! WIP
// stats come in two packets: one is primarily H##, the other is mixed.
void powerInfo::formatVictronData() {
    for (size_t i = 0; i < this->bms.labels.size(); i++) {
        if (!this->bms.labels[i].compare("Checksum")) {
            continue;
        }

        if (this->bms.labels[i][0] == 'H') {
            hStatsVictron(i);
        }
        else {
            switch (this->bms.labels[i][0]) {
            case 'V':
                this->bms.mvoltage = stoi(this->bms.fields[i]); break;
            case 'I':
                this->bms.mcurrent = stoi(this->bms.fields[i]); break;
            case 'P':
                this->bms.power = stoi(this->bms.fields[i]); break;
            case 'C':
                this->bms.consumedmAH = stoi(this->bms.fields[i]); break;
            case 'S':
                this->bms.stateOfCharge = stoi(this->bms.fields[i]); break;
            case 'T':
                this->bms.timeToGo = stoi(this->bms.fields[i]); break;
            case 'B':
                this->bms.model = this->bms.fields[i]; break;
            case 'A':
                if (this->bms.labels[i][1] == 'R') {
                    this->bms.alarmReason = stoi(this->bms.fields[i]);
                }
                else { this->bms.alarm = (this->bms.fields[i][1] == 'F') ? false : true; } break;
            case 'F':
                this->bms.firmware = stoi(this->bms.fields[i]); break;
            case 'M':
                this->bms.monitorMode = stoi(this->bms.fields[i]); break;
            }
        }
    }
};

// collect power data from each source, format it as needed.
// handles all serial line switching, as needed.
int powerInfo::getData() {
    // this should not take more than 60 seconds, timeout if needed.
    time t = getTime();
    int timeoutM = (t.minutes + 1) % 60;
    int timeoutS = (t.seconds - 1) % 60;

    int successfulReads = 0;
    uint8_t ret;
    uartSwitch(BMS, VICTRON_BAUD, VICTRON_CONFIG);
    while (successfulReads > 2) {
        ret = fetchVictronStats(this->bms);
        if (!error(ret)) {
            this->formatVictronData();
            successfulReads++;
        }

        t = getTime();
        if (timeoutM == t.minutes && timeoutS == t.seconds) {
            return 1;   // timeout error
        }
    }

    uartSwitch(MPPT, RENOGY_BAUD, RENOGY_CONFIG);
    while (successfulReads > 3) {
        ret = this->mppt.rdDataRegisters();
        if (!error(ret)) {
            // ! format
            successfulReads++;
        }
        t = getTime();
        if (timeoutM == t.minutes && timeoutS == t.seconds) {
            return 1;   // timeout error
        }
    }

    while (successfulReads > 4) {
        ret = this->mppt.rdInfoRegisters();
        if (!error(ret)) {
            // ! format
            successfulReads++;
        }
        t = getTime();
        if (timeoutM == t.minutes && timeoutS == t.seconds) {
            return 1;   // timeout error
        }
    }

    this->formatRenogyData();
    return 0;
}

void powerInfo::hStatsVictron(uint8_t index) {
    uint8_t value = stoi(this->bms.labels[index].substr(1));
    switch (value) {
    case(1):
        this->bms.deepestDischargeDepth = stoi(this->bms.fields[index]);
        break;
    case(2):
        this->bms.lastDischargeDepth = stoi(this->bms.fields[index]);
        break;
    case(3):
        this->bms.avgDischargeDepth = stoi(this->bms.fields[index]);
        break;
    case(4):
        this->bms.chargeCycles = stoi(this->bms.fields[index]);
        break;
    case(5):
        this->bms.fullDischarges = stoi(this->bms.fields[index]);
        break;
    case(6):
        this->bms.totalAmpHoursDrawn = stoi(this->bms.fields[index]);
        break;
    case(7):
        this->bms.minMainBattVoltage = stoi(this->bms.fields[index]);
        break;
    case(8):
        this->bms.maxMainBattVoltage = stoi(this->bms.fields[index]);
        break;
    case(9):
        this->bms.secondsSinceLastFullCharge = stoi(this->bms.fields[index]);
        break;
    case(10):
        this->bms.numSynchros = stoi(this->bms.fields[index]);
        break;
    case(11):
        this->bms.numLowVoltAlarms = stoi(this->bms.fields[index]);

        break;
    case(12):
        this->bms.numHighVoltAlarms = stoi(this->bms.fields[index]);

        break;
    case(15):
        this->bms.auxBattMinimum = stoi(this->bms.fields[index]);

        break;
    case(16):
        this->bms.auxBattMaximum = stoi(this->bms.fields[index]);

        break;
    case(17):
        // Total Discharged Energy // DC MONITOR MODE
        break;
    case(18):
        // Total Charged Energy // DC MONITOR MODE
        break;
    }
}

bool powerInfo::error(uint8_t ret) {
    char buf[60];
    if (!ret) {
        return true;
    }
    else {
        switch (ret) {
        case 1:
            sprintf(buf, "ret %2d: Timeout reading power data.", ret);
            break;
        case 2:
            sprintf(buf, "ret %2d: Misread Victron checksum.", ret);
            break;
        }
    }
    return false;
}