#include <RoboClaw.h>
#include "float_hardware.h"

#include "bear.h"
extern tBEAR_STATES m_bearState;
extern bool debugOff;

bool limitReached(int switchPin) {
  if (digitalRead(switchPin) == HIGH)
  {
    if(!debugOff)
    {
      terminal.println("Bump!");
      terminal.print("BEAR STATE = ");
      terminal.print(m_bearState);
      terminal.print("Limit reached at pin ");
      terminal.println(switchPin);
    }
    return true;
  }
  else
  {
    return false;
  }
}


