#include "victron.h"

void fetchVictronStats(ShuntPowerData& stats) {
    const size_t buffer_size = 256; // SAMD RX buffer is 256 total.
    uint8_t buffer[buffer_size] = { '\0' };   // checksum byte is not guaranteed to be ascii

    if (Serial1.available() > 5) {
        Serial1.readBytes(buffer, buffer_size);
        if (!victronChecksum(buffer, buffer_size)) {
            // TODO
            // transmission error.
        }
    }
    victronParse(stats, buffer, buffer_size);
}
