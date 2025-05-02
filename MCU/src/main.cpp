#include "FSM.h"

data d;

void setup()
{
  initializeStartup(d);
}

void loop()
{
  FSM(d);
}
