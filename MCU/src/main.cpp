#include "FSM.h"
#include "SAMDTimerInterrupt.h"
data d;
void testRenogyReading(data &d)
{
  d.powerData->updateData();
  d.powerData->printRenogyData();
}
void initJustPower(data &d)
{

  attachInterrupt(digitalPinToInterrupt(RX_INTERRUPT), commsHandler, RISING);
  Serial1.begin(9600, SERIAL_8N1);
  timerInit();
  initMuxPins();
  d.powerData = new powerInfo;
  d.powerData->initData();
}
void setup()
{
  // my current file structure makes my interrupts awkwardly be in main
  // to avoid a dependency loop
  //initializeStartup(d);
  initJustPower(d);
}

void loop()
{

  d.powerData->updateData();
  d.powerData->printRenogyData();
  d.powerData->printVictronData();
  delay(1000);
  // FSM(d);
}
