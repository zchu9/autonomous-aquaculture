#include "FSM.h"
#include "SAMDTimerInterrupt.h"
data d;

void setup()
{
  // my current file structure makes my interrupts awkwardly be in main
  // to avoid a dependency loop
  initializeStartup(d);
}
static int firstrunna = 0;
void loop()
{

  loraListen(d);

  // FSM(d);
}
