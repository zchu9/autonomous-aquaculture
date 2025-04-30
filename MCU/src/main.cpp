#include "FSM.h"

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
  initializeStartup(d);
  // initJustPower(d);
}

void loop()
{
  FSM(d);
}
