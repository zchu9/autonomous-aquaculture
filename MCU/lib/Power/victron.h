// 2.13.2025
// header for the BMV71X SmartShunt data
// VE.Direct protocol spec 3.33

// TODO:
// denote cases where value returned is '---'.
// include a timeout.

#include <map>
#include "victronFunctions.h"

void fetchVictronStats(std::map<std::string, std::string>& stats) {
    const size_t buffer_size = 128; // SAMD RX buffer is 256 total.
    uint8_t buffer[buffer_size] = { '\0' };   // checksum byte is not guaranteed to be ascii

    delay(VICTRON_DELAY);   // either wait or return if UART was not configured. return for non-blocking, call function again after sec to retrieve data.
    if (Serial1.available() > 0) {
        Serial1.readBytes(buffer, buffer_size);
        if (!victronChecksum(buffer, buffer_size)) {
            // TODO
            // transmission error.
        }
    }
    victronParse(stats, buffer, buffer_size);
}
