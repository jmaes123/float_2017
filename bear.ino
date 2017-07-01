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

void stopBear(void) {
  int16_t duty = 0;
  roboclaw.DutyM1(RCAddress, (uint16_t) duty);
  m_lastStopTime = millis();
  m_bearState = BS_STOPPED;
}

void rotateBearCCW(unsigned long msTime)
{
  int16_t duty = -(BEAR_DUTY_CYCLE);
  roboclaw.DutyM1(RCAddress, (uint16_t) duty);
  m_bearState = BS_ROTATING_CCW;
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

void stepBear() {
  unsigned long msTime = millis();

  debugOff = true;
  if (m_bearState != BS_ERROR && (limitReached(BEAR_CCW_SWITCH_PIN) && limitReached(BEAR_CW_SWITCH_PIN)))
  {
    handleBearError(msTime);
    terminal.println("The bear appears to be stuck!");
    digitalWrite(13, HIGH);
    debugOff = false;
    return;
  }

  debugOff = false;

  switch(m_bearState)
  {
    case BS_STOPPED:
      if( !limitReached(BEAR_CW_SWITCH_PIN) )
      {
        rotateBearCW(msTime);
      }
      else
      {
        rotateBearCCW(msTime);
      }
    break;

    case BS_ROTATING_CW:
      if( limitReached(BEAR_CW_SWITCH_PIN) )
      {
        stopBear();
        m_bearState = BS_PAUSED_CW;
      }
    break;
    
    case BS_PAUSED_CW:
      if(msTime - m_lastStopTime >= BEAR_PAUSE_TIME_MSEC)
      {
        if( !limitReached(BEAR_CCW_SWITCH_PIN) )
        {
          rotateBearCCW(msTime);
        }
      }
    break;
    
    case BS_ROTATING_CCW:
      if( limitReached(BEAR_CCW_SWITCH_PIN) )
      {
        stopBear();
        m_bearState = BS_PAUSED_CCW;
      }
    break;
    
    case BS_PAUSED_CCW:
      if(msTime - m_lastStopTime >= BEAR_PAUSE_TIME_MSEC)
      {
        if( !limitReached(BEAR_CW_SWITCH_PIN) )
        {
          rotateBearCW(msTime);
        }
      }
    break;
    
    case BS_ERROR:
      if (msTime - m_lastErrorTime > BEAR_ERROR_TIME_MSEC)
      {
        m_bearState = BS_STOPPED;
      }
    break;

    default:
      handleBearError(msTime);
    break;
  }
  
}


