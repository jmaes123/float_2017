#include <SoftwareSerial.h>
#include <RoboClaw.h>
#include <PID_v1.h>

#include "bear.h"
#include "float_hardware.h"

static double m_cougarTransitRate = 0.0f;

static tCOUGAR_STATES m_cougarState = CS_OFF;
static unsigned long m_lastCougarTransitionTime = 0;
static int16_t m_cougarCurrentDutyCycle = 0;
static int m_currentCougarStep = 0;

static double m_cougarDesiredPosition = 3.8f;
static double m_cougarMeasuredPosition = 3.5f;
static double m_cougarControllerOutput = 0.0f;

static PID m_cougarPID(&m_cougarMeasuredPosition,
                       &m_cougarControllerOutput,
                       &m_cougarDesiredPosition,
                       COUGAR_Kp, COUGAR_Ki, COUGAR_Kd,
                       REVERSE, P_ON_M);

double getCougarPosition(void)
{
  return (analogRead(CONTROL_ANALOG_INPUT_COUGAR_PIN) / 1024.0f) * 5.0f;
}

int16_t computeCougarDutyCycle(double potVoltage)
{
  if(fabs(potVoltage) < 0.00001)
  {
    terminal.print("PI Command: 0 results in zero PWM\n");
    return 0;
  }
  
  terminal.print("PI Command: ");
  terminal.print(potVoltage);
  terminal.print(" --> ");
  terminal.print((int16_t)(((double)(potVoltage / COUGAR_POT_VOLTAGE_RANGE)) * 32767));
  terminal.println(" PWM.");
  
  return (int16_t)(((double)(potVoltage / COUGAR_POT_VOLTAGE_RANGE)) * 32767);
}

void initializeCougar(void)
{
  m_cougarPID.SetOutputLimits((COUGAR_POT_VOLTAGE_RANGE), -COUGAR_POT_VOLTAGE_RANGE);
  m_cougarPID.SetSampleTime(33);
  //m_cougarPID.SetMode(MANUAL);
  m_cougarPID.SetMode(AUTOMATIC);
  m_cougarMeasuredPosition = getCougarPosition();
  m_cougarDesiredPosition = m_cougarMeasuredPosition;

  m_cougarTransitRate = (COUGAR_POT_VOLTAGE_MAX - COUGAR_POT_VOLTAGE_MIN)/COUGAR_TRANSIT_TIME_MSEC;
  //moveCougarForward();

}

void moveCougarForward(void)
{
  unsigned long msTime = millis();
  unsigned long dT = msTime - m_lastCougarTransitionTime;
  m_cougarMeasuredPosition = getCougarPosition();
  double distanceRemaining = COUGAR_POT_VOLTAGE_MAX - m_cougarMeasuredPosition;

  m_cougarPID.SetMode(AUTOMATIC);

  if(m_cougarState != CS_FORWARD)
  {
    m_cougarState = CS_FORWARD;
    m_lastCougarTransitionTime = msTime;
    dT = msTime - m_lastCougarTransitionTime;
    m_cougarDesiredPosition = m_cougarMeasuredPosition;
  }

  /*
  if(dT > COUGAR_TRANSIT_STEP_PERIOD_MSEC)
  {
    
    terminal.print("Distance remaining: ");
    terminal.print(distanceRemaining);
    terminal.print(" Last TransitionTime: ");
    terminal.print(m_lastCougarTransitionTime);
    terminal.print(" dT: ");
    terminal.println(dT);
  }
  */
  
  if (fabs(distanceRemaining) > COUGAR_DEADBAND)
  {
    if(dT > COUGAR_TRANSIT_STEP_PERIOD_MSEC)
    {
      //m_cougarDesiredPosition = max(COUGAR_POT_VOLTAGE_MAX, min(5.0f, m_cougarDesiredPosition + (m_cougarTransitRate * dT)));
      m_lastCougarTransitionTime = msTime;
      m_cougarDesiredPosition = min(COUGAR_POT_VOLTAGE_MAX, m_cougarDesiredPosition + (m_cougarTransitRate * dT));
      
    }
  }
  else
  {
    terminal.println("TRANISITION TO CS_FORWARD_PAUSE!");
    stopCougar();
    m_cougarState = CS_FORWARD_PAUSE;
    m_lastCougarTransitionTime = msTime;
    
  }
}

void moveCougarBackward(void)
{
  unsigned long msTime = millis();
  unsigned long dT = msTime - m_lastCougarTransitionTime;
  m_cougarMeasuredPosition = getCougarPosition();
  double distanceRemaining = COUGAR_POT_VOLTAGE_MIN - m_cougarMeasuredPosition;

  m_cougarPID.SetMode(AUTOMATIC);

  if(m_cougarState != CS_BACKWARD)
  {
    m_cougarState = CS_BACKWARD;
    m_lastCougarTransitionTime = msTime;
    dT = msTime - m_lastCougarTransitionTime;
    m_cougarDesiredPosition = m_cougarMeasuredPosition;
  }
/*
  if(dT > COUGAR_TRANSIT_STEP_PERIOD_MSEC)
  {
    
    terminal.print("Distance remaining: ");
    terminal.print(distanceRemaining);
    terminal.print(" Last TransitionTime: ");
    terminal.print(m_lastCougarTransitionTime);
    terminal.print(" dT: ");
    terminal.println(dT);
  }
 */ 
  if (fabs(distanceRemaining) > COUGAR_DEADBAND)
  {
    if(dT > COUGAR_TRANSIT_STEP_PERIOD_MSEC)
    {
      //m_cougarDesiredPosition = min(COUGAR_POT_VOLTAGE_MIN, m_cougarDesiredPosition - (m_cougarTransitRate * dT));;
      m_lastCougarTransitionTime = msTime;
      
      m_cougarDesiredPosition = max(COUGAR_POT_VOLTAGE_MIN, min(5.0f, m_cougarDesiredPosition - (m_cougarTransitRate * dT)));
      
    }
  }
  else
  {
    terminal.println("TRANISITION TO CS_BACKWARD_PAUSE!");
    stopCougar();
    m_cougarState = CS_BACKWARD_PAUSE;
    m_lastCougarTransitionTime = msTime;
    
  }
}

void actuateCougar(void)
{
  //int16_t newDutyCycle = (int16_t)(m_cougarControllerOutput * 1000);
  int16_t newDutyCycle = m_cougarCurrentDutyCycle;

  if (fabs(m_cougarControllerOutput) > (COUGAR_DEADBAND))
  {
    terminal.print("Calculating new duty cycle\n");
    newDutyCycle = computeCougarDutyCycle(m_cougarControllerOutput);
  }

  //if (m_cougarCurrentDutyCycle != newDutyCycle)
  {
    m_cougarCurrentDutyCycle = newDutyCycle;
    roboclaw2.DutyM1(RCAddress, m_cougarCurrentDutyCycle);
    terminal.print("Cougar State: ");
    terminal.print(m_cougarState);
    terminal.print(", Cougar Duty Cycle: ");
    terminal.print(newDutyCycle);
    terminal.print(" Calculated from: ");
    terminal.println((double)m_cougarControllerOutput / COUGAR_POT_VOLTAGE_RANGE);
  }
}

void stepCougar(void)
{
  static unsigned long lastLoopTime = millis();
  unsigned long msTime = millis();

  int16_t dT = msTime - lastLoopTime;
  lastLoopTime = msTime;
  unsigned long elapsedTime = msTime - m_lastCougarTransitionTime;

  switch(m_cougarState)
  {
    case CS_FORWARD:
      terminal.println("IN_CS_FORWARD");
      
      moveCougarForward();
      
      break;

    case CS_FORWARD_PAUSE:
      terminal.println("IN CSFORWARD_PAUSE");
      if (msTime - m_lastCougarTransitionTime > COUGAR_OFF_TIME_FWD)
      {
        moveCougarBackward();
      }
      break;
      
    case CS_BACKWARD:
      terminal.println("InCSBackward");
      moveCougarBackward();
      break;

    case CS_BACKWARD_PAUSE:
      terminal.println("InBackwardsPause");
      if (msTime - m_lastCougarTransitionTime > COUGAR_OFF_TIME_BKWD)
      {
        moveCougarForward();
      }
      break;

    case CS_OFF:
      break;

    case CS_READY:
       moveCougarForward();
       break;
      
    default:
      break;
  }

  if (m_cougarState == CS_OFF || m_cougarState == CS_ERROR)
  {

  }
  else
  {
    m_cougarMeasuredPosition = getCougarPosition();

    if (m_cougarPID.Compute())
    {
      actuateCougar();
    }
  }

  terminal.print("MP: ");
  terminal.print(m_cougarMeasuredPosition);
  terminal.print(" SP: ");
  terminal.print(m_cougarDesiredPosition);
  terminal.print(" CMD: ");
  terminal.println(m_cougarControllerOutput);
}

void stopCougar(void)
{
  m_cougarPID.SetMode(MANUAL);
  roboclaw2.DutyM1(RCAddress, 0);
  m_cougarControllerOutput = 0;
}

void turnCougarOff(void)
{
  stopCougar();

  terminal.println("Turning cougar off");
  m_cougarState = CS_OFF;
}

void turnCougarOn(void)
{
  terminal.print("Trying to turn cougar on from state ");
  terminal.println(m_cougarState);

  if (m_cougarState == CS_OFF || m_cougarState == CS_ERROR)
  {
    terminal.println("CougarOn!");
    m_cougarState = CS_READY;
  }
}

