#include "FSM.h"
#include "SAMDTimerInterrupt.h"
data d;

void setup()
{
  // my current file structure makes my interrupts awkwardly be in main
  // to avoid a dependency loop
  initializeStartup(d);
  setupInterrupts();
}

void loop()
{
   FSM(d);
}