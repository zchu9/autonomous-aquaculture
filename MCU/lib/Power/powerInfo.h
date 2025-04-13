#include <renogy.h>
#include <victron.h>
#include <ArduinoJson.h>

class powerInfo {
public:
    RenogyMPPT mppt;
    ShuntPowerData bms;
    JsonDocument data;

    JsonDocument getData();
    void formatRenogyData();
};

void powerInfo::formatRenogyData() {
    // TODO
    // this->mppt.renogyData;
    
};