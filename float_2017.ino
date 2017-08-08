#include <PID_v1.h>
#include <SoftwareSerial.h>
#include <RoboClaw.h>
#include "float_hardware.h"
#include "bear.h"
#include "moose.h"
#include "cougar.h"

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
RoboClaw roboclaw2(&Serial3, 10000);


static unsigned long msTime = 0;
static bool bearOff = true;
static bool mooseOff = true;
static bool cougarOff = true;

void setup() {
  //Open terminal and roboclaw serial ports
  terminal.begin(57600);
  roboclaw.begin(57600);
  roboclaw2.begin(57600);
  
  analogReference(EXTERNAL);

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
  turnCougarOff();
  
  delay(2500);
  initializeCougar();
}



void loop() {

  float c1, c2;
  int16_t i1 = 0, i2 = 0, v;

  int foo = analogRead(CONTROL_ANALOG_INPUT_COUGAR_PIN);

  terminal.print("POT Value: ");
  terminal.println((foo/1024.00)*5);
  stepCougar();
 
  /*
    
   v = roboclaw.ReadCurrents(RCAddress, i1, i2);

  c1 = (float) i1/100.0;
  c2 = (float) i2/100.0;

  terminal.print(v);
  terminal.print(" M1 current: ");
  terminal.print(i1);
  terminal.print(" M2 current: ");
  terminal.println(i2);
  */
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

    /*
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
    */
    
    if(mooseOff)
    {
      turnMooseOn();
      mooseOff = false;
    }
    
    stepMoose();
 
  }
  else
  {
    if(mooseOff == false){
      turnMooseOff();
      mooseOff = true;
    }
  }

  if(switchOn(CONTROL_SWITCH_COUGAR_PIN))
  {
    if(cougarOff)
    {
      turnCougarOn();
      cougarOff = false;
    }
    stepCougar();
  }
  else
  {
    turnCougarOff();
    cougarOff = true;
  }

}

