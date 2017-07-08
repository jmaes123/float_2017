#include <SoftwareSerial.h>
#include <RoboClaw.h>

#include "moose.h"
#include "float_hardware.h"

static tMOOSE_STATES m_mooseState = MS_OFF;
static unsigned long m_lastMooseTransitionTime = 0;
static int16_t m_mooseCurrentDutyCycle = 0;
static int m_currentMooseStep = 0;

void moveMooseForward(void)
{
  m_currentMooseStep = 0;
  m_mooseCurrentDutyCycle = MOOSE_DUTY_CYCLE/15;
  roboclaw.DutyM2(RCAddress, m_mooseCurrentDutyCycle);
  m_lastMooseTransitionTime = millis();
  m_mooseState = MS_FORWARD;
  terminal.println("Moving moose forward");
}

void moveMooseBackward(void)
{
  m_currentMooseStep = 0;
  m_mooseCurrentDutyCycle = -(MOOSE_DUTY_CYCLE/15);
  roboclaw.DutyM2(RCAddress, m_mooseCurrentDutyCycle);
  m_lastMooseTransitionTime = millis();
  m_mooseState = MS_BACKWARD;
  terminal.println("Moving moose backward");
}

void stepMoose(void)
{
  unsigned long msTime = millis();
  unsigned long elapsedTime = msTime - m_lastMooseTransitionTime;
  
  switch(m_mooseState)
  {
    case MS_OFF:
      stopMoose();
      break;

    case MS_READY:
      moveMooseForward();
      break;

    case MS_FORWARD:
      if (elapsedTime <= 1489)
      {
        int tmpStep = elapsedTime / 100;

        if (tmpStep > m_currentMooseStep)
        {
          m_currentMooseStep = tmpStep;
          m_mooseCurrentDutyCycle = m_mooseCurrentDutyCycle + (MOOSE_DUTY_CYCLE/15);
          //terminal.print("Elapsed time: ");
          //terminal.print(elapsedTime);
          //terminal.print(" Step #");
          //terminal.print(tmpStep);
          //terminal.print(" Duty Cycle: ");
          //terminal.println(m_mooseCurrentDutyCycle);
          roboclaw.DutyM2(RCAddress, m_mooseCurrentDutyCycle);
        }
        
      }
      
      if (msTime - m_lastMooseTransitionTime > MOOSE_ON_TIME)
      {
        stopMoose();
        m_lastMooseTransitionTime = msTime;
        m_mooseState = MS_FORWARD_PAUSE;
      }
      break;

    case MS_FORWARD_PAUSE:
      if (msTime - m_lastMooseTransitionTime > MOOSE_OFF_TIME_FWD)
      {
        moveMooseBackward();
      }
      break;
      
    case MS_BACKWARD:
      if (elapsedTime <= 1489)
      {
        int tmpStep = elapsedTime / 100;

        if (tmpStep > m_currentMooseStep)
        {
          m_currentMooseStep = tmpStep;
          m_mooseCurrentDutyCycle = m_mooseCurrentDutyCycle + (-MOOSE_DUTY_CYCLE/15);
          //terminal.print("Step #");
          //terminal.print(m_currentMooseStep);
          //terminal.print(" Duty Cycle: ");
          //terminal.println(m_mooseCurrentDutyCycle);
          roboclaw.DutyM2(RCAddress, m_mooseCurrentDutyCycle);
        }
        
      }
      if (msTime - m_lastMooseTransitionTime > MOOSE_ON_TIME)
      {
        stopMoose();
        m_lastMooseTransitionTime = msTime;
        m_mooseState = MS_BACKWARD_PAUSE;
      }
      break;
      
    case MS_BACKWARD_PAUSE:
      if (msTime - m_lastMooseTransitionTime > MOOSE_OFF_TIME_FWD)
      {
        moveMooseForward();
      }
      break;
      
    case MS_ERROR:
      stopMoose();
      break;
  }
}

void stopMoose(void)
{
  roboclaw.DutyM2(RCAddress, 0);
}

void turnMooseOff(void)
{
  stopMoose();

  terminal.println("Turning moose off");
  m_mooseState = MS_OFF;
}

void turnMooseOn(void)
{
  terminal.print("Trying to turn moose on from state ");
  terminal.println(m_mooseState);
  
  if(m_mooseState == MS_OFF || m_mooseState == MS_ERROR)
  {
    terminal.println("MooseOn!");
    m_mooseState = MS_READY;
  }
}

