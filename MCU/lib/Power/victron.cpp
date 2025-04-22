#include "victron.h"

int fetchVictronStats(ShuntPowerData &stats)
{
    const size_t buffer_size = 256;       // SAMD RX buffer is 256 total.
    uint8_t buffer[buffer_size] = {'\0'}; // checksum byte is not guaranteed to be ascii
    int index = 0;
    bool successful = false;
    Serial.println("fetching");
    if (Serial1.available())
    {
        successful = true;
    }
    while (Serial1.available())
    {

        buffer[index] = Serial1.read();
        index++;
    }

    victronParse(stats, buffer, buffer_size);
    return successful;
}
