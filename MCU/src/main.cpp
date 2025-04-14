#include "FSM.h"
#include "SAMDTimerInterrupt.h"
data d;
static int oggg = 1;

void setup()
{
  // my current file structure makes my interrupts awkwardly be in main
  // to avoid a dependency loop
  initializeStartup(d);
  setupInterrupts();
}

void loop()
{
  // Test get and send image
  if (oggg == 1)
  {
    getAndSendImg(d);
    oggg = 0;
  }
  Serial.println("done");
  delay(10000);

  //  FSM(d);
}