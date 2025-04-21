#include "FSM.h"
#include "SAMDTimerInterrupt.h"
data d;
void testRenogyReading(data &d)
{
  d.powerData->updateData();
  d.powerData->printRenogyData();
}

void setup()
{
  // my current file structure makes my interrupts awkwardly be in main
  // to avoid a dependency loop
  initializeStartup(d);
  d.powerData->initData();
}

void loop()
{

  d.powerData->updateData();
  d.powerData->printRenogyData();
  d.powerData->printVictronData();
  delay(1000);
  // FSM(d);
}
