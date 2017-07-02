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


bool switchOn(int switchPin) {
 /*
  static unsigned long ms = millis();
  int a,b,c;
  a = digitalRead(CONTROL_SWITCH_BEAR_PIN);
  b = digitalRead(CONTROL_SWITCH_MOOSE_PIN);
  c = digitalRead(CONTROL_SWITCH_COUGAR_PIN);

  if(millis() - ms > 500)
  {
    ms=millis();
    terminal.print("Bear: ");
    terminal.print(a);
    terminal.print(" Moose: ");
    terminal.print(b);
    terminal.print(" Cougar: ");
    terminal.println(c);
  }
*/  
  if (digitalRead(switchPin) == LOW)
  {
    return true;
  }
  else
  {
    return false;
  }
}
