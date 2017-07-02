#include <SoftwareSerial.h>
#include <RoboClaw.h>
#include "bear.h"
#include "float_hardware.h"

//Robo Claw Velocity PID Coefficients
#define RC_Kp 1.0
#define RC_Ki 0.0
#define RC_Kd 0.0
#define RC_qpps 10700  //This is probably wrong.

//Set up a terminal for debug output.  Use hardware serial pins 0 and 1
SoftwareSerial terminal(0,1);

//Setup communcaitions with roboclaw. Use pins 10 and 11 with 10ms timeout
SoftwareSerial serial(10,11);  
RoboClaw roboclaw(&serial,10000);


static unsigned long msTime = 0;
static bool bearOff = true;

void setup() {
  //Open terminal and roboclaw serial ports
  terminal.begin(57600);
  roboclaw.begin(57600);
   

  pinMode(BEAR_CCW_SWITCH_PIN, INPUT_PULLUP);
  pinMode(BEAR_CW_SWITCH_PIN, INPUT_PULLUP);
  pinMode(CONTROL_SWITCH_BEAR_PIN, INPUT_PULLUP);
  pinMode(CONTROL_SWITCH_MOOSE_PIN, INPUT_PULLUP);
  pinMode(CONTROL_SWITCH_COUGAR_PIN, INPUT_PULLUP);


  pinMode(CONTROL_UPDOWN_MOOSE_A_PIN, INPUT_PULLUP);
  pinMode(CONTROL_UPDOWN_MOOSE_B_PIN, INPUT_PULLUP);
  
  pinMode(13, OUTPUT);
  
  turnBearOff();
  bearOff = true;
  
  delay(2500);
}


#define MOOSE_DUTY_CYCLE (32767)


void loop() {

  if(switchOn(CONTROL_SWITCH_BEAR_PIN))
  {
    if(bearOff)
    {
      turnBearOn();
      bearOff = false;
    }
    stepBear();
  }
  else
  {
    turnBearOff();
    bearOff = true;
  }

  if(switchOn(CONTROL_SWITCH_MOOSE_PIN))
  {
    if(switchOn(CONTROL_UPDOWN_MOOSE_A_PIN))
    {
      roboclaw.DutyM2(RCAddress, MOOSE_DUTY_CYCLE);
    }
    else
    {
      if(switchOn(CONTROL_UPDOWN_MOOSE_B_PIN))
      {
        roboclaw.DutyM2(RCAddress, -(MOOSE_DUTY_CYCLE));
      }
      else
      {
        roboclaw.DutyM2(RCAddress, 0);
      }
    }
  }
  else
  {
    roboclaw.DutyM2(RCAddress, 0);
  }

}

