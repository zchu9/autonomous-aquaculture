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
  /*
  // Test the temperature sensor FSM function:
  temperatureFSM(d);  // Reads sensor, prints to Serial, and adds value to d.temp
  
  Serial.print("Number of temperature readings stored: ");
  Serial.println(d.temp.size());
  //d.temp.clear(); to erase data

  delay(5000); 
  */


  FSM(d);
//   d.winch->lift(3.5);
//   while(1);
}