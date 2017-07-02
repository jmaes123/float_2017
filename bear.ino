#include <SoftwareSerial.h>
#include <RoboClaw.h>

#include "bear.h"
#include "float_hardware.h"


extern SoftwareSerial terminal;
extern RoboClaw roboclaw;

tBEAR_STATES m_bearState = BS_OFF;

static unsigned long m_lastStopTime = 0;
static unsigned long m_lastErrorTime = 0;
static unsigned long m_lastTransitionTime = 0;

bool debugOff = false;

void initBear(void)
{
}

void stopBearNoStateChange(void)
{
  
  int16_t duty = 0;
  roboclaw.DutyM1(RCAddress, (uint16_t) duty);
}

void stopBear(void) {
  stopBearNoStateChange();

  //terminal.println("STOPPING BEAR!");
  m_lastStopTime = millis();
  setBearState(BS_STOPPED);
}

void turnBearOff(void)
{
  stopBear();
  m_bearState = BS_OFF;
}

void turnBearOn(void)
{
  stopBear();  //Set it to state stopped, so that it will run it's startup sequence on the next step()
}

void rotateBearCCW(unsigned long msTime)
{
  int16_t duty = -(BEAR_DUTY_CYCLE);
  roboclaw.DutyM1(RCAddress, (uint16_t) duty);
  setBearState(BS_ROTATING_CCW);
  terminal.println("Attempting to rotate counter-clockwise");
}

void rotateBearCW(unsigned long msTime)
{
  int16_t duty = BEAR_DUTY_CYCLE;
  roboclaw.DutyM1(RCAddress, (uint16_t) duty);
  setBearState(BS_ROTATING_CW);
  terminal.println("Attempting to rotate clockwise"); 
}

void handleBearError(unsigned long msTime)
{
    stopBear();
    setBearState(BS_ERROR);
    m_lastErrorTime = msTime;
}

void setBearState(tBEAR_STATES newState)
{
  m_bearState = newState;
  m_lastTransitionTime = millis();
  
}

bool isStateChangeOverdue(unsigned long timeNow)
{
  if (timeNow - m_lastTransitionTime > BEAR_STATE_TRANSITION_TIME_MSEC)
  {
    terminal.print("State change overdue (");
    terminal.print(timeNow - m_lastTransitionTime);
    terminal.println(")");
    return true; 
  }

  return false;
}

void stepBear() {
  unsigned long msTime = millis();
  bool overdue = false;

  debugOff = true;
  if (m_bearState != BS_ERROR && (limitReached(BEAR_CCW_SWITCH_PIN) && limitReached(BEAR_CW_SWITCH_PIN)))
  {
    handleBearError(msTime);
    terminal.println("The bear appears to be stuck!");
    digitalWrite(13, HIGH);
    debugOff = false;
    return;
  }

  if(m_bearState == BS_OFF || m_bearState == BS_ERROR)
  {
    m_lastTransitionTime = msTime; 
  }

  debugOff = false;
  overdue = isStateChangeOverdue(msTime);

  if(overdue)
  {
    stopBear();
  }
  
  switch(m_bearState)
  {
    case BS_OFF:
      stopBear();
      break;
      
    case BS_STOPPED:
      if( !limitReached(BEAR_CW_SWITCH_PIN) && !overdue)
      {
        rotateBearCW(msTime);
      }
      else
      {
        rotateBearCCW(msTime);
      }
    break;

    case BS_ROTATING_CW:
      if( limitReached(BEAR_CW_SWITCH_PIN) || overdue )
      {
        stopBear();
        setBearState(BS_PAUSED_CW);
      }
    break;
    
    case BS_PAUSED_CW:
      if(msTime - m_lastStopTime >= BEAR_PAUSE_TIME_MSEC)
      {
        if( !limitReached(BEAR_CCW_SWITCH_PIN) && !overdue )
        {
          rotateBearCCW(msTime);
        }
        else
        {
          if(overdue)
          {
            rotateBearCW(msTime);
            setBearState(BS_ROTATING_CW);
          }
        }
      }
    break;
    
    case BS_ROTATING_CCW:
      if( limitReached(BEAR_CCW_SWITCH_PIN) || overdue )
      {
        stopBear();
        setBearState(BS_PAUSED_CCW);
      }
    break;
    
    case BS_PAUSED_CCW:
      if(msTime - m_lastStopTime >= BEAR_PAUSE_TIME_MSEC)
      {
        if( !limitReached(BEAR_CW_SWITCH_PIN) )
        {
          rotateBearCW(msTime);
        }
        else
        {
          if(overdue)
          {
            rotateBearCCW(msTime);
            setBearState(BS_ROTATING_CCW);
          }
        }
      }
    break;
    
    case BS_ERROR:
      if (msTime - m_lastErrorTime > BEAR_ERROR_TIME_MSEC)
      {
        setBearState(BS_STOPPED);
      }
    break;

    default:
      handleBearError(msTime);
    break;
  }
  
}


