// 2.13.2025
// header for the BMV71X SmartShunt data
// VE.Direct protocol spec 3.33

// TODO:
// denote cases where value returned is '---'.

#include <Arduino.h>
#include "victronFunctions.h"

void fetchVictronStats(ShuntPowerData& stats);